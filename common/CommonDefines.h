#ifndef COMMON_COMMONDEFINES_H_
#define COMMON_COMMONDEFINES_H_

//C system headers

//C++ system headers
#include <cstdint>

//Other libraries headers

//Own components headers

//Forward declarations

namespace Textures {
enum : uint8_t {
  TIME, ALL_POINTS, ERROR, VBO,

  COUNT, TEXTS_COUNT = 3
};
}

namespace FontSize {
enum {
  SMALL, BIG
};
}

#endif /* COMMON_COMMONDEFINES_H_ */

