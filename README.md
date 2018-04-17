# A Multiplayer 2D Platformer using SDL2
Website: https://evg-game-engines.herokuapp.com/platformer.html

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

******************
Original README:

# FinalProject

Note: There is only 1 part to the final project. This is it--the grand finale!

## Team

Team Name: EVG

- Eric VandeGriek


## Final Project - Description

The industry is getting positive buzz as Nolan and Shigeru have been talking about your studio. But dynamics change rapidly in the game industry, and your team has once again decided to part ways and form a new studio. Your team during this time has also developed a nice codebase of intellectual property, and has decided forming a 'game making' studio is the best option. Your team has decided it will start building an impressive piece of middleware (i.e. a tool or technology that is not a game) to showcase at the giant tradeshows (PAX East, GDC, E3, etc.) 

## Options

### Option 1 - Tech Demo 2D or 3D

Often a game mechanic or graphical technique is later developed into a game. For your final project you can build a high powered tech demo. This should be a short showcase of some feature demo that showcases your technology. The demo can be 2D or 3D.

Some examples of tech demos
- https://www.youtube.com/watch?v=ni-SmkCY-Sw (2D game demo showing off only 1 feature that is sufficiently complex)
- https://www.youtube.com/watch?v=4W8zxAV8RRY (2D game engine lighting effects)
- https://www.youtube.com/watch?v=LseTVMkG7V8 (An even better 2D game engine lighting effects, something like this is final project worthy if incorporated into platformer)
- https://www.youtube.com/watch?v=gLAYBdHxfjs (Shows a few of the features of a game tool)
- https://www.youtube.com/watch?v=jVlxmC2qBGU (Unity3D Tech demo showing off one simple but high powered toggable feature in an editor).


### Option 2 - Game Maker

For your final project, you may make a 'game maker'. That is, you will build a tool that someone with no C++ knowledge can use. Our pybind11 lab is a good starting point. You should document some 2D API for your games.

Some examples for inspiration
- Look on the left panel to see the API for the Love2D engine: https://love2d.org/wiki/Main_Page
- Look at Gamemakers API for inspiration of what commands are exposed. http://docs2.yoyogames.com/
- https://www.youtube.com/watch?v=b8RyQSvmrEU (Shows a neat console editor)
- https://www.youtube.com/watch?v=oU69bjOMTUc (Another platformer, shows off some cool extensions to our project, with transparency, particles, etc.)
- https://www.youtube.com/watch?v=o59PVYfKlzQ (A little RPG like game. Neat effects, and can benefit from some sort of scripting engine for battle sequences).
- https://www.youtube.com/watch?v=5ZuQIbMEjLw (Here is GoDot engine. I would look to GoDot and GameMaker as inspiration).

### Constraints on Options 1 and 2

The constraints are that you have to build the majority of your technology. 3rd party libraries for a physics engine are okay, and must be cited. 3rd party game or graphics engines may not be used (e.g. you cannot build a tech demo with Ogre3D or Unity3D).

## Minimum Viable Product

A Minimum Viable Product (MVP) is the smallest subset of features that you consider a project to be a success. Make a list below of the features you consider to be in your MVP. Then make a list of features that are 'stretch goals' that you think you can achieve.

#### Proposal
For my final project I propose to create a tech demo of networked multiplayer in a 2d platformer. I would use my platformer from assignment 2 as the starting point for this project.

Writing a multiplayer game is something that I've wanted to try my hand at, and I think I'll learn a lot about game engine design by implementing it myself.

I plan to spend some time reading up on modern techniques and best practices for game networking so that my design is relevant to what modern games are doing, even though my platformer is very simple.

I should be able to use this http://nshipster.com/network-link-conditioner/ to simulate high-latency connections when testing that my network code handles imperfect network conditions satisfactorily. 

MVP
* Simultaneous multiplayer in a 2D platformer over a local network
  * Support an arbitrary number of players
  * See and interact with other player characters
  * Display info on the state of the other players on the server
* A server process that can be connected to by each player's game process
  * Server can be running next to the game process on one player's machine
* Configured with config files and/or command line arguments

Strecth Goals
* Replace config file / command-line args with a GUI to specify the server to join
* A server browser that finds and displays available multiplayer servers that can be connected to
* Player name/sprite customization
* More advanced netcode features:
  * Client-side prediction
* Get it working over the internet


## Requirements (For either option)

### Documentation
Fire up 'Doxygen' <a href="http://www.stack.nl/~dimitri/doxygen/">(Doxygen webpage)</a> to document the source code and automatically generate .html pages. Your documentation should cover your classes and functions.

Additionally, you documentation should have instructions on how to obtain, build, and run your final project.

### Build (binary file)
You need to have a compiled binary of your game. You can assume a target audience of either a 64-bit Mac, Ubuntu Linux, or Windows 10 machine. There should additionally be instructions about how to compile your code from source.

### Post mortem
A post mortem in games is a look back at what could be improved. Write a brief (2-3 paragraphs) on what could be improved if you had an additional 8 weeks to work on this project. Where would you allocate time, what tools would you build, would you use any different tools, etc.

### Website
Your documentation, a link to your binary, and your post mortem should be put together on a 1-page .html page. Additionally, provide a 1 minute video trailer(but 1 minute I suggest) and at least 3 screenshots of your game. This website will be the first place I look to grab your project. (Additionally, I think it is important to build a portfolio of your game development works!)

### Project Hieararchy

There should be some logical hierarchy for your project. Look to previous assignments for inspiration. Your project should not differ greatly unless there is a reason justified in your documentation.

## Rubric

This project will be evaluated in the following way. 

- 25% On meeting the requirements above
- 25% If you achieved your "Minimum viable product" (from your lab proposal.)
- 25% On the polish of your final project (did it work as intended, were there bugs, did I have to struggle to get your tool running)
- 25% On the creativity and overall 'wow' factor of your project. 
  - Some of this requirement *could* be met with the following idea. But you are not limited to this rubric.
    - Was there a cool feature or surprise that got me excited about the project?
    - Does this project have potential to be extended in the future?
    - Were there creative additional tools that supported your work.
    - Was there something not covered in class that you were excited about, learned, and implemented anyway? (Document this to bring to my attention if so!)
