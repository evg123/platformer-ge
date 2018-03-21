//
//  gui.cpp
//  platformer
//
//  Created by Vande Griek, Eric on 3/9/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "gui.h"

/**
 Constructor not used, things are set up in init()
 */
Gui::Gui() {}

/**
 Private destructor
 */
Gui::~Gui() {}

/**
 Get the singleton instance
 */
Gui& Gui::instance() {
    static Gui *instance = new Gui();
    return *instance;
}

void Gui::init() {
    // init group state vector using end value from group id enum
    // this is probably a bad idea...
    groupStates.assign(GuiGroupId::__END__, false);

    // create empty vectors for each group
    for (int gid = 0; gid < groupStates.size(); ++gid) {
        menus[gid] = std::vector<Menu*>();
        elements[gid] = std::vector<GuiElement*>();
    }
}

void Gui::shutdown() {
    clearGui();
}

void Gui::clearGui() {
    for (int gid = 0; gid < groupStates.size(); ++gid) {
        for (auto item : menus[gid]) {
            delete item;
        }
        for (auto item : elements[gid]) {
            delete item;
        }
    }
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

void Gui::setGroupDisplay(GuiGroupId gid, bool display) {
    groupStates[gid] = display;
}

void Gui::toggleGroupDisplay(GuiGroupId gid) {
    groupStates[gid] = !groupStates[gid];
}

bool Gui::handleClick(int xpos, int ypos, bool released) {
    for (int gid = 0; gid < groupStates.size(); ++gid) {
        if (!groupStates[gid]) {
            continue;
        }

        bool handled = false;
        for (auto &menu : menus[gid]) {
            menu->releaseAll();
            if (!handled) {
                handled = menu->handleClick(xpos, ypos, released);
            }
        }
    }
    return false;
}
