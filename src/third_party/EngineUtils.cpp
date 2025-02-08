#include "EngineUtils.h"
#include <iostream>
#include <fstream>
#include <stdio.h>

#include "rapidjson/filereadstream.h"

void EngineUtils::ReadJsonFile(const std::string& path, rapidjson::Document& out_document) {
    FILE* file_pointer = nullptr;

#ifdef _WIN32
    fopen_s(&file_pointer, path.c_str(), "rb");
#else
    file_pointer = fopen(path.c_str(), "rb");
#endif

    if (!file_pointer) {
        std::cerr << "Error: Unable to open JSON file: " << path << std::endl;
        return;
    }

    char buffer[65536];
    rapidjson::FileReadStream stream(file_pointer, buffer, sizeof(buffer));
    out_document.ParseStream(stream);
    std::fclose(file_pointer);

    if (out_document.HasParseError()) {
        std::cerr << "Error parsing JSON at [" << path << "]" << std::endl;
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

glm::ivec2 EngineUtils::ParseCompositeKey(uint64_t composite_position)
{
    uint32_t uy = static_cast<uint32_t>(composite_position & 0xFFFFFFFF); // Extract lower 32 bits
    uint32_t ux = static_cast<uint32_t>(composite_position >> 32);        // Extract upper 32 bits

    // Reverse the offset added in CreateCompositeKey
    int x = static_cast<int>(ux) - (INT32_MAX + 1);
    int y = static_cast<int>(uy) - (INT32_MAX + 1);

    return glm::ivec2(x, y);
}

