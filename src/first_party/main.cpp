#include <iostream>
#include <filesystem>
#include <cstdlib> // For exit(0)
#include "Engine.h"
#include "EngineUtils.h"

namespace fs = std::filesystem;

int main() {
	if (!fs::exists("resources")) {
		std::cout << "resources/ missing";
		exit(0);  // Exit immediately with code 0
	}

	if (!fs::exists("resources/game.config")) {
		std::cout << "resources/game.config missing";
		exit(0);  // Exit immediately with code 0
	}

	rapidjson::Document config;

	EngineUtils::ReadJsonFile("resources/game.config", config);

	Engine engine(config);

	engine.GameLoop();

	return 0;
}