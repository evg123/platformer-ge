//
//  gui.h
//  platformer
//
//  Created by Vande Griek, Eric on 3/9/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef gui_h
#define gui_h

#ifdef __linux__
#include "../lib/linux/SDL2.framework/Headers/SDL.h"
#else // Mac
#include "../lib/osx/SDL2.framework/Headers/SDL.h"
#endif

#include <map>
#include <vector>
#include "menu.h"
#include "gui_element.h"

/**
 Identifiers used to distinguish between collections of UI elements
 Not all of these are implemented
 */
enum GuiGroupId {
    STATUS_BAR,
    MAIN_MENU,
    PAUSE,
    OPTIONS,
    GAME_MESSAGE,
    __END__ // do not use, must be last
};

class Gui {
private:
    Gui();
    std::vector<bool> groupStates;
    std::map<int, std::vector<Menu*>> menus;
    std::map<int, std::vector<GuiElement*>> elements;
public:
    static Gui& instance();
    void render();
    void update();
    void add(GuiGroupId gid, Menu *menu);
    void add(GuiGroupId gid, GuiElement *elem);
    void setGroupDisplay(GuiGroupId gid, bool display);
    void toggleGroupDisplay(GuiGroupId gid);
    bool handleClick(int xpos, int ypos);
};

#endif /* gui_h */
