//Corresponding header
#include "SDLLoader.h"

//C system headers

//C++ system headers
#include <cstdio>

//Other libraries headers
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

//Own components headers

int32_t SDLLoader::init() {
  int32_t err = EXIT_SUCCESS;

  if (EXIT_SUCCESS == err) {
    if (-1 == TTF_Init()) {
      fprintf(stderr, "SDL_ttf could not initialize! SDL_ttf Error: %s\n",
      TTF_GetError());

      err = EXIT_FAILURE;
    }
  }

  if (EXIT_SUCCESS == err) {
    if (0 > SDL_Init(SDL_INIT_VIDEO)) {
      fprintf(stderr, "SDL could not be initialised! SDL Error: %s\n",
          SDL_GetError());

      err = EXIT_FAILURE;
    } else {
      //Initialise PNG loading
      const int32_t imgFlags = IMG_INIT_PNG;
      if (! (IMG_Init(imgFlags) & imgFlags)) {
        fprintf(stderr, "SDL_image could not be initialised! "
            "SDL_image Error: %s\n", IMG_GetError());

        err = EXIT_FAILURE;
      }
    }
  }

  if (EXIT_SUCCESS == err) {
    if (0 > SDL_Init(SDL_INIT_TIMER)) {
      fprintf(stderr, "SDL TIMER could not be initialised! "
          "SDL Error: %s\n", SDL_GetError());

      err = EXIT_FAILURE;
    }
  }

  return err;
}

void SDLLoader::deinit() {
  //Quit SDL subsystems
  IMG_Quit();
  TTF_Quit();
  SDL_Quit();
}

