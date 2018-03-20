/**
 \mainpage Hopman Documentation Main Page
 
 # A 2D platformer using SDL2
 
 ### Features:
 -
 
 ### Controls:
 - Use the arrow keys or A and D to move left and right
 - Press Space to jump
 - Press Q to quit
 - Esc opens pause menu
 - The F key toggles an FPS display
 
*/

#include "hopman.h"

/**
 It all starts here
 */
int main() {
    Hopman hpm = Hopman();

    hpm.init();
    int ret = hpm.play();
    hpm.shutdown();

    return ret;
}
