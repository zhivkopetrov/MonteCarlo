//C system headers

//C++ system headers
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <stdexcept>

//Other libraries headers
#include "sdl/SDLLoader.h"

//Own components headers
#include "Game.h"

static GameCfg parseInput(int32_t argc, char *args[]) {
  GameCfg cfg;
  if (1 == argc) {
    return cfg;
  }

  try {
    cfg.samplesCount = static_cast<uint32_t>(std::stoi(args[1]));
  } catch(const std::invalid_argument & ex) {
    fprintf(stderr, "Error, bad args provided. Second parameter should be "
        "a number. Using the default config number\n");

    cfg.showTexts = (std::string(args[1]) == "--show-texts=no") ? false : true;
  }

  if (3 == argc) {
    cfg.showTexts = (std::string(args[2]) == "--show-texts=no") ? false : true;
  }

  return cfg;
}

int32_t main(int32_t argc, char *args[]) {
  int32_t err = EXIT_SUCCESS;

  if (EXIT_SUCCESS != SDLLoader::init()) {
    fprintf(stderr, "Error in SDLLoader::init() -> Terminating ...\n");

    err = EXIT_FAILURE;
  }

  const GameCfg gameCfg = parseInput(argc, args);
  Game game;

  if (EXIT_SUCCESS == err) {
    if (EXIT_SUCCESS != game.init(gameCfg)) {
      fprintf(stderr, "game.init() failed\n");

      err = EXIT_FAILURE;
    } else {
      game.start();
    }
  }

  game.deinit();

  //close SDL libraries
  SDLLoader::deinit();

  return err;
}

