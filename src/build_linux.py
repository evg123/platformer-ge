#!/usr/bin/env python3
# builds Hopman into ../Game

import os
import sys

COMPILER="clang++"

SOURCE="./src/*.cpp ./src/*/*.cpp"

ARGUMENTS="-D LINUX -std=c++14" 

# Which directories do we want to include.
INCLUDE_DIR="-I ./include/game -I ./include/services -I ./include/gui -I ./include/util -I/usr/include/SDL2 -D_REENTRANT"

# What libraries do we want to include
LIBRARIES="-L/usr/lib/x86_64-linux-gnu -lSDL2 -lSDL2_mixer -lSDL2_image -lSDL2_ttf"

# The name of our executable
EXECUTABLE="./Game/Hopman"

# Build a string of our compile commands that we run in the terminal
compileString=COMPILER+" "+ARGUMENTS+" -o "+EXECUTABLE+" "+" "+INCLUDE_DIR+" "+SOURCE+" "+LIBRARIES

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

