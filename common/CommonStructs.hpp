#ifndef COMMON_COMMONSTRUCTS_HPP_
#define COMMON_COMMONSTRUCTS_HPP_

//C system headers

//C++ system headers
#include <cstdint>

//Other libraries headers

//Own components headers

//Forward declarations

struct Point {
  Point(const double inputX, const double inputY) {
    x = inputX;
    y = inputY;
  }

  Point() {
    x = 0;
    y = 0;
  }

  double x;
  double y;
};

struct MonteCarloArgs {
  Point animationCenter;
  double animationScale;
  Point ovalRadius;
};

#endif /* COMMON_COMMONSTRUCTS_HPP_ */
