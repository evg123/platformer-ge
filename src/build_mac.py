#!/usr/bin/env python3
# builds Hopman into ../Game

import os
import sys

COMPILER="clang++"

SOURCE="./src/*.cpp ./src/*/*.cpp"

ARGUMENTS="-D MAC -std=c++14" 

# Which directories do we want to include.
SDL_INCLUDE="-I ./lib/osx/SDL2.framework/Headers -I ./lib/osx/SDL2_image.framework/Headers -I ./lib/osx/SDL2_mixer.framework/Headers -I ./lib/osx/SDL2_ttf.framework/Headers"
GAME_INCLUDE="-I ./include/game -I ./include/services -I ./include/gui -I ./include/util -I ./lib/SDL2.framework/Headers"

# What libraries do we want to include
LIBRARIES="-F ./lib/osx/ -framework SDL2 -framework SDL2_image -framework SDL2_ttf -framework SDL2_mixer"

# The name of our executable
EXECUTABLE="./Game/Hopman"

# Build a string of our compile commands that we run in the terminal
compileString=" ".join([COMPILER, ARGUMENTS, "-o " + EXECUTABLE, SDL_INCLUDE, GAME_INCLUDE, SOURCE, LIBRARIES])

# make transient directories
os.system('mkdir -p ./Game')
os.system('cp -r Assets ./Game/')

# Print out the compile string
print("Building...")

# Run our command
os.system(compileString)

if (len(sys.argv) > 1 and sys.argv[1] == "release"):
    # build a tarball
    print("Creating tarball")
    os.system("tar -zcf hopman.tar.gz Game")

print("Done")

