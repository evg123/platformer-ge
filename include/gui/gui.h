//
//  gui.h
//  platformer
//
//  Created by Vande Griek, Eric on 3/9/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef gui_h
#define gui_h

#include <map>
#include <vector>
#include "SDL2.framework/Headers/SDL.h"
#include "menu.h"
#include "gui_element.h"

/**
 Identifiers used to distinguish between collections of UI elements
 Not all of these are implemented
 */
enum GuiGroupId {
    STATUS_BAR,
    GAME_MESSAGE,
    MAIN_MENU,
    PAUSE,
    OPTIONS,
    __END__ // do not use, must be last
};

class Gui {
private:
    Gui();
    ~Gui();
    std::vector<bool> groupStates;
    std::map<int, std::vector<Menu*>> menus;
    std::map<int, std::vector<GuiElement*>> elements;
public:
    static Gui& instance();
    void init();
    void shutdown();
    void clearGui();
    void render();
    void update();
    void add(GuiGroupId gid, Menu *menu);
    void add(GuiGroupId gid, GuiElement *elem);
    void setGroupDisplay(GuiGroupId gid, bool display);
    void toggleGroupDisplay(GuiGroupId gid);
    bool handleClick(int xpos, int ypos, bool released);
};

#endif /* gui_h */
