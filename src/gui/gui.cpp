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

/**
 Set up the GUI.
 It will be empty initially, until GuiElements and Menus are added to it
 */
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

/**
 Clean up things allocated in the GUI
 */
void Gui::shutdown() {
    clearGui();
}

/**
 Delete all the GuiElements and Menus
 */
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

/**
 Draw the GUI based on what is currently visible
 */
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

/**
 Update the dynamic parts of the GUI
 */
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

/**
 Add a new Menu to the GUI
 */
void Gui::add(GuiGroupId gid, Menu *menu) {
    menus[gid].push_back(menu);
}

/**
 Add a new GuiElement
 */
void Gui::add(GuiGroupId gid, GuiElement *elem) {
    elements[gid].push_back(elem);
}

/**
 Set the given groupid to be visible or not
 */
void Gui::setGroupDisplay(GuiGroupId gid, bool display) {
    groupStates[gid] = display;
}

/**
 Toggle the visibility of the given group
 */
void Gui::toggleGroupDisplay(GuiGroupId gid) {
    groupStates[gid] = !groupStates[gid];
}

/**
 Take a click and pass it to all menus and elements
 */
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
