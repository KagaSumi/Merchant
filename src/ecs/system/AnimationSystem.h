//
// Created by Curry on 2026/02/17.
//

#ifndef PROJECT_ANIMATIONSYSTEM_H
#define PROJECT_ANIMATIONSYSTEM_H
#include <memory>
#include <vector>

#include "Components.h"
#include "Entity.h"

/*3 Responsibilities
State System: Deciding Which Clip to use
Check if the animations has been switched
Playback System: advances the animations

*/

class AnimationSystem {
    //Default Spawn facing down
public:
    void update(const std::vector<std::unique_ptr<Entity> > &entities, float dt) {
        for (auto &e: entities) {
            if (e->hasComponent<Animation>() && e->hasComponent<Velocity>()) {
                auto &anim = e->getComponent<Animation>();
                auto &velocity = e->getComponent<Velocity>();


                //state system
                std::string newClip;

                // Get the absolute values to see which direction they are moving the most
                float absX = std::abs(velocity.direction.x);
                float absY = std::abs(velocity.direction.y);

                // Use a tiny threshold to ignore microscopic physics jitters
                float deadzone = 0.01f;

                if (absX > absY && absX > deadzone) {
                    // Horizontal movement is dominant
                    if (velocity.direction.x > 0.0f) {
                        newClip = "walk_right";
                        anim.direction = 4;
                    } else {
                        newClip = "walk_left";
                        anim.direction = 3;
                    }
                } else if (absY > absX && absY > deadzone) {
                    // Vertical movement is dominant
                    if (velocity.direction.y > 0.0f) {
                        newClip = "walk_down";
                        anim.direction = 2;
                    } else {
                        newClip = "walk_up";
                        anim.direction = 1;
                    }
                } else {
                    // Entity is practically stopped, revert to idle based on last known direction
                    switch (anim.direction) {
                        case 1: newClip = "idle_up"; break;
                        case 2: newClip = "idle_down"; break;
                        case 3: newClip = "idle_left"; break;
                        case 4: newClip = "idle_right"; break;
                        default: newClip = "idle_down";
                    }
                }
                //Check if the animations has switched
                //if the chosen clip is different from the current one, switch to new clip, reset time and frame index
                if (newClip != anim.currentClip) {
                    anim.currentClip = newClip; //Switch to new clip
                    anim.time = 0.0f; // Reset time to 0
                    anim.currentFrame = 0; // Reset frame index to 0
                }

                //playback system: Advances the animations
                float animFrameSpeed = anim.speed; //how long each 'animations frame' should last
                auto clip = anim.clips[anim.currentClip]; //retrieve frame data from current clip

                //advance the time
                anim.time += dt; //Every 'game loop frame' we are going to add the accumulated time

                //once enough time has passed
                if (anim.time >= animFrameSpeed) {
                    anim.time -= animFrameSpeed; //subtract animFrameSpeed (e.g. 0.1f) so that extra time isn't lost

                    std::size_t totalAnimationFrames = clip.frameIndicies.size();
                    //advance to the next animations frame (currentAnimationFrame +1)
                    //wrap around % so it loops when reaching the end of the clip
                    anim.currentFrame = (anim.currentFrame + 1) % totalAnimationFrames;
                }
            }
        }
    }
};

#endif //PROJECT_ANIMATIONSYSTEM_H
