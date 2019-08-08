# A 2D Platformer using SDL2

![Screenshot](https://github.com/evg123/platformer-ge/blob/master/media/img1.png)

Website: https://evg-game-engines.herokuapp.com/platformer.html

### Special Feature:
* Parallax scrolling background
  * Hopman features a parallax background system that allows the programmer to add multiple background layers that move independently.
  * Each layer is assigned a distance that affects how much it moves as the screen scrolls.
  * Layers that are further away will scroll at a slower rate than foreground layers.
  * Layers repeat to the left and right independently from each other.


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
1. From the root directory run "python src/build_mac.py"
   A Game directory will be created containing the binary and all assets needed.
1. Run the game with "./Game/Hopman"
1. Enjoy

Only tested on OSX 10.12

#### Attributions:
* Player sprite from https://opengameart.org/content/classic-hero
* Enemy sprites from https://opengameart.org/content/classic-hero-and-baddies-pack
* Background images from https://opengameart.org/content/mountain-at-dusk-background
* Dusk Tileset from https://opengameart.org/content/cavern-tile-set
* Fire and chest images from https://0x72.itch.io/16x16-dungeon-tileset
* Sound effects from https://opengameart.org/content/sfx-the-ultimate-2017-16-bit-mini-pack
* Background track by HitCtrl downloaded from https://opengameart.org/content/the-swarm

