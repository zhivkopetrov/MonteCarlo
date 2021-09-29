//Corresponding header
#include "TextureContainer.h"

//C system headers

//C++ system headers
#include <cstdlib>

//Other libraries headers
#include <SDL_render.h>
#include <SDL_video.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

//Own components headers
#include "common/CommonDefines.h"

TextureContainer::TextureContainer() {
  _fontSmall = nullptr;
  _fontBig = nullptr;
  _renderer = nullptr;

  //set green color for text
  _color.r = 255;
  _color.g = 0;
  _color.b = 0;
  _color.a = 255;
}

int32_t TextureContainer::init(SDL_Renderer *renderer) {
  _renderer = renderer;

  _textures.resize(Textures::COUNT, nullptr);

  _textureFrameRects.resize(Textures::COUNT);

  if (EXIT_SUCCESS != loadTextures()) {
    fprintf(stderr, "Error in loadTextures()\n");

    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void TextureContainer::deinit() {
  if (_fontSmall) //sanity check
  {
    TTF_CloseFont(_fontSmall);
    _fontSmall = nullptr;
  }

  if (_fontBig) //sanity check
  {
    TTF_CloseFont(_fontBig);
    _fontBig = nullptr;
  }

  for (SDL_Texture *texture : _textures) {
    SDL_DestroyTexture(texture);
    texture = nullptr;
  }

  _textures.clear();
}

void TextureContainer::setText(const char *text, const int32_t fontSize,
                               const uint8_t textureId, int32_t *outTextWidth,
                               int32_t *outTextHeight) {
  TTF_Font *font = nullptr;

  FontSize::SMALL == fontSize ? font = _fontSmall : font = _fontBig;

  SDL_Surface *loadedSurface = TTF_RenderText_Blended(font, text, _color);

  if (loadedSurface == nullptr) {
    fprintf(stderr, "Unable to load image! SDL_image Error: %s\n",
    IMG_GetError());

    return;
  }

  *outTextWidth = loadedSurface->w;
  *outTextHeight = loadedSurface->h;

  _textureFrameRects[textureId][0].w = loadedSurface->w;
  _textureFrameRects[textureId][0].h = loadedSurface->h;

  //create hardware accelerated texture
  if (EXIT_SUCCESS != loadTextureFromSurface(loadedSurface,
          _textures[textureId])) {
    fprintf(stderr, "Unable to create text texture for ID: %hhu\n", textureId);

    return;
  }
}

int32_t TextureContainer::createEmptyTexture(const int32_t textureWidth,
                                             const int32_t textureHeight,
                                             const uint8_t textureId) {
  int32_t err = EXIT_SUCCESS;

  _textures[textureId] = SDL_CreateTexture(_renderer,        //hardware renderer
      SDL_PIXELFORMAT_RGBA8888, //format
      SDL_TEXTUREACCESS_TARGET, //access
      textureWidth,             //texture width
      textureHeight);           //texture height

  if (nullptr == _textures[textureId]) {
    fprintf(stderr, "SDL_CreateTexture() failed: %s\n", SDL_GetError());

    err = EXIT_FAILURE;
  } else {
    _textureFrameRects[textureId][0].w = textureWidth;
    _textureFrameRects[textureId][0].h = textureHeight;

    SDL_SetRenderTarget(_renderer, _textures[textureId]);
    SDL_RenderClear(_renderer);
    SDL_SetRenderTarget(_renderer, nullptr);
  }

  return err;
}

int32_t TextureContainer::loadTextures() {
  int32_t err = EXIT_SUCCESS;

  populateTextureFrameRects();

  //Open the font with font size 30
  _fontSmall = TTF_OpenFont("../assets/orbitron-medium.otf", 40);
  if (nullptr == _fontSmall) {
    fprintf(stderr, "Failed to load font! SDL_ttf Error: %s\n",
    TTF_GetError());

    err = EXIT_FAILURE;
  }

  //Open the font with font size 80
  _fontBig = TTF_OpenFont("../assets/orbitron-medium.otf", 80);
  if (nullptr == _fontBig) {
    fprintf(stderr, "Failed to load font! SDL_ttf Error: %s\n",
    TTF_GetError());

    err = EXIT_FAILURE;
  }

  return err;
}

int32_t TextureContainer::loadSingleTexture(const char *filePath,
                                            const uint8_t textureId) {
  int32_t err = EXIT_SUCCESS;

  SDL_Surface *surface = nullptr;

  //Load image at specified path
  surface = IMG_Load(filePath);
  if (nullptr == surface) {
    fprintf(stderr, "Unable to load image %s! SDL_image Error: %s\n", filePath,
        IMG_GetError());

    err = EXIT_FAILURE;
  } else {
    //create hardware accelerated texture
    if (EXIT_SUCCESS != loadTextureFromSurface(surface, _textures[textureId])) {
      fprintf(stderr, "Unable to create texture with Id: %hhu\n", textureId);

      err = EXIT_FAILURE;
    }
  }

  return err;
}

int32_t TextureContainer::loadTextureFromSurface(SDL_Surface *&surface,
                                                 SDL_Texture *&outTexture) {
  int32_t err = EXIT_SUCCESS;

  //Create texture from surface pixels
  outTexture = SDL_CreateTextureFromSurface(_renderer, surface);

  if (nullptr == outTexture) {
    fprintf(stderr, "Unable to create texture! SDL Error: %s\n",
        SDL_GetError());

    err = EXIT_FAILURE;
  } else //success
  {
    SDL_FreeSurface(surface);
    surface = nullptr;
  }

  return err;
}

void TextureContainer::populateTextureFrameRects() {
  for (int32_t i = 0; i < Textures::COUNT; ++i) {
    //all textures have only 1 frame rectangle
    _textureFrameRects[i].resize(1);

    //and it tart from the beginning of the image
    _textureFrameRects[i][0].x = 0;
    _textureFrameRects[i][0].y = 0;
  }
}

