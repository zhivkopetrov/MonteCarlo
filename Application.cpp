//Corresponding header
#include "Application.h"

//C system headers

//C++ system headers
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <iomanip>
#include <random>
#include <thread>

//Other libraries headers

//Own components headers

namespace {
constexpr int32_t MONITOR_WIDTH = 1920;
constexpr int32_t MONITOR_HEIGHT = 1080;

constexpr int32_t DRAW_STEP = 100;

constexpr double HASH_1 = (6 * sqrt(10)) / 7;
constexpr double HASH_2 = HASH_1 / 2;
constexpr double HASH_3 = ( (3 * sqrt(33)) - 7) / 112;
}

using Time = std::chrono::high_resolution_clock;

int32_t Application::init(const ApplicationCfg &cfg) {
  _showTexts = cfg.showTexts;
  memset(&_inputEvent, 0, sizeof (_inputEvent));

  if ( EXIT_SUCCESS != initGraphics()) {
    fprintf( stderr, "Error, initGraphics() failed\n");

    return EXIT_FAILURE;
  }

  generatePoints(MONITOR_WIDTH, MONITOR_HEIGHT, cfg.samplesCount);

  return EXIT_SUCCESS;
}

void Application::deinit() {
  _renderer.deinit();
}

void Application::start() {
  const double X_RADIUS = MONITOR_WIDTH / 2;
  const double Y_RADIUS = X_RADIUS / 2;

  MonteCarloArgs args;
  args.animationCenter = Point(MONITOR_WIDTH / 2, MONITOR_HEIGHT / 2);
  args.animationScale = 120.0;
  args.ovalRadius = Point(X_RADIUS, Y_RADIUS);

  monteCarlo(args);

  waitForExit();
}

int32_t Application::initGraphics() {
  if ( EXIT_SUCCESS != _renderer.init(0, 0, MONITOR_WIDTH, MONITOR_HEIGHT)) {
    fprintf( stderr, "Error in _renderer.init() \n");

    return EXIT_FAILURE;
  }

  if ( EXIT_SUCCESS != _pointsFBO.init(&_renderer, Textures::VBO, SDL_Point { 0,
      0 }, MONITOR_WIDTH, MONITOR_HEIGHT)) {
    fprintf( stderr, "Error in _pointsVBO.init()\n");

    return EXIT_FAILURE;
  }

  if ( EXIT_SUCCESS
      != _texts[Textures::TIME].init(_renderer.getTextureContainer(),
          Textures::TIME, SDL_Point { 20, 20 }, "Time spent: 0 ms",
          FontSize::SMALL)) {
    fprintf( stderr, "Error in _texts[Textures::TIME].init()\n");

    return EXIT_FAILURE;
  }

  if ( EXIT_SUCCESS
      != _texts[Textures::ALL_POINTS].init(_renderer.getTextureContainer(),
          Textures::ALL_POINTS, SDL_Point { 1500, 20 }, "Points: ",
          FontSize::SMALL)) {
    fprintf( stderr, "Error in _texts[Textures::TIME].init()\n");

    return EXIT_FAILURE;
  }

  if ( EXIT_SUCCESS
      != _texts[Textures::ERROR].init(_renderer.getTextureContainer(),
          Textures::ERROR, SDL_Point { 20, 1020 }, "Error: 0.0%",
          FontSize::SMALL)) {
    fprintf( stderr, "Error in _texts[Textures::TIME].init()\n");

    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void Application::drawWorld(const std::vector<Point> &outSamples) {
  _renderer.clearScreen();

  _pointsFBO.unlockFBO();

  SDL_Point arr[DRAW_STEP];
  for (int32_t i = 0; i < DRAW_STEP; ++i) {
    arr[i].x = static_cast<int32_t>(outSamples[i].x);
    arr[i].y = static_cast<int32_t>(outSamples[i].y);
  }

  _renderer.drawPoints(arr, DRAW_STEP);

  _pointsFBO.lockFBO();

  _renderer.drawTexture(&_pointsFBO.drawParams);

  if (_showTexts) {
    for (uint8_t i = 0; i < Textures::TEXTS_COUNT; ++i) {
      _renderer.drawTexture(&_texts[i].drawParams);
    }
  }

  _renderer.finishFrame();
}

bool Application::isInBatman(const Point &point, const Point &origin,
                             const double scale) const {
  const double POS_X = (point.x - origin.x) / scale;
  const double POS_Y = (point.y - origin.y) / scale;
  double tempX = 0.0;
  double tempY = 0.0;

  if (POS_Y < 0.0) {
    /* left upper wing */
    if (POS_X <= -3) {
      tempX = (-7 * sqrt(1 - ( (POS_Y * POS_Y) / 9.0)));
      return POS_X >= tempX ? true : false;
    }

    /* left shoulder */
    if (POS_X > -3.0 && POS_X <= -1.0) {
      tempX = -POS_X;
      const double LOC_HASH = fabs(tempX) - 1;
      tempY = - (HASH_1 + (1.5 - 0.5 * tempX))
          + HASH_2 * sqrt(4.0 - (LOC_HASH * LOC_HASH));
      return POS_Y > tempY ? true : false;
    }

    /* exterior left ear */
    if (POS_X > -1.0 && POS_X <= -0.75) {
      tempY = 9.0 + 8.0 * POS_X;
      return POS_Y > -tempY ? true : false;
    }

    /* interior left ear */
    if (POS_X > -0.75 && POS_X <= -0.5) {
      tempY = -3 * POS_X + 0.75;
      return POS_Y > -tempY ? true : false;
    }

    /* top of head */
    if (POS_X > -0.5 && POS_X <= 0.5) {
      tempY = 2.25;
      return POS_Y > -tempY ? true : false;
    }

    /* interior right ear */
    if (POS_X > 0.5 && POS_X <= 0.75) {
      tempY = 3 * POS_X + 0.75;
      return POS_Y > -tempY ? true : false;
    }

    /* exterior right ear */
    if (POS_X > 0.75 && POS_X <= 1.0) {
      tempY = 9.0 - 8 * POS_X;
      return POS_Y > -tempY ? true : false;
    }

    /* right shoulder */
    if (POS_X <= 3.0 && POS_X > 1.0) {
      const double LOC_HASH = fabs(POS_X) - 1.0;
      tempY = - (HASH_1 + (1.5 - 0.5 * POS_X))
          + HASH_2 * sqrt(4.0 - (LOC_HASH * LOC_HASH));
      return POS_Y > tempY ? true : false;
    }

    /* right upper wing */
    if (POS_X > 3.0) {
      tempX = (7.0 * sqrt(1 - ( (POS_Y * POS_Y) / 9.0)));
      return POS_X <= tempX ? true : false;
    }
  }
  if (POS_Y >= 0) {
    /* bottom left wing */
    if (POS_X <= -4.0) {
      tempX = (-7 * sqrt(1 - ( (POS_Y * POS_Y) / 9.0)));
      return POS_X >= tempX ? true : false;
    }

    /* bottom wing */
    if (POS_X > -4.0 && POS_X <= 4.0) {
      const double LOC_HASH = fabs(fabs(POS_X) - 2.0) - 1.0;
      tempY = (fabs(POS_X / 2) - (HASH_3 * POS_X * POS_X) - 3.0)
          + sqrt(1 - (LOC_HASH * LOC_HASH));
      tempY *= -1.0;
      return POS_Y < tempY ? true : false;
    }

    /* bottom right wing */
    if (POS_X >= 4.0) {
      tempX = (7.0 * sqrt(1 - ( (POS_Y * POS_Y) / 9.0)));
      return POS_X <= tempX ? true : false;
    }
  }

  return false;
}

void Application::generatePoints(const uint32_t windowWidth,
                                 const uint32_t windowHeight,
                                 const uint32_t maxPoints) {
  //reserve enough memory for all points so no unneeded reallocation
  //occur at run-time
  _pointsToEvaluate.reserve(maxPoints);

  Point currPoint;
  uint32_t totalPointsGenerated = 0;

  std::random_device rd; /* seed for the pseudo random engine */
  std::mt19937 range(rd()); /* mersenne_twister engine */
  /* we NEED uniform distribution for Monte Carlo */
  std::uniform_real_distribution<> distr(0, 1.0);

  while (totalPointsGenerated < maxPoints) {
    ++totalPointsGenerated;
    currPoint.x = distr(range) * windowWidth;
    currPoint.y = distr(range) * windowHeight;

    _pointsToEvaluate.emplace_back(currPoint);
  }
}

void Application::monteCarlo(const MonteCarloArgs args) {
  std::chrono::high_resolution_clock::time_point start = Time::now();

  std::vector<Point> outSamples(DRAW_STEP);
  int32_t currOutSampleIdx = 0;

  for (const auto &point : _pointsToEvaluate) {
    ++_totalEvaluatedPoints;
    if (!inOval(point, args.animationCenter, args.ovalRadius)) {
      continue;
    }

    ++_pointsInOval;

    if (isInBatman(point, args.animationCenter, args.animationScale)) {
      ++_pointsInBatman;
      continue;
    }

    //remember only points outside of target
    outSamples[currOutSampleIdx] = point;
    ++currOutSampleIdx;

    //update the draw target only once every DRAW_STEP
    if (currOutSampleIdx < DRAW_STEP) {
      continue;
    }

    if (checkForExitRequest()) {
      return;
    }

    updateTexts(args, start);
    drawWorld(outSamples);
    currOutSampleIdx = 0;
  }

  //perform the final draw
  updateTexts(args, start);
  drawWorld(outSamples);
}

bool Application::inOval(const Point &point, const Point &origin,
                         const Point &ovalRadius) const {
  const double posX = point.x - origin.x;
  const double posY = point.y - origin.y;
  const double deltaX = posX / ovalRadius.x;
  const double deltaY = posY / ovalRadius.y;

  return ( (deltaX * deltaX) + (deltaY * deltaY) <= 1.0) ? true : false;
}

void Application::updateTexts(
    const MonteCarloArgs &args,
    const std::chrono::high_resolution_clock::time_point &start) {
  if (!_showTexts) {
    return;
  }

  std::string content;
  content.reserve(50);

  content = "Time spent: ";
  content.append(
      std::to_string(
          std::chrono::duration_cast<std::chrono::milliseconds>(
              Time::now() - start).count()));
  content.append(" ms");
  _texts[Textures::TIME].setText(content.c_str());

  content = "Points: ";
  content.append(std::to_string(_totalEvaluatedPoints));
  _texts[Textures::ALL_POINTS].setText(content.c_str());

  content = "Error: ";

  std::ostringstream ostr;
  ostr << std::fixed << std::setprecision(3) << calculateError(args);

  content.append(ostr.str());
  content.append("%");
  _texts[Textures::ERROR].setText(content.c_str());
}

double Application::calculateError(const MonteCarloArgs &args) const {
  constexpr double MATH_AREA = 48.4243597;
  static const double REAL_AREA = MATH_AREA * args.animationScale
                                  * args.animationScale;
  static const double OVAL_AREA = ovalArea(args.ovalRadius);

  const double AREA_DIFF =
      fabs( ( (_pointsInBatman / static_cast<double>(
                  _pointsInOval)) * OVAL_AREA) - REAL_AREA);

  /* *100 to get the error in percents */
  return ( (AREA_DIFF / REAL_AREA) * 100.0);
}

bool Application::checkForExitRequest() {
  while (0 != SDL_PollEvent(&_inputEvent)) {
    if ( (SDL_KEYDOWN == _inputEvent.type
        && SDLK_ESCAPE == _inputEvent.key.keysym.sym)
        || SDL_QUIT == _inputEvent.type) {
      return true;
    }
  }
  return false;
}

void Application::waitForExit() {
  using namespace std::literals;

  while (true) {
    if (checkForExitRequest()) {
      break;
    }

    std::this_thread::sleep_for(20ms);
  }
}

