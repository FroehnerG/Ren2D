#pragma once
#ifndef ENGINEUTILS_H
#define ENGINEUTILS_H

#include <string>
#include "rapidjson/document.h"
#include "glm/glm.hpp"

class EngineUtils {
public:
    static void ReadJsonFile(const std::string& path, rapidjson::Document& out_document);
    static uint64_t CreateCompositeKey(glm::ivec2 position);
    static uint64_t HashString(std::string name);
    static glm::ivec2 ParseCompositeKey(uint64_t composite_position);
    static std::string ObtainWordAfterPhrase(const std::string& input, const std::string& phrase);
};

#endif // ENGINEUTILS_H
