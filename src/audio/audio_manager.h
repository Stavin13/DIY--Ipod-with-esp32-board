#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <Arduino.h>
#include "AudioTools.h"
#include "AudioCodecs/CodecMP3Helix.h"
#include "../config.h"

enum class AudioSource {
    SD_CARD,
    BLUETOOTH,
    USB_AUDIO
};

enum class PlaybackState {
    STOPPED,
    PLAYING,
    PAUSED
};

struct TrackInfo {
    String title;
    String artist;
    String album;
    String filename;
    uint32_t duration;
    uint32_t position;
};

class AudioManager {
private:
    I2SStream i2s;
    MP3DecoderHelix decoder;
    AudioSourceSDFAT source;
    VolumeStream volume;
    
    AudioSource currentSource;
    PlaybackState state;
    TrackInfo currentTrack;
    uint8_t volumeLevel;
    
    File audioFile;
    bool initialized;
    
public:
    AudioManager();
    bool init();
    
    // Playback control
    bool play(const String& filename);
    void pause();
    void resume();
    void stop();
    void next();
    void previous();
    
    // Volume control
    void setVolume(uint8_t volume);
    uint8_t getVolume() const { return volumeLevel; }
    
    // Position control
    void seek(uint32_t position);
    uint32_t getPosition() const;
    uint32_t getDuration() const;
    
    // State queries
    PlaybackState getState() const { return state; }
    const TrackInfo& getCurrentTrack() const { return currentTrack; }
    
    // Audio source switching
    void setAudioSource(AudioSource source);
    AudioSource getAudioSource() const { return currentSource; }
    
    // Update loop
    void update();
    
    // Bluetooth audio
    bool initBluetooth();
    void connectBluetooth(const String& deviceName);
    
    // USB audio
    bool initUSBAudio();
};

#endif // AUDIO_MANAGER_H