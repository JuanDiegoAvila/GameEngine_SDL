#pragma once

#include <string>
#include "Engine/Entity.h"
#include "Engine/Systems.h"
#include "Engine/Components.h"
#include "Components.h"

struct TilemapComponent {
    std::string filename;
    std::vector<std::vector<int>> map;
    int tileSize;
    int scale;
};

class TilemapSetupSystem : public SetupSystem {
public:
    void run() override {
        std::vector<std::vector<int>> map = { 
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        };

        std::string filename = "assets/Tilesets/tileset2.png";
        Entity* tilemap = scene->createEntity("TILEMAP");
        tilemap->addComponent<TilemapComponent>(filename, map, 8, 8);
        tilemap->addComponent<TextureComponent>(filename);

    }
};

class TilemapRenderSystem : public RenderSystem {
    void run(SDL_Renderer* renderer) override {
        auto view = scene->r.view<TilemapComponent, TextureComponent>();
        for (auto entity : view) {
            auto tilemap = view.get<TilemapComponent>(entity);
            auto tex = view.get<TextureComponent>(entity);

            Texture* texture = TextureManager::GetTexture(tex.filename);

            int tileSize = tilemap.tileSize * tilemap.scale;
            int tilemapHeight = tilemap.map.size();
            int tilemapWidth = tilemap.map[0].size();

            for (int y = 0; y < tilemapHeight; y++){
                for (int x = 0; x < tilemapWidth; x++){
                    int tileIndex = tilemap.map[y][x];
                    
                    if(tileIndex >= 0) {
                        int tileIndexX = tileIndex % (texture->getWidth() / tilemap.tileSize);
                        int tileIndexY = tileIndex / (texture->getHeight() / tilemap.tileSize);
                        // int tileIndexX = tileIndex % 4;

                        printf("tileIndexX: %d\n", tileIndexX);
                        printf("tileIndexY: %d\n", tileIndexY);
                        // int tileIndexY = tileIndex / 4;

                        SDL_Rect clip = { 
                            tileIndexX * tilemap.tileSize, 
                            tileIndexY * tilemap.tileSize, 
                            tilemap.tileSize, 
                            tilemap.tileSize 
                        };
                        texture->render(
                            scene->renderer, 
                            x * tileSize,
                            y * tileSize, 
                            tileSize, 
                            tileSize, 
                            &clip
                        );
                    }
                }
            }
        }
    }
};

class AutoTilingSetupSystem : public SetupSystem {
private :
    bool isTile(std::vector<std::vector<int>>& map, int x, int y)
    {
        return (x >= 0 && x < map[0].size() && y >= 0 && y < map.size() && map[y][x] == 1);
    }

public:
    void run() override 
    {
        auto view = scene->r.view<TilemapComponent>();
        for (auto e : view) {
            auto& tilemap = view.get<TilemapComponent>(e);
            int tilemapHeight = tilemap.map.size();
            int tilemapWidth = tilemap.map[0].size();

            std::vector<std::vector<int>> newMap = tilemap.map;

            for (int y = 0; y < tilemapHeight; y++) {
                for (int x = 0; x < tilemapWidth; x++) {
                    if (tilemap.map[y][x] == 1){
                        bool north = isTile(tilemap.map, x, y - 1);
                        bool south = isTile(tilemap.map, x, y + 1);
                        bool east = isTile(tilemap.map, x + 1, y);
                        bool west = isTile(tilemap.map, x - 1, y);

                        int mask = 0;
                        if (north){
                            mask |= 1;
                        }
                        if (west){
                            mask |= 2;
                        }
                        if (east){
                            mask |= 4;
                        }
                        if (south){
                            mask |= 8;
                        }

                        newMap[y][x] = mask;

                    } else {
                        newMap[y][x] = -1;
                    }
                }
            }

            tilemap.map = newMap;
        }
    }
};