#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <lvgl.h>
#include "../config.h"
#include "../display/display_manager.h"
#include "../audio/audio_manager.h"
#include "../storage/sd_manager.h"
#include "../input/input_manager.h"

enum class Screen {
    HOME,
    NOW_PLAYING,
    LIBRARY,
    ARTISTS,
    ALBUMS,
    SETTINGS,
    EQUALIZER
};

class UIManager {
private:
    DisplayManager* display;
    AudioManager* audio;
    SDManager* storage;
    InputManager* input;
    
    Screen currentScreen;
    lv_obj_t* screenContainer;
    lv_obj_t* statusBar;
    lv_obj_t* mainContent;
    lv_obj_t* navBar;
    
    // Screen objects
    lv_obj_t* homeScreen;
    lv_obj_t* nowPlayingScreen;
    lv_obj_t* libraryScreen;
    lv_obj_t* settingsScreen;
    
    // UI elements
    lv_obj_t* batteryIcon;
    lv_obj_t* timeLabel;
    lv_obj_t* titleLabel;
    lv_obj_t* artistLabel;
    lv_obj_t* progressBar;
    lv_obj_t* volumeBar;
    
    // Styles
    lv_style_t styleBackground;
    lv_style_t styleText;
    lv_style_t styleButton;
    lv_style_t styleBar;
    
    void initStyles();
    void createStatusBar();
    void createNavBar();
    void createHomeScreen();
    void createNowPlayingScreen();
    void createLibraryScreen();
    void createSettingsScreen();
    
    void switchToScreen(Screen screen);
    void updateStatusBar();
    void updateNowPlaying();
    void handleInput();
    
    // Event callbacks
    static void buttonEventHandler(lv_event_t* e);
    
public:
    UIManager();
    void init(DisplayManager* disp, AudioManager* aud, SDManager* stor, InputManager* inp);
    void update();
    
    // Screen navigation
    void showHome();
    void showNowPlaying();
    void showLibrary();
    void showSettings();
    
    // UI updates
    void updateTrackInfo();
    void updateProgress();
    void updateVolume();
    void updateBattery(uint8_t percentage);
};

#endif // UI_MANAGER_H