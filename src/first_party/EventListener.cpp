#include <regex>
#include <iostream>
#include <sstream>
#include "EventListener.h"
#include "EngineUtils.h"

void EventListener::LoadFlags(rapidjson::Document& flags_json)
{
	for (auto& flag_json : flags_json["flags"].GetArray()) {
		Flag flag;

		string name = flag_json["name"].GetString();

		flag.flag_name = name;

		if (flag_json.HasMember("bool_value") && flag_json["bool_value"].IsBool())
			flag.bool_value = flag_json["bool_value"].GetBool();

		if (flag_json.HasMember("int_value") && flag_json["int_value"].IsInt())
			flag.int_value = flag_json["int_value"].GetInt();

		int id = HashStringToInt(name);

		flag_id_to_flag[id] = flag;
	}
}

bool EventListener::EventCheck(string event_name)
{
    int event_id = HashStringToInt(event_name);

    auto it = event_id_to_event.find(event_id);
    if (it == event_id_to_event.end()) {
        return false;
    }

    Event* event = &it->second;

	// check if flags fulfill event conditions
    for (size_t i = 0; i < event->flags.size(); ++i) {
        Flag* flag = event->flags[i];
        if (!flag) return false;

        // check bool condition
        if (event->desired_bool_value[i].has_value()) {
            if (!flag->bool_value.has_value() ||
                flag->bool_value.value() != event->desired_bool_value[i].value()) {
                return false;
            }
        }

        // check integer condition
        if (event->desired_int_value[i].has_value()) {
            if (!flag->int_value.has_value()) {
                return false;
            }

			int actual;

			if (flag->flag_name == "score") {
				actual = *score;
			}
			else if (flag->flag_name == "health") {
				actual = *health;
			}
			else {
				actual = flag->int_value.value();
			}

            int expected = event->desired_int_value[i].value();
            Comparison cmp = event->int_comparisons[i];

            switch (cmp) {
            case Comparison::Equal:        if (actual != expected) return false; break;
            case Comparison::NotEqual:     if (actual == expected) return false; break;
            case Comparison::Greater:      if (actual <= expected) return false; break;
            case Comparison::Less:         if (actual >= expected) return false; break;
            case Comparison::GreaterEqual: if (actual < expected) return false; break;
            case Comparison::LessEqual:    if (actual > expected) return false; break;
            }
        }
    }
	
	UpdateFlags(event);

    return true;
}

void EventListener::UpdateFlags(Event* event) {
	// assign set flags to event pointer flags if all checks pass or if no checks
	for (const auto& set_flag_pair : event->set_flags) {
		int flag_id = set_flag_pair.first;
		const Flag& set_flag = set_flag_pair.second;

		auto it = flag_id_to_flag.find(flag_id);
		if (it == flag_id_to_flag.end() && set_flag.flag_name != "health" && set_flag.flag_name != "score") continue;

		Flag& flag = it->second;

		// bool update
		if (set_flag.bool_value.has_value()) {
			flag.bool_value = set_flag.bool_value;
			continue; // skip int logic if this is a bool
		}

		// int update
		if (!set_flag.int_value.has_value()) continue;

		int change = set_flag.int_value.value();
		if (!flag.int_value.has_value()) flag.int_value = 0;

		int* int_value;

		if (set_flag.flag_name == "score") {
			int_value = score;
		}
		else if (set_flag.flag_name == "health") {
			int_value = health;
		}
		else {
			int_value = &(*flag.int_value);
		}

		switch (set_flag.op) {
		case Operation::Set:       *int_value = change; break;
		case Operation::Increment: *int_value = *int_value + change; break;
		case Operation::Decrement: *int_value = *int_value - change; break;
		case Operation::Multiply:  *int_value = *int_value * change; break;
		case Operation::Divide:
			if (change != 0) *int_value = *int_value / change;
			break;
		}
	}
}

void EventListener::ParseAndLoadDialogueDSL(const string& dsl, const string& event_name) {
	int event_id = HashStringToInt(event_name);
	Event& event = event_id_to_event[event_id];
	event.event_name = event_name;

	string text = dsl;

	regex if_regex(R"(\[if ([^\]]+)\])");
	regex set_regex(R"(\[flag ([^\]]+)\])");
	regex op_pattern(R"(([^=+\-*/]+)\s*(\+=|\-=|\*=|/=|=)\s*(.+))");
	smatch match;

	// --------- [if ...] ----------
	if (regex_search(text, match, if_regex)) {
		string condition_block = match[1];
		text = regex_replace(text, if_regex, "");

		stringstream ss(condition_block);
		string token;

		while (getline(ss, token, ',')) {
			token = regex_replace(token, regex(R"(^\s+|\s+$)"), "");

			Comparison comp = Comparison::Equal;
			string flag_name;
			optional<bool> bool_val;
			optional<int> int_val;

			// Parse different formats
			if (token[0] == '!') {
				flag_name = token.substr(1);
				bool_val = false;
			}
			else if (token.find(">=") != string::npos) {
				comp = Comparison::GreaterEqual;
				size_t pos = token.find(">=");
				flag_name = token.substr(0, pos);
				int_val = stoi(token.substr(pos + 2));
			}
			else if (token.find("<=") != string::npos) {
				comp = Comparison::LessEqual;
				size_t pos = token.find("<=");
				flag_name = token.substr(0, pos);
				int_val = stoi(token.substr(pos + 2));
			}
			else if (token.find("==") != string::npos) {
				comp = Comparison::Equal;
				size_t pos = token.find("==");
				flag_name = token.substr(0, pos);
				int_val = stoi(token.substr(pos + 2));
			}
			else if (token.find(">") != string::npos) {
				comp = Comparison::Greater;
				size_t pos = token.find(">");
				flag_name = token.substr(0, pos);
				int_val = stoi(token.substr(pos + 1));
			}
			else if (token.find("<") != string::npos) {
				comp = Comparison::Less;
				size_t pos = token.find("<");
				flag_name = token.substr(0, pos);
				int_val = stoi(token.substr(pos + 1));
			}
			else {
				flag_name = token;
				bool_val = true;
			}

			flag_name = regex_replace(flag_name, regex(R"(^\s+|\s+$)"), "");

			int flag_id = HashStringToInt(flag_name);

			// Ensure flag exists
			if (flag_id_to_flag.find(flag_id) == flag_id_to_flag.end()) {
				flag_id_to_flag[flag_id] = Flag{ flag_name };
			}

			Flag* flag_ptr = &flag_id_to_flag[flag_id];

			event.flags.push_back(flag_ptr);
			event.desired_bool_value.push_back(bool_val);
			event.desired_int_value.push_back(int_val);
			event.int_comparisons.push_back(comp);
		}
	}

	smatch flag_match;

	// --------- [flag ...] ----------
	if (regex_search(text, flag_match, set_regex)) {
		string set_block = flag_match[1];
		text = regex_replace(text, set_regex, ""); // remove [flag ...] from the text

		stringstream ss(set_block);
		string token;

		while (getline(ss, token, ',')) {
			token = regex_replace(token, regex(R"(^\s+|\s+$)"), "");

			// Match format: flag_name <op> value
			regex op_pattern(R"(([^=+\-*/]+)\s*(\+=|\-=|\*=|/=|=)\s*(.+))");
			smatch token_match;

			if (regex_match(token, token_match, op_pattern)) {
				string set_flag_name = regex_replace(token_match[1].str(), regex(R"(^\s+|\s+$)"), "");
				string op_str = token_match[2];
				string value_str = regex_replace(token_match[3].str(), regex(R"(^\s+|\s+$)"), "");

				int flag_id = HashStringToInt(set_flag_name);

				Flag& set_flag = event.set_flags[flag_id];
				set_flag.flag_name = set_flag_name;

				// Determine operation type
				if (op_str == "+=") set_flag.op = Operation::Increment;
				else if (op_str == "-=") set_flag.op = Operation::Decrement;
				else if (op_str == "*=") set_flag.op = Operation::Multiply;
				else if (op_str == "/=") set_flag.op = Operation::Divide;
				else set_flag.op = Operation::Set;

				// Assign value
				if (value_str == "true" || value_str == "false") {
					set_flag.bool_value = (value_str == "true");
					set_flag.int_value = nullopt;
				}
				else {
					set_flag.int_value = stoi(value_str);
					set_flag.bool_value = nullopt;
				}
			}
		}
	}
}

void EventListener::LoadScoreAndHealth(int* score_in, int* health_in)
{
	score = score_in;
	health = health_in;
}

int EventListener::HashStringToInt(const string& str) {
	unsigned long hash = 5381; // Starting seed (a large prime)

	for (char c : str) {
		hash = ((hash << 5) + hash) + static_cast<unsigned char>(c); // hash * 33 + c
	}

	return static_cast<int>(hash);
}
