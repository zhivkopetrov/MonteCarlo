#include "FBO.h"

//Corresponding header
#include "Text.h"

//C system headers

//C++ system headers

//Other libraries headers

//Own components headers
#include "sdl/Renderer.h"

FBO::FBO()
    : _renderer(nullptr) {

}

int32_t FBO::init(Renderer *renderer, const uint8_t rsrcId,
                  const SDL_Point startPoint, const int32_t FBOWidth,
                  const int32_t FBOHeight) {
  int32_t err = EXIT_SUCCESS;

  _renderer = renderer;
  drawParams.rsrcId = rsrcId;
  drawParams.frame = 0;
  drawParams.pos = startPoint;

  if (EXIT_SUCCESS != _renderer->getTextureContainer()->createEmptyTexture(
          FBOWidth, FBOHeight, rsrcId)) {
    fprintf(stderr, "Error, _textureContainer->createEmptyTexture() "
        "failed\n");

    err = EXIT_FAILURE;
  }

  return err;
}

void FBO::unlockFBO() {
  _renderer->changeRendererTarget(drawParams.rsrcId);
}

void FBO::lockFBO() {
  _renderer->resetRendererTarget();
}

