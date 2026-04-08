//
// Created by Curry on 2026/04/06.
//

#include "AudioManager.h"

#include <iostream>
#include <ostream>

MIX_Track* AudioManager::sfxTrack;
std::unordered_map<std::string, MIX_Audio*> AudioManager::audio;

AudioManager::AudioManager() {
    if (MIX_Init() == 0) {
        std::cerr << "Failed to initialize mixer" << std::endl;
    }

    mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);

    if (!mixer) {
        std::cerr << "Failed to create mixer device" << std::endl;
        return;
    }

    musicTrack = MIX_CreateTrack(mixer);
    sfxTrack = MIX_CreateTrack(mixer);

    MIX_SetTrackGain(musicTrack, 0.8f);

}

void AudioManager::loadAudio(const std::string &name, const char *path) const {

    if (audio.contains(path)) return;

    auto audioPtr = MIX_LoadAudio(mixer, path, true);
    if (!audioPtr) {
        std::cerr << "Failed to load audio: " << path << std::endl;
        return;
    }

    audio.emplace(name,audioPtr);

}

void AudioManager::stopMusic(int fadeOutMS = 2000) const {
    // SDL3_mixer requires audio frames for precision, so we convert the MS first
    Sint64 fadeFrames = MIX_TrackMSToFrames(musicTrack, fadeOutMS);
    MIX_StopTrack(musicTrack, fadeFrames);
}

void AudioManager::playMusic(const std::string &name, int fadeInMS = 2000) const {
    // 1. Stop the current track with a fast 500ms fade
    stopMusic(500);

    // 2. Assign the new audio to the track
    if (MIX_SetTrackAudio(musicTrack, audio[name]) == 0 ) {
        std::cerr << "Failed to set track audio: " << name << std::endl;
        return;
    }

    // 3. SDL3_mixer uses a properties object to handle all play settings
    SDL_PropertiesID props = SDL_CreateProperties();

    // Set our fade-in time in milliseconds
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER, fadeInMS);

    // Set to -1 for infinite looping
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, -1);

    // 4. Play the track using our properties
    if (!MIX_PlayTrack(musicTrack, props)) {
        std::cerr << "Failed to play music: " << name << std::endl;
    } else {
        std::cout << "Fading in music: " << name << std::endl;
    }

    // 5. Clean up the properties object to prevent memory leaks
    SDL_DestroyProperties(props);
}

void AudioManager::playSFX(const std::string &name) {
    if (MIX_SetTrackAudio(sfxTrack, audio[name]) == 0) {
        std::cerr << "Failed to play music: " << name << std::endl;
        return;
    }

    MIX_PlayTrack(sfxTrack, 0);
    std::cout << "Playing Sfx: " << name << std::endl;
}


