#ifndef SDL_FBO_H_
#define SDL_FBO_H_

//C system headers

//C++ system headers
#include <cstdint>

//Other libraries headers

//Own components headers
#include "sdl/DrawParams.h"

//Forward declarations
class Renderer;

class FBO {
public:
  FBO();
  ~FBO() = default;

  int32_t init(Renderer *renderer, const uint8_t rsrcId,
               const SDL_Point startPoint, const int32_t FBOWidth,
               const int32_t FBOHeight);

  DrawParams drawParams;

  void unlockFBO();

  void lockFBO();

private:
  Renderer *_renderer;
};

#endif /* SDL_FBO_H_ */

