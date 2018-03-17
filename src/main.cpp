/**
 \mainpage Hopman Documentation Main Page
 
 # A 2D platformer using SDL2
 
 ### Features:
 -
 
 ### Controls:
 - Use the arrow keys or A and D to move left and right
 - Press Space to jump
 - Press Q to quit
 - The F key toggles an FPS display
 
*/

#include "hopman.h"

/**
 It all starts here
 */
int main() {
    
    Hopman hpm = Hopman();
    int ret = hpm.play();
    
    return ret;
}
