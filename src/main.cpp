/**
 \mainpage Hopman Documentation Main Page
 
 # A Mulitplayer 2D platformer using SDL2
 
 ### Features:
 - Up to 4 player Multiplayer
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
 Server mode uses HopmanServer
 Client mode uses HopmanClient
*/

#include <string>
#include <cstring>
#include <iostream>
#include "hopman.h"
#include "hopman_server.h"
#include "hopman_client.h"

#define HOST_OPT  "--server-addr"
#define HEADLESS_FLAG  "--headless"
#define HELP_FLAG_1 "--help"
#define HELP_FLAG_2 "-h"

/**
 Display how to call the program
 */
void printUsage() {
    std::cout << "Usage:" << std::endl;
    std::cout << "  Hopman [ " << HOST_OPT << " <server IP address>"
              << " [" << HEADLESS_FLAG << "] ]" << std::endl;
}

/**
 Check if a command line flag was provided
 */
bool getCmdLineFlag(char **start, char **end, std::string flag) {
    char** opt_start = std::find(start, end, flag);
    if (opt_start != end) {
        return true;
    }
    return false;
}

/**
 Check if a command line parameter was set
 Fill in value_destination if it was
 */
bool getCmdLineOption(char **start, char **end, std::string option, std::string &value_destination) {
    char** opt_start = std::find(start, end, option);
    if (opt_start == end) {
        // option not specified
        return false;
    }
    opt_start++;
    if (opt_start == end) {
        // option has no value
        value_destination = "";
        return true;
    }
    value_destination = *opt_start;
    return true;
}

/**
 It all starts here
 */
int main(int argc, char *argv[]) {
    // parse arguments
    bool help = getCmdLineFlag(argv, argv + argc, HELP_FLAG_1) ||
                getCmdLineFlag(argv, argv + argc, HELP_FLAG_2);
    if (help) {
        printUsage();
        return 0;
    }

    std::string host;
    bool server_mode = !getCmdLineOption(argv, argv + argc, HOST_OPT, host);
    bool headless = getCmdLineFlag(argv, argv + argc, HEADLESS_FLAG);

    if (!server_mode && host.empty()) {
        printUsage();
        return 1;
    }

    if (!server_mode && headless) {
        std::cout << "Headless mode is not supported when running as client" << std::endl;
        return 1;
    }

    // start the game
    Hopman *hpm;
    if (server_mode) {
        hpm = new HopmanServer(headless);
    } else {
        hpm = new HopmanClient(host);
    }

    hpm->init();
    int ret = hpm->play();
    hpm->shutdown();

    return ret;
}
