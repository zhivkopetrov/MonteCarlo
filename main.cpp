//C system headers

//C++ system headers
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <string>

//Other libraries headers
#include "sdl/SDLLoader.h"

//Own components headers
#include "Game.h"

int32_t main(int32_t argc, char *args[]) {
  int32_t err = EXIT_SUCCESS;

  uint32_t samplesCount = 500000;
  if (1 < argc) {
    samplesCount = static_cast<uint32_t>(std::stoi(args[1]));
  }

  Game game;

  if (EXIT_SUCCESS != SDLLoader::init()) {
    fprintf(stderr, "Error in SDLLoader::init() -> Terminating ...\n");

    err = EXIT_FAILURE;
  }

  if (EXIT_SUCCESS == err) {
    if (EXIT_SUCCESS != game.init(samplesCount)) {
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

