#include <iostream>
#include <string>
#include <filesystem>
#include <cstdlib> // For exit(0)
#include "Engine.h"
#include "Renderer.h"
#include "EngineUtils.h"
#include "SDL2/SDL.h"

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
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