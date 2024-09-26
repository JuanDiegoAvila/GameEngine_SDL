#pragma once
#include <string>
#include "Engine/Entity.h"
#include "Engine/Systems.h"
#include "Engine/Components.h"
#include "Components.h"
#include "Sprites.h"
#include <printf.h>

struct RockComponent {};

class RockMovementSystem : public UpdateSystem {
    void run(float dT) {
        auto view = scene->r.view<PositionComponent, VelocityComponent, RockComponent>();
        
        for (auto e : view) {
            auto& pos = view.get<PositionComponent>(e);
            auto vel = view.get<VelocityComponent>(e);

            pos.y += vel.y * dT;

            if (pos.y > HEIGHT) {
                scene->r.destroy(e);

                printf("Rock Destroyed\n");
            }
        }
    }
};