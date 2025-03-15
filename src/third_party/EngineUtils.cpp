#include <iostream>
#include <fstream>
#include <stdio.h>

#include "EngineUtils.h"
#include "rapidjson/filereadstream.h"

void EngineUtils::ReadJsonFile(const std::string& path, rapidjson::Document& out_document) {
    FILE* file_pointer = nullptr;

#ifdef _WIN32
    fopen_s(&file_pointer, path.c_str(), "rb");
#else
    file_pointer = fopen(path.c_str(), "rb");
#endif

    if (!file_pointer) {
        std::cout << "Error: Unable to open JSON file: " << path << std::endl;
        return;
    }

    char buffer[65536];
    rapidjson::FileReadStream stream(file_pointer, buffer, sizeof(buffer));
    out_document.ParseStream(stream);
    std::fclose(file_pointer);

    if (out_document.HasParseError()) {
        std::cout << "Error parsing JSON at [" << path << "]" << std::endl;
        exit(1);
    }
}

uint64_t EngineUtils::CreateCompositeKey(glm::ivec2 position)
{
    // Use offsets to ensure negative values are correctly mapped
    uint32_t ux = static_cast<uint32_t>(position.x + INT32_MAX + 1);
    uint32_t uy = static_cast<uint32_t>(position.y + INT32_MAX + 1);

    uint64_t result = static_cast<uint64_t>(ux);
    result = (result << 32) | static_cast<uint64_t>(uy);

    return result;
}

uint64_t EngineUtils::HashString(std::string name)
{
    // Hash the string before inserting it
    std::hash<std::string> hasher;
    uint64_t key = hasher(name);

    return key;
}

glm::ivec2 EngineUtils::ParseCompositeKey(uint64_t composite_position)
{
    uint32_t uy = static_cast<uint32_t>(composite_position & 0xFFFFFFFF); // Extract lower 32 bits
    uint32_t ux = static_cast<uint32_t>(composite_position >> 32);        // Extract upper 32 bits

    // Reverse the offset added in CreateCompositeKey
    int x = static_cast<int>(ux) - (INT32_MAX + 1);
    int y = static_cast<int>(uy) - (INT32_MAX + 1);

    return glm::ivec2(x, y);
}

std::string EngineUtils::ObtainWordAfterPhrase(const std::string& input, const std::string& phrase)
{
    // Find the position of the phrase in the string
    size_t pos = input.find(phrase);

    // If phrase is not found, return an empty string
    if (pos == std::string::npos) return "";

    // Find the starting position of the next word (skip spaces after the phrase)
    pos += phrase.length();
    while (pos < input.size() && std::isspace(input[pos])) {
        ++pos;
    }

    // If we're at the end of the string, return an empty string
    if (pos == input.size()) return "";

    // Find the end position of the word (until a space or the end of the string)
    size_t endPos = pos;
    while (endPos < input.size() && !std::isspace(input[endPos])) {
        ++endPos;
    }

    // Extract and return the word
    return input.substr(pos, endPos - pos);
}



