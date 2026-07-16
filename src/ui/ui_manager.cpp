#include "ui_manager.h"

UIManager::UIManager() : 
    display(nullptr),
    audio(nullptr),
    storage(nullptr),
    input(nullptr),
    currentScreen(Screen::HOME) {
}

void UIManager::init(DisplayManager* disp, AudioManager* aud, SDManager* stor, InputManager* inp) {
    display = disp;
    audio = aud;
    storage = stor;
    input = inp;
    
    // Initialize styles
    initStyles();
    
    // Create main container
    screenContainer = lv_obj_create(lv_scr_act());
    lv_obj_set_size(screenContainer, TFT_WIDTH, TFT_HEIGHT);
    lv_obj_add_style(screenContainer, &styleBackground, 0);
    lv_obj_clear_flag(screenContainer, LV_OBJ_FLAG_SCROLLABLE);
    
    // Create UI sections
    createStatusBar();
    createNavBar();
    
    // Create screens
    createHomeScreen();
    createNowPlayingScreen();
    createLibraryScreen();
    createSettingsScreen();
    
    // Show home screen initially
    switchToScreen(Screen::HOME);
    
    Serial.println("UI Manager initialized");
}

void UIManager::initStyles() {
    // Background style
    lv_style_init(&styleBackground);
    lv_style_set_bg_color(&styleBackground, lv_color_hex(COLOR_BLACK));
    lv_style_set_border_width(&styleBackground, 0);
    lv_style_set_pad_all(&styleBackground, 0);
    
    // Text style
    lv_style_init(&styleText);
    lv_style_set_text_color(&styleText, lv_color_hex(COLOR_WHITE));
    lv_style_set_text_font(&styleText, &lv_font_montserrat_14);
    
    // Button style
    lv_style_init(&styleButton);
    lv_style_set_bg_color(&styleButton, lv_color_hex(COLOR_BLUE));
    lv_style_set_text_color(&styleButton, lv_color_hex(COLOR_WHITE));
    lv_style_set_border_width(&styleButton, 1);
    lv_style_set_border_color(&styleButton, lv_color_hex(COLOR_WHITE));
    lv_style_set_radius(&styleButton, 5);
    
    // Progress bar style
    lv_style_init(&styleBar);
    lv_style_set_bg_color(&styleBar, lv_color_hex(COLOR_GRAY));
    lv_style_set_bg_grad_color(&styleBar, lv_color_hex(COLOR_BLUE));
    lv_style_set_bg_grad_dir(&styleBar, LV_GRAD_DIR_HOR);
}

void UIManager::createStatusBar() {
    statusBar = lv_obj_create(screenContainer);
    lv_obj_set_size(statusBar, TFT_WIDTH, STATUS_BAR_HEIGHT);
    lv_obj_set_pos(statusBar, 0, 0);
    lv_obj_add_style(statusBar, &styleBackground, 0);
    lv_obj_clear_flag(statusBar, LV_OBJ_FLAG_SCROLLABLE);
    
    // Time label
    timeLabel = lv_label_create(statusBar);
    lv_label_set_text(timeLabel, "12:34");
    lv_obj_add_style(timeLabel, &styleText, 0);
    lv_obj_set_pos(timeLabel, 5, 2);
    
    // Battery icon
    batteryIcon = lv_label_create(statusBar);
    lv_label_set_text(batteryIcon, "100%");
    lv_obj_add_style(batteryIcon, &styleText, 0);
    lv_obj_set_pos(batteryIcon, TFT_WIDTH - 40, 2);
}

void UIManager::createNavBar() {
    navBar = lv_obj_create(screenContainer);
    lv_obj_set_size(navBar, TFT_WIDTH, NAV_BAR_HEIGHT);
    lv_obj_set_pos(navBar, 0, TFT_HEIGHT - NAV_BAR_HEIGHT);
    lv_obj_add_style(navBar, &styleBackground, 0);
    lv_obj_clear_flag(navBar, LV_OBJ_FLAG_SCROLLABLE);
    
    // Navigation buttons
    lv_obj_t* homeBtn = lv_btn_create(navBar);
    lv_obj_set_size(homeBtn, 60, 18);
    lv_obj_set_pos(homeBtn, 5, 1);
    lv_obj_add_style(homeBtn, &styleButton, 0);
    lv_obj_t* homeLabel = lv_label_create(homeBtn);
    lv_label_set_text(homeLabel, "Home");
    lv_obj_center(homeLabel);
    
    lv_obj_t* playBtn = lv_btn_create(navBar);
    lv_obj_set_size(playBtn, 60, 18);
    lv_obj_set_pos(playBtn, 90, 1);
    lv_obj_add_style(playBtn, &styleButton, 0);
    lv_obj_t* playLabel = lv_label_create(playBtn);
    lv_label_set_text(playLabel, "Play");
    lv_obj_center(playLabel);
    
    lv_obj_t* libBtn = lv_btn_create(navBar);
    lv_obj_set_size(libBtn, 60, 18);
    lv_obj_set_pos(libBtn, 175, 1);
    lv_obj_add_style(libBtn, &styleButton, 0);
    lv_obj_t* libLabel = lv_label_create(libBtn);
    lv_label_set_text(libLabel, "Library");
    lv_obj_center(libLabel);
}

void UIManager::createHomeScreen() {
    homeScreen = lv_obj_create(screenContainer);
    lv_obj_set_size(homeScreen, TFT_WIDTH, MAIN_CONTENT_HEIGHT);
    lv_obj_set_pos(homeScreen, 0, STATUS_BAR_HEIGHT);
    lv_obj_add_style(homeScreen, &styleBackground, 0);
    
    // Welcome message
    lv_obj_t* welcomeLabel = lv_label_create(homeScreen);
    lv_label_set_text(welcomeLabel, "ESP32-S3 iPod\nMusic Player");
    lv_obj_add_style(welcomeLabel, &styleText, 0);
    lv_obj_center(welcomeLabel);
    
    // Quick stats
    lv_obj_t* statsLabel = lv_label_create(homeScreen);
    lv_label_set_text(statsLabel, "Songs: 0\nArtists: 0\nAlbums: 0");
    lv_obj_add_style(statsLabel, &styleText, 0);
    lv_obj_set_pos(statsLabel, 20, 150);
}

void UIManager::createNowPlayingScreen() {
    nowPlayingScreen = lv_obj_create(screenContainer);
    lv_obj_set_size(nowPlayingScreen, TFT_WIDTH, MAIN_CONTENT_HEIGHT);
    lv_obj_set_pos(nowPlayingScreen, 0, STATUS_BAR_HEIGHT);
    lv_obj_add_style(nowPlayingScreen, &styleBackground, 0);
    lv_obj_add_flag(nowPlayingScreen, LV_OBJ_FLAG_HIDDEN);
    
    // Album art placeholder
    lv_obj_t* albumArt = lv_obj_create(nowPlayingScreen);
    lv_obj_set_size(albumArt, 120, 120);
    lv_obj_set_pos(albumArt, (TFT_WIDTH - 120) / 2, 20);
    lv_obj_add_style(albumArt, &styleButton, 0);
    
    // Track title
    titleLabel = lv_label_create(nowPlayingScreen);
    lv_label_set_text(titleLabel, "No Track");
    lv_obj_add_style(titleLabel, &styleText, 0);
    lv_obj_set_pos(titleLabel, 20, 150);
    
    // Artist name
    artistLabel = lv_label_create(nowPlayingScreen);
    lv_label_set_text(artistLabel, "Unknown Artist");
    lv_obj_add_style(artistLabel, &styleText, 0);
    lv_obj_set_pos(artistLabel, 20, 170);
    
    // Progress bar
    progressBar = lv_bar_create(nowPlayingScreen);
    lv_obj_set_size(progressBar, TFT_WIDTH - 40, 10);
    lv_obj_set_pos(progressBar, 20, 200);
    lv_obj_add_style(progressBar, &styleBar, 0);
    lv_bar_set_value(progressBar, 0, LV_ANIM_OFF);
    
    // Volume bar
    volumeBar = lv_bar_create(nowPlayingScreen);
    lv_obj_set_size(volumeBar, TFT_WIDTH - 40, 8);
    lv_obj_set_pos(volumeBar, 20, 230);
    lv_obj_add_style(volumeBar, &styleBar, 0);
    lv_bar_set_value(volumeBar, 50, LV_ANIM_OFF);
}

void UIManager::createLibraryScreen() {
    libraryScreen = lv_obj_create(screenContainer);
    lv_obj_set_size(libraryScreen, TFT_WIDTH, MAIN_CONTENT_HEIGHT);
    lv_obj_set_pos(libraryScreen, 0, STATUS_BAR_HEIGHT);
    lv_obj_add_style(libraryScreen, &styleBackground, 0);
    lv_obj_add_flag(libraryScreen, LV_OBJ_FLAG_HIDDEN);
    
    // Library list (placeholder)
    lv_obj_t* listLabel = lv_label_create(libraryScreen);
    lv_label_set_text(listLabel, "Music Library\n\nScanning...");
    lv_obj_add_style(listLabel, &styleText, 0);
    lv_obj_set_pos(listLabel, 20, 50);
}

void UIManager::createSettingsScreen() {
    settingsScreen = lv_obj_create(screenContainer);
    lv_obj_set_size(settingsScreen, TFT_WIDTH, MAIN_CONTENT_HEIGHT);
    lv_obj_set_pos(settingsScreen, 0, STATUS_BAR_HEIGHT);
    lv_obj_add_style(settingsScreen, &styleBackground, 0);
    lv_obj_add_flag(settingsScreen, LV_OBJ_FLAG_HIDDEN);
    
    // Settings options
    lv_obj_t* settingsLabel = lv_label_create(settingsScreen);
    lv_label_set_text(settingsLabel, "Settings\n\nBrightness\nEqualizer\nBluetooth\nAbout");
    lv_obj_add_style(settingsLabel, &styleText, 0);
    lv_obj_set_pos(settingsLabel, 20, 30);
}

void UIManager::update() {
    // Handle LVGL tasks
    lv_timer_handler();
    
    // Handle input events
    handleInput();
    
    // Update UI elements
    updateStatusBar();
    if (currentScreen == Screen::NOW_PLAYING) {
        updateNowPlaying();
    }
}

void UIManager::handleInput() {
    if (!input) return;
    
    while (input->hasEvents()) {
        InputEvent event = input->getNextEvent();
        
        switch (event) {
            case InputEvent::ENCODER_CW:
                // Scroll down or increase volume
                break;
                
            case InputEvent::ENCODER_CCW:
                // Scroll up or decrease volume
                break;
                
            case InputEvent::ENCODER_CLICK:
                // Select or play/pause
                if (audio && currentScreen == Screen::NOW_PLAYING) {
                    if (audio->getState() == PlaybackState::PLAYING) {
                        audio->pause();
                    } else {
                        audio->resume();
                    }
                }
                break;
                
            case InputEvent::TOUCH_SWIPE_LEFT:
                // Next screen or next track
                break;
                
            case InputEvent::TOUCH_SWIPE_RIGHT:
                // Previous screen or previous track
                break;
                
            default:
                break;
        }
    }
}

void UIManager::switchToScreen(Screen screen) {
    // Hide all screens
    lv_obj_add_flag(homeScreen, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(nowPlayingScreen, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(libraryScreen, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(settingsScreen, LV_OBJ_FLAG_HIDDEN);
    
    // Show selected screen
    switch (screen) {
        case Screen::HOME:
            lv_obj_clear_flag(homeScreen, LV_OBJ_FLAG_HIDDEN);
            break;
        case Screen::NOW_PLAYING:
            lv_obj_clear_flag(nowPlayingScreen, LV_OBJ_FLAG_HIDDEN);
            break;
        case Screen::LIBRARY:
            lv_obj_clear_flag(libraryScreen, LV_OBJ_FLAG_HIDDEN);
            break;
        case Screen::SETTINGS:
            lv_obj_clear_flag(settingsScreen, LV_OBJ_FLAG_HIDDEN);
            break;
        default:
            break;
    }
    
    currentScreen = screen;
}

void UIManager::updateStatusBar() {
    // Update time (placeholder)
    static unsigned long lastTimeUpdate = 0;
    if (millis() - lastTimeUpdate > 1000) {
        // In real implementation, get time from RTC
        lv_label_set_text(timeLabel, "12:34");
        lastTimeUpdate = millis();
    }
}

void UIManager::updateNowPlaying() {
    if (!audio) return;
    
    const TrackInfo& track = audio->getCurrentTrack();
    
    // Update track info
    lv_label_set_text(titleLabel, track.title.c_str());
    lv_label_set_text(artistLabel, track.artist.c_str());
    
    // Update progress
    if (track.duration > 0) {
        int progress = (track.position * 100) / track.duration;
        lv_bar_set_value(progressBar, progress, LV_ANIM_OFF);
    }
    
    // Update volume
    lv_bar_set_value(volumeBar, audio->getVolume(), LV_ANIM_OFF);
}