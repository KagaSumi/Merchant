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
    int direction = 2; //1,2,3,4 (N,S,W,E)
public:
    void update(const std::vector<std::unique_ptr<Entity> > &entities, float dt) {
        for (auto &e: entities) {
            if (e->hasComponent<Animation>() && e->hasComponent<Velocity>()) {
                auto &anim = e->getComponent<Animation>();
                auto &velocity = e->getComponent<Velocity>();


                //state system
                std::string newClip;

                if (velocity.direction.x > 0.0f) {
                    newClip = "walk_right";
                    direction = 4;
                } else if (velocity.direction.x < 0.0f) {
                    newClip = "walk_left";
                    direction = 3;
                } else if (velocity.direction.y > 0.0f) {
                    newClip = "walk_down";
                    direction = 2;
                } else if (velocity.direction.y < 0.0f) {
                    newClip = "walk_up";
                    direction = 1;
                } else {
                    switch (direction) {
                        case 1:
                            newClip = "idle_up";
                            break;
                        case 2:
                            newClip = "idle_down";
                            break;
                        case 3:
                            newClip = "idle_left";
                            break;
                        case 4:
                            newClip = "idle_right";
                            break;
                        default:
                            newClip = "idle_down";
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
