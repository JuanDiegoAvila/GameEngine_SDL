#include "Texture.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>

Texture::Texture()  
    : texture(nullptr), width(0), height(0) {
}
Texture::~Texture() {
    free();
}

void Texture::load(const std::string& path, SDL_Renderer* renderer){
    free();
    SDL_Surface* surface = IMG_Load(path.c_str());

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
    }

    width = surface->w;
    height = surface->h;
}

void Texture::free(){
    if(texture != nullptr){
        SDL_DestroyTexture(texture);
        width = 0;
        height = 0;
    }
}

void Texture::render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip){
    SDL_Rect srcrect = SDL_Rect{ x, y, width, height };
    
    if(clip != nullptr){
        srcrect.w = clip->w;
        srcrect.h = clip->h;
    }

    SDL_RenderCopy(renderer, texture, &srcrect, clip);
}