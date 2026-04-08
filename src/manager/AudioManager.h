//
// Created by Curry on 2026/04/06.
//

#ifndef PROJECT_AUDIOMANAGER_H
#define PROJECT_AUDIOMANAGER_H


#include <string>
#include <unordered_map>

#include "SDL3_mixer/SDL_mixer.h"
class AudioManager {

    MIX_Mixer *mixer = nullptr;
    MIX_Track *musicTrack = nullptr;
    static MIX_Track* sfxTrack;
    static std::unordered_map<std::string,MIX_Audio*> audio;

public:
    AudioManager();

    void loadAudio(const std::string& name, const char* path) const;


    void playMusic(const std::string &name, int fadeInMS) const;
    void stopMusic(int fadeOutMS) const;


    static void playSFX(const std::string& name);
};
#endif //PROJECT_AUDIOMANAGER_H
