//Corresponding header
#include "Renderer.h"

//C system headers

//C++ system headers
#include <cstdlib>

//Other libraries headers
#include <SDL_render.h>
#include <SDL_hints.h>

//Own components headers
#include "DrawParams.h"

int32_t Renderer::init(const int32_t windowX, const int32_t windowY,
                       const int32_t windowWidth, const int32_t windowHeight) {
  //Create window
  _window = SDL_CreateWindow("Batman", windowX, windowY, windowWidth,
      windowHeight, SDL_WINDOW_FULLSCREEN_DESKTOP);

  if (nullptr == _window) {
    fprintf(stderr, "Window could not be created! SDL Error: %s\n",
        SDL_GetError());

    return EXIT_FAILURE;
  }

  /** Set texture filtering to linear
   *                      (used for image scaling /pixel interpolation/ )
   * */
  if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
    fprintf(stderr, "Warning: Linear texture filtering not enabled! "
        "SDL_SetHint() failed. SDL Error: %s\n", SDL_GetError());

    return EXIT_FAILURE;
  }

  constexpr auto unspecifiedRendererDriver = -1;
  //Create renderer for window
  _sdlRenderer = SDL_CreateRenderer(_window, unspecifiedRendererDriver,
      (SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE));

  if (nullptr == _sdlRenderer) {
    fprintf(stderr, "Renderer could not be created! SDL Error: %s\n",
        SDL_GetError());

    return EXIT_FAILURE;
  }

  //Initialize renderer color to black
  if (EXIT_SUCCESS != SDL_SetRenderDrawColor(_sdlRenderer, 0, 0, 0,
          SDL_ALPHA_OPAQUE)) {
    fprintf(stderr, "Error in, SDL_SetRenderDrawColor(), "
        "SDL Error: %s\n", SDL_GetError());

    return EXIT_FAILURE;
  }

  if (EXIT_SUCCESS != _textureContainer.init(_sdlRenderer)) {
    fprintf(stderr, "Error in _textureContainer.init()");

    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void Renderer::deinit() {
  _textureContainer.deinit();

  if (_sdlRenderer) { //sanity check
    //Destroy window
    SDL_DestroyRenderer(_sdlRenderer);
    _sdlRenderer = nullptr;
  }

  if (_window) {    //sanity check
    SDL_DestroyWindow(_window);
    _window = nullptr;
  }
}

void Renderer::clearScreen() {
  if (EXIT_SUCCESS != SDL_RenderClear(_sdlRenderer)) {
    fprintf(stderr, "Error in, SDL_RenderClear(), SDL Error: %s\n",
        SDL_GetError());

    return;
  }
}

void Renderer::finishFrame() {
  SDL_Rect renderQuad = { 0, 0, 0, 0 };

  SDL_Rect sourceQuad = { 0, 0, 0, 0 };

  //create stach variable for better cache performance
  const int32_t USED_SIZE = _currWidgetCounter;

  //do the actual drawing of all stored images for THIS FRAME
  for (int32_t i = 0; i < USED_SIZE; ++i) {
    renderQuad.x = _widgets[i].pos.x;
    renderQuad.y = _widgets[i].pos.y;

    sourceQuad = _textureContainer.getTextureFrameRect(_widgets[i].rsrcId,
        _widgets[i].frame);

    renderQuad.w = sourceQuad.w;
    renderQuad.h = sourceQuad.h;

    if (EXIT_SUCCESS != SDL_RenderCopy(_sdlRenderer,
            _textureContainer.getTexture(_widgets[i].rsrcId), &sourceQuad,
            &renderQuad)) {
      fprintf(stderr, "Error in, SDL_RenderCopy(), SDL Error: %s\n",
          SDL_GetError());

      return;
    }
  }

  //reset widget counter
  _currWidgetCounter = 0;

  //------------- UPDATE SCREEN----------------
  SDL_RenderPresent(_sdlRenderer);
}

void Renderer::changeRendererTarget(const uint8_t rsrcId) {
  if (EXIT_SUCCESS != SDL_SetRenderTarget(_sdlRenderer,
          _textureContainer.getTexture(rsrcId))) {
    fprintf(stderr, "Error, default renderer target could not be set. "
        "SDL_SetRenderTarget() failed, SDL Error: %s\n", SDL_GetError());
  }
}

void Renderer::resetRendererTarget() {
  if (EXIT_SUCCESS != SDL_SetRenderTarget(_sdlRenderer, nullptr)) {
    fprintf(stderr, "Error, default renderer target could not be set. "
        "SDL_SetRenderTarget() failed, SDL Error: %s\n", SDL_GetError());
  }
}

void Renderer::drawPoints(const SDL_Point *points, const int32_t count) {
  //change to yellow color
  SDL_SetRenderDrawColor(_sdlRenderer, 255, 255, 0, SDL_ALPHA_OPAQUE);

  SDL_RenderDrawPoints(_sdlRenderer, points, count);

  //return to black color
  SDL_SetRenderDrawColor(_sdlRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
}

