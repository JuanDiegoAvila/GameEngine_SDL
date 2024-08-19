#pragma once

#include "Texture.h"
#include <map>
#include <string>

class TextureManager {
public:
    static Texture* LoadTexture(const std::string& filename, SDL_Renderer* renderer);
    static void UnloadTexture(const std::string& filename);
    static Texture* GetTexture(const std::string& filename);
    
private:
    static std::map<std::string, Texture*> textures;
};
