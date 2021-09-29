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
#include "Application.h"

static ApplicationCfg parseInput(int32_t argc, char *args[]) {
  ApplicationCfg cfg;
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

static int32_t runApplication(const ApplicationCfg& cfg) {
  Application app;

  if (EXIT_SUCCESS != app.init(cfg)) {
    fprintf(stderr, "app.init() failed\n");

    return EXIT_FAILURE;
  }

  app.start();

  app.deinit();

  return EXIT_SUCCESS;
}

int32_t main(int32_t argc, char *args[]) {
  if (EXIT_SUCCESS != SDLLoader::init()) {
    fprintf(stderr, "Error in SDLLoader::init() -> Terminating ...\n");

    return EXIT_FAILURE;
  }

  const auto appCfg = parseInput(argc, args);
  if (EXIT_SUCCESS != runApplication(appCfg)) {
    fprintf(stderr, "runApplication() failed\n");

    return EXIT_FAILURE;
  }

  //close SDL libraries
  SDLLoader::deinit();

  return EXIT_SUCCESS;
}

