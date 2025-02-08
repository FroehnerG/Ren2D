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
    static std::pair<int, int> ParseCompositeKey(uint64_t composite_position);
};

#endif // ENGINEUTILS_H
