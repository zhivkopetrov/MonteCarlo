#ifndef APPLICATION_H_
#define APPLICATION_H_

//C system headers

//C++ system headers
#include <cstdint>
#include <cmath>
#include <chrono>

//Other libraries headers
#include <SDL_events.h>

//Own components headers
#include "common/CommonDefines.h"
#include "common/CommonStructs.hpp"

#include "sdl/Renderer.h"
#include "sdl/Text.h"
#include "sdl/FBO.h"

//Forward declarations
struct Point;
struct MonteCarloArgs;

struct ApplicationCfg {
  uint32_t samplesCount = 2000000;
  bool showTexts = true;
};

class Application {
public:
  int32_t init(const ApplicationCfg &cfg);

  void deinit();

  void start();

private:
  int32_t initGraphics();

  void drawWorld(const std::vector<Point> &outSamples);

  bool isInBatman(const Point &point, const Point &origin,
                  const double scale) const;

  void generatePoints(const uint32_t windowWidth, const uint32_t windowHeight,
                      const uint32_t maxPoints);

  void monteCarlo(const MonteCarloArgs args);

  bool inOval(const Point &point, const Point &origin,
              const Point &ovalRadius) const;

  double calculateError(const MonteCarloArgs &args) const;

  void updateTexts(const MonteCarloArgs &args,
                   const std::chrono::high_resolution_clock::time_point &start);

  inline double ovalArea(const Point &ovalRadius) const {
    return M_PI * (ovalRadius.x * ovalRadius.y);
  }

  bool checkForExitRequest();

  void waitForExit();

  SDL_Event _inputEvent;

  Renderer _renderer;

  Text _texts[Textures::TEXTS_COUNT];

  FBO _pointsFBO; //frame buffer object

  std::vector<Point> _pointsToEvaluate;

  int32_t _totalEvaluatedPoints = 0;
  int32_t _pointsInOval = 0;
  int32_t _pointsInBatman = 0;

  bool _showTexts = false;
};

#endif /* APPLICATION_H_ */

