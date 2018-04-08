//
//  Created by Vande Griek, Eric on 2/22/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "hopman.h"

/**
 Set up the game
 */
void Hopman::init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        throw std::runtime_error("Failed to initialize SDL");
    }

    // init services
    ResourceManager::instance().init();
    Graphics::instance().init(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    Audio::instance().init();
    Input::instance().init();
    Gui::instance().init();

    objects = {};
    fps_display = 0;
    paused = false;
    level = STARTING_LEVEL;
    game_state = GameState::NONE;
    setGameState(GameState::LOADING);
    fps_limit = DEFAULT_FPS_LIMIT;
    score = 0;
    lives = DEFAULT_EXTRA_LIVES;
}

/**
 Tear it down
 */
void Hopman::shutdown() {
    cleanupLevel();

    // shutdown services
    Gui::instance().shutdown();
    Input::instance().shutdown();
    Audio::instance().shutdown();
    Graphics::instance().shutdown();
    ResourceManager::instance().shutdown();
    SDL_Quit();
}

/**
 Main gameplay loop
 */
int Hopman::play() {
    registerInputCallbacks();
    createUI();

    FrameTimer timer = FrameTimer(fps_limit);

    while (true) {
        // wait until it is time to render the next frame
        timer.delayUntilNextFrame();

        fps_display = timer.getFps();

        // signal a new frame to the fps timer and get the delta since the last frame
        int delta = timer.newFrame();

        // tell the input singleton to poll for events
        Input::instance().handleEvents();
        if (game_state == GameState::EXITING) {
            break;
        }

        // update game objects
        if (game_state == GameState::PLAYING && !paused) {
            update(delta);

            // focus the screen on the player
            Graphics::instance().focusScreenOffsets(getPlayer()->getRect().getCollider());
            background.updateLayerOffsets(getPlayer()->getRect().xPos(),
                                          getPlayer()->getRect().yPos());
        }

        // update the GUI
        Gui::instance().update();

        // draw the new frame
        render();

        handleGameState();

        // send/receive updates on the network
        networkUpdate();
    }
    
    return 0;
}

/**
 toggle the fps display UI
 */
void Hopman::toggleFps() {
    Gui::instance().toggleGroupDisplay(GuiGroupId::FPS_DISPLAY);
}

/**
 Set up the UI for the game
 */
void Hopman::createUI() {
    createStatusBar();
    createPauseMenu();
    createGameMessage();
}

/**
 Set up the status bar displayed at the top of the screen
 */
void Hopman::createStatusBar() {
    int screen_w = Graphics::instance().getWindowWidth();

    // bar background
    GuiElement *elem = new GuiElement({0, STATUS_BAR_Y, screen_w, STATUS_BAR_THICKNESS},
                                      ResourceManager::instance().getImageTexture(STATUS_BAR_IMG));
    Gui::instance().add(GuiGroupId::STATUS_BAR, elem);

    // score display
    int elem_pos = 10;
    elem = new TextGuiElement<std::string>({elem_pos, STATUS_BAR_Y + 5, 0, 0},
                                           SCORE_STR, STATUS_BAR_TEXT_SIZE);
    Gui::instance().add(GuiGroupId::STATUS_BAR, elem);
    elem = new TextGuiElement<int>({elem_pos + elem->getWidth(), STATUS_BAR_Y + 5, 0, 0},
                                           score, STATUS_BAR_TEXT_SIZE, true);
    Gui::instance().add(GuiGroupId::STATUS_BAR, elem);
    
    // level display
    elem_pos = 250;
    elem = new TextGuiElement<std::string>({elem_pos, STATUS_BAR_Y + 5, 0, 0},
                                           LEVEL_STR, STATUS_BAR_TEXT_SIZE);
    Gui::instance().add(GuiGroupId::STATUS_BAR, elem);
    elem = new TextGuiElement<int>({elem_pos + elem->getWidth(), STATUS_BAR_Y + 5, 0, 0},
                                   level, STATUS_BAR_TEXT_SIZE, true);
    Gui::instance().add(GuiGroupId::STATUS_BAR, elem);
    
    // lives display
    elem_pos = screen_w - 150;
    elem = new TextGuiElement<std::string>({elem_pos, STATUS_BAR_Y + 5, 0, 0},
                                           LIVES_STR, STATUS_BAR_TEXT_SIZE);
    Gui::instance().add(GuiGroupId::STATUS_BAR, elem);
    elem = new TextGuiElement<int>({elem_pos + elem->getWidth(), STATUS_BAR_Y + 5, 0, 0},
                                   lives, STATUS_BAR_TEXT_SIZE, true);
    Gui::instance().add(GuiGroupId::STATUS_BAR, elem);

    // fps display
    // in a different group so it can be hidden independently
    elem_pos = screen_w - 300;
    elem = new TextGuiElement<std::string>({elem_pos, STATUS_BAR_Y + 5, 0, 0},
                                           FPS_STR, STATUS_BAR_TEXT_SIZE);
    Gui::instance().add(GuiGroupId::FPS_DISPLAY, elem);
    elem = new TextGuiElement<int>({elem_pos + elem->getWidth(), STATUS_BAR_Y + 5, 0, 0},
                                   fps_display, STATUS_BAR_TEXT_SIZE, true);
    Gui::instance().add(GuiGroupId::FPS_DISPLAY, elem);

    // show the status bar
    Gui::instance().setGroupDisplay(GuiGroupId::STATUS_BAR, true);
}

void Hopman::createPauseMenu() {
    int menu_x = (Graphics::instance().getWindowWidth() / 2) - (PAUSE_MENU_WIDTH / 2);
    int menu_y = (Graphics::instance().getWindowHeight() / 2) - (PAUSE_MENU_HEIGHT / 2);
    Menu *menu = new Menu({menu_x, menu_y, PAUSE_MENU_WIDTH, PAUSE_MENU_HEIGHT},
                          ResourceManager::instance().getImageTexture(MAIN_MENU_BG_IMG),
                          PAUSE_MENU_TOP_PAD, PAUSE_MENU_BOT_PAD);
    
    menu->addItem("Pause Menu", 50, ResourceManager::instance().getImageTexture(MENU_TITLE_IMG),
                  NULL, false);
    //TODO replace
    //menu->addItem("Restart", 42, ResourceManager::instance().getImageTexture(BUTTON_IMG),
    //              std::bind(&Hopman::restartGame, this), true);
    menu->addItem("Quit", 42,
                  ResourceManager::instance().getImageTexture(BUTTON_IMG),
                  std::bind(&Hopman::exitGame, this), true);
    
    // add the completed menu to the gui
    Gui::instance().add(GuiGroupId::PAUSE, menu);
}

/**
 Set up the message occasionally displayed across the center of the screen
 */
void Hopman::createGameMessage() {
    // render a message of max length and get its size in order to center the message
    int msg_w, msg_h;
    std::string max_msg_str(GAME_MESSAGE_MAX_LEN, ' ');
    SDL_Texture *max_msg_texture = ResourceManager::instance().getTextTexture(max_msg_str, GAME_MSG_TEXT_SIZE);
    SDL_QueryTexture(max_msg_texture, NULL, NULL, &msg_w, &msg_h);

    // create a gui element for the message
    int item_x = (Graphics::instance().getWindowWidth() / 2) - (msg_w / 2);
    int item_y = (Graphics::instance().getWindowHeight() / 2) - (msg_h / 2);
    GuiElement *elem = new TextGuiElement<std::string>({item_x, item_y, msg_w, msg_h},
                                                       game_message, GAME_MSG_TEXT_SIZE, true);
    Gui::instance().add(GuiGroupId::GAME_MESSAGE, elem);
    game_message.reserve(GAME_MESSAGE_MAX_LEN);
}

/**
 Set the message displayed at the center of the screen
 */
void Hopman::setGameMessage(std::string new_msg) {
    if (new_msg.length() > GAME_MESSAGE_MAX_LEN) {
        throw std::runtime_error("Game message is too long: " + new_msg);
    }

    // get padding at front and end of new msg
    // round down to long
    long padding = (GAME_MESSAGE_MAX_LEN - new_msg.length()) / 2;
    std::string pad_str(padding, ' ');
    game_message.assign(pad_str + new_msg);
    Gui::instance().setGroupDisplay(GuiGroupId::GAME_MESSAGE, true);
}

/**
 Update all the game objects
 */
void Hopman::update(int delta) {
    for (auto &obj_record : objects) {
        Drawable *obj = obj_record.second;
        obj->update(delta, objects);
        // check if obj has fallen off the map
        if (obj->getRect().top() > lower_bound) {
            obj->destroy();
        }
    }

    // clean up objects that need to be removed from the game
    removeDestroyed();
}

/**
 Remove any objects that have been destroyed
 */
void Hopman::removeDestroyed() {
    handleDeath();

    // remove destroyed objects
    for (auto iter = objects.begin(); iter != objects.end();) {
        if (iter->second->needsRemoval() && !isPlayer(iter->second)) {
           this->score += iter->second->getScoreOnDestruction();
            iter = objects.erase(iter);
        } else {
            ++iter;
        }
    }
}

/**
 try to respawn after player death
 */
void Hopman::tryRespawn() {
    if (lives > 0) {
        // restart the level
        --lives;
        setGameState(GameState::RESPAWN);
    } else {
        setGameState(GameState::LOSS);
    }
}

void Hopman::pause() {
    paused = !paused;
    Gui::instance().toggleGroupDisplay(GuiGroupId::PAUSE);
}

/**
 Check if the user is ready to move on from the end of a level / loss
 */
void Hopman::advanceScreen() {
    if (paused) {
        // don't advance if we are paused
        return;
    }
    
    if (game_state == GameState::LOSS) {
        setGameState(GameState::LOADING);
    } else if (game_state == GameState::LEVEL_WON) {
        // move to next level
        setGameState(GameState::LOADING);
    } else if (game_state == GameState::RESPAWN) {
        // try the level again
        setGameState(GameState::LEVEL_START);
    } else if (game_state == GameState::LEVEL_START) {
        // start the level
        setGameState(GameState::PLAYING);
    }
}

/**
 Draw everything to the screen
 */
void Hopman::render() {
    background.render();
    
    for (auto &obj_rec : objects) {
        Drawable *obj = obj_rec.second;
        obj->render();
    }
    
    renderGui();
    
    Graphics::instance().swapFrame();
}

/**
 Draw the UI on top of everthing
 */
void Hopman::renderGui() {
    Gui::instance().render();
}

/**
 create a tile object at the given tile coordinates
 */
Drawable* Hopman::addTile(int tile_type, int tx, int ty, int id) {
    if (tile_type == TileNum::EMPTY) {
        return NULL;
    }

    // calculate position based on tile index
    int xpos = tx * TILE_SIDE;
    int ypos = ty * TILE_SIDE;

    Drawable *obj;
    if (tile_type == TileNum::PLAYER) {
        Being *player = new Being();
        player->init(BeingType::player());
        PlayerState player_state(player);
        players.push_back(player_state);
        obj = player;

    } else if (tile_type == TileNum::RED_ENEMY) {
        Being *enemy = new Being();
        enemy->init(BeingType::redEnemy());
        obj = enemy;

    } else if (tile_type == TileNum::BLUE_ENEMY) {
        Being *enemy = new Being();
        enemy->init(BeingType::blueEnemy());
        obj = enemy;

    } else {
        Tile *tile = new Tile(tile_type);
        if (tile_type == TileNum::GOAL) {
            tile->setCollisionCallback(std::bind(&Hopman::hitGoal, this, std::placeholders::_1));
        }
        obj = tile;
    }

    if (id > 0) {
        // id was specified, override the auto-assigned id
        obj->setId(id);
    }

    obj->setPosition(xpos, ypos);
    objects[obj->getId()] = obj;
    return obj;
}

Being* Hopman::getPlayer() {
    return players[0].player;
}

/**
 Called when something hits a goal tile.
 If its the player then they beat the level
 */
void Hopman::hitGoal(Drawable& other) {
    if (isPlayer(&other)) {
        setGameState(GameState::LEVEL_WON);
    }
}

/**
 Set up a parallax background
 */
void Hopman::createBackground() {
    int sw = Graphics::instance().getWindowWidth();
    int sh = Graphics::instance().getWindowHeight();
    background.init(getPlayer()->getRect().xPos(),
                    getPlayer()->getRect().yPos(),
                    lower_bound - 200);
    background.setColor(125, 90, 125);
    
    // add layers at different distances
    background.addLayer("background/dusk/layer_0.png", sw, sh, 60);
    background.addLayer("background/dusk/layer_1.png", sw, sh, 30);
    background.addLayer("background/dusk/layer_2.png", sw, sh, 20);
    background.addLayer("background/dusk/layer_3.png", sw, sh, 16);
    background.addLayer("background/dusk/layer_4.png", sw, sh, 6);
    background.addLayer("background/dusk/layer_5.png", sw, sh, 2);
}

/**
 Remove objects at the end of a level or loss
 */
void Hopman::cleanupLevel() {
    // deallocate everything in the drawable list
    for (auto &obj_record : objects) {
        Drawable *obj = obj_record.second;
        if (!isPlayer(obj)) {
            delete obj;
        }
    }
    objects.clear();

    Gui::instance().setGroupDisplay(GuiGroupId::GAME_MESSAGE, false);

    background.shutdown();
}

/**
 Return true if this object is one of the player characters
 */
bool Hopman::isPlayer(Drawable *obj) {
    for (auto &player_state : players) {
        if (player_state.player == obj) {
            return true;
        }
    }
    return false;
}

void Hopman::setGameState(GameState state) {
    if (state == game_state) {
        // no change
        return;
    }

    switch (state) {
        case GameState::LOADING:
            setGameMessage("Loading...");
            break;
        case GameState::RESPAWN:
            setGameMessage("You Died!");
            break;
        case GameState::LOSS:
            setGameMessage("GAME OVER :(");
            Audio::instance().playSound("game_over.wav");
            break;
        case GameState::PLAYING:
            Gui::instance().setGroupDisplay(GuiGroupId::GAME_MESSAGE, false);
            break;
        case GameState::LEVEL_WON:
            setGameMessage("YOU WIN!");
            Audio::instance().playSound("you_win.wav");
            break;
        case GameState::LEVEL_START:
            setGameMessage("Level " + std::to_string(level));
            break;
        default:
            break;
    }
    game_state = state;
}

// PlayerState Definitions

PlayerState::PlayerState(Being *player)
: player(player), assigned(false), state(GameState::LOADING) {}

