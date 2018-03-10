//
//  gui.cpp
//  platformer
//
//  Created by Vande Griek, Eric on 3/9/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "../include/gui.h"

/**
 private constructor
 */
Gui::Gui()
: groupStates(GuiGroupId::__END__, false) // this is probably a bad idea...
{
    for (int gid = 0; gid < groupStates.size(); ++gid) {
        menus[gid] = std::vector<Menu*>();
        elements[gid] = std::vector<GuiElement*>();
    }
}

/**
 Get the singleton instance
 */
Gui& Gui::instance() {
    static Gui *instance = new Gui();
    return *instance;
}

void Gui::render() {
    for (int gid = 0; gid < groupStates.size(); ++gid) {
        if (!groupStates[gid]) {
            continue;
        }
        for (auto &menu : menus[gid]) {
            menu->render();
        }
        for (auto &elem : elements[gid]) {
            elem->render();
        }
    }
}

void Gui::update() {
    for (int gid = 0; gid < groupStates.size(); ++gid) {
        if (!groupStates[gid]) {
            continue;
        }
        for (auto &elem : elements[gid]) {
            elem->update();
        }
    }
}

void Gui::add(GuiGroupId gid, Menu *menu) {
    menus[gid].push_back(menu);
}

void Gui::add(GuiGroupId gid, GuiElement *elem) {
    elements[gid].push_back(elem);
}

void Gui::toggleGroupDisplay(GuiGroupId gid) {
    groupStates[gid] = !groupStates[gid];
}

bool Gui::handleClick(int xpos, int ypos) {
    for (int gid = 0; gid < groupStates.size(); ++gid) {
        if (!groupStates[gid]) {
            continue;
        }
        for (auto &menu : menus[gid]) {
            bool handled = menu->handleClick(xpos, ypos);
            if (handled) {
                return true;
            }
        }
    }
    return false;
}
