#include <iostream>
#include <filesystem>
#include <cstdlib> // For exit(0)
#include "Engine.h"
#include "EngineUtils.h"

namespace fs = std::filesystem;

int main() {
	if (!fs::exists("resources")) {
		std::cout << "error: resources/ missing";
		exit(0);  // Exit immediately with code 0
	}

	if (!fs::exists("resources/game.config")) {
		std::cout << "error: resources/game.config missing";
		exit(0);  // Exit immediately with code 0
	}

	rapidjson::Document game_config;

	EngineUtils::ReadJsonFile("resources/game.config", game_config);

	Engine engine(game_config);

	if (fs::exists("resources/rendering.config")) {
		rapidjson::Document rendering_config;
		EngineUtils::ReadJsonFile("resources/rendering.config", rendering_config);

		engine.InitResolution(rendering_config);
	}

	engine.GameLoop();

	return 0;
}