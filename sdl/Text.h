#ifndef GAMEENTITIES_TEXT_H_
#define GAMEENTITIES_TEXT_H_

//C system headers

//C++ system headers
#include <cstdint>

//Other libraries headers

//Own components headers
#include "sdl/DrawParams.h"

//Forward declarations
class TextureContainer;

class Text {
public:
  Text();
  ~Text() = default;

  int32_t init(TextureContainer *textureContainer, const uint8_t rsrcId,
               const SDL_Point startPoint, const char *startText,
               const int32_t fontSize);

  void setText(const char *text);

  DrawParams drawParams;

private:
  TextureContainer *_textureContainer;

  int32_t _fontSize;

  int32_t _width;
  int32_t _height;
};

#endif /* GAMEENTITIES_TEXT_H_ */

