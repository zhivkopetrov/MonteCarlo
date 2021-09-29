//Corresponding header
#include "FBO.h"

//C system headers

//C++ system headers

//Other libraries headers

//Own components headers
#include "sdl/Renderer.h"

int32_t FBO::init(Renderer *renderer, const uint8_t rsrcId,
                  const SDL_Point startPoint, const int32_t FBOWidth,
                  const int32_t FBOHeight) {
  _renderer = renderer;
  drawParams.rsrcId = rsrcId;
  drawParams.frame = 0;
  drawParams.pos = startPoint;

  if (EXIT_SUCCESS != _renderer->getTextureContainer()->createEmptyTexture(
          FBOWidth, FBOHeight, rsrcId)) {
    fprintf(stderr, "Error, _textureContainer->createEmptyTexture() "
        "failed\n");

    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void FBO::unlockFBO() {
  _renderer->changeRendererTarget(drawParams.rsrcId);
}

void FBO::lockFBO() {
  _renderer->resetRendererTarget();
}

