//
// Created by Curry on 2026/04/06.
//

#ifndef PROJECT_AUDIOEVENTQUEUE_H
#define PROJECT_AUDIOEVENTQUEUE_H
#include <memory>
#include <queue>
#include <string>

#include "manager/AudioManager.h"

class AudioEvent {
public:
    explicit AudioEvent(const std::string &name) : name(name) {}

    void execute() const {
        AudioManager::playSFX(name);
    }

private:
    std::string name;
};


class AudioEventQueue {
public:
    void push(std::unique_ptr<AudioEvent> event) {
        events.push(std::move(event));
    }

    void process() {
        while (!events.empty()) {
            events.front()->execute();
            events.pop();
        }
    }

private:
    std::queue<std::unique_ptr<AudioEvent> > events;
};

#endif //PROJECT_AUDIOEVENTQUEUE_H
