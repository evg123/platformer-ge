/**
 \mainpage Hopman Documentation Main Page
 
 # A 2D platformer using SDL2
 
 ### Features:
 - Parallax scrolling background
 - Multiple enemy types
 - Animated sprites
 - A level editor
 - A menu system
 
 ### Controls:
 - Use the arrow keys or A and D to move left and right
 - Press Space to jump
 - Press Q to quit
 - Esc opens pause menu
 - The F key toggles an FPS display
 
 The main game class is Hopman
*/

#include <string>
#include "hopman.h"
#include "hopman_server.h"
#include "hopman_client.h"

#define SERVER_MODE_FLAG  "--server-mode"
#define HEADLESS_FLAG  "--headless"
#define HOST_OPT  "--server-host"

bool getCmdLineFlag(char **start, char **end, std::string flag) {
    char** opt_start = std::find(start, end, flag);
    if (opt_start != end) {
        return true;
    }
    return false;
}

std::string getCmdLineOption(char **start, char **end, std::string option) {
    char** opt_start = std::find(start, end, option);
    if (opt_start == end) {
        // option not specified
        return "";
    }
    opt_start++;
    if (opt_start == end) {
        // option has no value
        //TODO print usage
        return "";
    }
    return *opt_start;
}

/**
 It all starts here
 */
int main(int argc, char *argv[]) {
    // parse arguments
    bool server_mode = getCmdLineFlag(argv, argv + argc, SERVER_MODE_FLAG);
    bool headless = getCmdLineFlag(argv, argv + argc, HEADLESS_FLAG);
    std::string host = getCmdLineOption(argv, argv + argc, HOST_OPT);

    // start the game
    Hopman *hpm;
    if (server_mode) {
        hpm = new HopmanServer();
    } else {
        hpm = new HopmanClient(host);
    }

    hpm->init(headless);
    int ret = hpm->play();
    hpm->shutdown();

    return ret;
}
