#include "audio_manager.h"

AudioManager::AudioManager() : 
    currentSource(AudioSource::SD_CARD),
    state(PlaybackState::STOPPED),
    volumeLevel(50),
    initialized(false) {
}

bool AudioManager::init() {
    // Configure I2S
    auto config = i2s.defaultConfig(TX_MODE);
    config.pin_bck = I2S_BCLK;
    config.pin_ws = I2S_LRC;
    config.pin_data = I2S_DOUT;
    config.sample_rate = SAMPLE_RATE;
    config.bits_per_sample = BITS_PER_SAMPLE;
    config.channels = CHANNELS;
    
    if (!i2s.begin(config)) {
        Serial.println("Failed to initialize I2S");
        return false;
    }
    
    // Configure volume control
    volume.begin(config);
    volume.setVolume(volumeLevel / 100.0f);
    
    // Configure decoder
    decoder.begin();
    
    Serial.println("Audio system initialized");
    initialized = true;
    return true;
}

bool AudioManager::play(const String& filename) {
    if (!initialized) return false;
    
    stop(); // Stop current playback
    
    // Open file
    audioFile = SD.open(filename);
    if (!audioFile) {
        Serial.println("Failed to open audio file: " + filename);
        return false;
    }
    
    // Configure audio pipeline
    source.begin(audioFile, filename.c_str());
    
    // Update track info
    currentTrack.filename = filename;
    currentTrack.title = extractTitle(filename);
    currentTrack.artist = "Unknown Artist";
    currentTrack.album = "Unknown Album";
    currentTrack.position = 0;
    
    state = PlaybackState::PLAYING;
    Serial.println("Playing: " + filename);
    return true;
}

void AudioManager::pause() {
    if (state == PlaybackState::PLAYING) {
        state = PlaybackState::PAUSED;
        Serial.println("Playback paused");
    }
}

void AudioManager::resume() {
    if (state == PlaybackState::PAUSED) {
        state = PlaybackState::PLAYING;
        Serial.println("Playback resumed");
    }
}

void AudioManager::stop() {
    if (state != PlaybackState::STOPPED) {
        state = PlaybackState::STOPPED;
        if (audioFile) {
            audioFile.close();
        }
        currentTrack = TrackInfo();
        Serial.println("Playback stopped");
    }
}

void AudioManager::setVolume(uint8_t vol) {
    volumeLevel = constrain(vol, 0, 100);
    volume.setVolume(volumeLevel / 100.0f);
}

void AudioManager::update() {
    if (!initialized || state != PlaybackState::PLAYING) return;
    
    // Process audio data
    if (audioFile && audioFile.available()) {
        // Read and process audio data
        // This is a simplified version - actual implementation would
        // involve reading chunks and feeding to the decoder
    } else if (state == PlaybackState::PLAYING) {
        // End of file reached
        stop();
    }
}

String AudioManager::extractTitle(const String& filename) {
    // Extract title from filename (remove path and extension)
    int lastSlash = filename.lastIndexOf('/');
    int lastDot = filename.lastIndexOf('.');
    
    String title = filename.substring(lastSlash + 1, lastDot);
    return title.length() > 0 ? title : "Unknown";
}

bool AudioManager::initBluetooth() {
    // Bluetooth A2DP initialization would go here
    Serial.println("Bluetooth audio not yet implemented");
    return false;
}

bool AudioManager::initUSBAudio() {
    // USB audio initialization would go here
    Serial.println("USB audio not yet implemented");
    return false;
}