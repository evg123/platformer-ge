# A Multiplayer 2D Platformer using SDL2
Website: https://evg-game-engines.herokuapp.com/final.html

### Features:
* Network Multiplayer
  * Up to 4 players
  * Dedicated server mode
  * Client-side prediction
  * Mac / Linux cross-platform play
* Parallax scrolling background
* Multiple enemy types
* Animated sprites
* A level editor


### Controls:
* Click anywhere to start playing
* Use the arrow keys or A and D to move left and right
* Press Spacebar to jump
* Esc opens the pause menu
* The F key toggles an FPS display


### Level Editor:
* Launch from the level_editor dir
* Takes level filename as an argument
  * ./level_editor.py ../Assets/levels/level_1
* Click to cycle through tile types
* Use arrow keys to move the editor view
* Press 's' to save


### Sprite Preview Tool:
* Launch from the sprite_preview dir
* Takes arguments sprite_file, sprite_width, sprite_height, frame_start, frame_end
  * ./sprite_tool.py ../Assets/images/sprites/player.png 16 16 0 3
* The provided sprites are 16x16
* Use different starting and ending frame numbers to view different animations


### Build Instructions:
1. From the root directory run "python src/build_mac.py" or "python src/build_linux.py" depending on your platform
  * A Game directory will be created containing the binary and all assets needed
  * The linux build requires that you install several SDL packages:
    * libsdl2-dev, libsdl2-image-dev, libsdl2-ttf-dev, libsdl2-mixer-dev
1. Start the server with "./Game/Hopman"
1. Each player should run the game with "./Game/Hopman --server-addr <server ip address>"
  * The server listens for connections on all interfaces
  * Localhost (127.0.0.1) can be used for testing on the same machine the server is running on
1. Enjoy

Tested on OSX 10.12, Ubuntu 16.04 and 17.10

#### Attributions:
* Player sprite from https://opengameart.org/content/classic-hero
* Enemy sprites from https://opengameart.org/content/classic-hero-and-baddies-pack
* Background images from https://opengameart.org/content/mountain-at-dusk-background
* Dusk Tileset from https://opengameart.org/content/cavern-tile-set
* Fire and chest images from https://0x72.itch.io/16x16-dungeon-tileset
* Sound effects from https://opengameart.org/content/sfx-the-ultimate-2017-16-bit-mini-pack
* Background track by HitCtrl downloaded from https://opengameart.org/content/the-swarm
* Client-side prediction system adapted from https://gafferongames.com/post/networked_physics_2004/
* Network clock sync algorithm adapted from: http://www.mine-control.com/zack/timesync/timesync.html

