#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <utility>
#include <optional>
#include "rapidjson/document.h"

using namespace std;

enum class Comparison {
	Equal,
	NotEqual,
	Greater,
	Less,
	GreaterEqual,
	LessEqual
};

enum class Operation {
	Set,     // =
	Increment,  // +=
	Decrement,  // -=
	Multiply,   // *=
	Divide      // /=
};

struct Flag {
	string flag_name = "";
	optional<bool> bool_value;
	optional<int> int_value;
	Operation op = Operation::Set;
};

struct IntModification {
	Flag* flag_ptr;
	int value;
	Operation op;
};

struct Event {
	string event_name = "";
	vector<Flag*> flags;
	vector<optional<bool>> desired_bool_value;
	vector<optional<int>> desired_int_value;
	vector<Comparison> int_comparisons;
	unordered_map<int, Flag> set_flags;
};

class EventListener {
public:
	void LoadFlags(rapidjson::Document& flags_json);
	bool EventCheck(string event_name);
	void UpdateFlags(Event* event);
	void ParseAndLoadDialogueDSL(const std::string& dsl, const std::string& event_name);
	void LoadScoreAndHealth(int* health_in, int* score_in);
	static int HashStringToInt(const std::string& str);
private:
	unordered_map<int, Flag> flag_id_to_flag;
	unordered_map<int, Event> event_id_to_event;
	int* score = nullptr;
	int* health = nullptr;
};