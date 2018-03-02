#!/usr/bin/env python3

import sys
import os
import pygame
import math
import argparse

WINDOW_WIDTH = 1280
WINDOW_HEIGHT = 720

TILE_SIDE_PX = 32
TILE_TYPE_START = 1
TILE_TYPE_COUNT = 4
EMPTY_TILE_NUM = 0
TILE_IMAGE_PATTERN = '../Assets/images/tiles/tile_{0}.png'
DIM_LINE_FORMAT = '{0} {1}'

WINDOW_WIDTH_TILES = math.ceil(WINDOW_WIDTH / TILE_SIDE_PX)
WINDOW_HEIGHT_TILES = math.ceil(WINDOW_HEIGHT / TILE_SIDE_PX)

LMB = 1
RMB = 3

class Editor(object):
    
    def __init__(self):
        self.screen = None
        self.running = True
        self.tiles = []
        self.xn_off = 0
        self.yn_off = 0
        self.image_bank = [0]
        self.level_filename = None
    
    def setup(self, level_file):
        self.screen = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
        self.load_image_bank()
        self.level_filename = level_file
        self.load_level()
    
    def load_image_bank(self):
        for num in range(TILE_TYPE_START, TILE_TYPE_COUNT):
            filename = TILE_IMAGE_PATTERN.format(num)
            img = pygame.image.load(filename)
            self.image_bank.append(img)
        print(self.image_bank)
    
    def load_level(self):
        # check if file exists
        if not os.path.isfile(self.level_filename):
            # new level file, start with a blank level
            self.blank_level()
            return
        
        # load an existing level from a file
        with open(self.level_filename, 'r') as lvl_file:
            # first line is dimensions, but we don't need it
            # it's helpful for the c++ parser
            _ = lvl_file.readline()
            
            level_lines = lvl_file.readlines()
            for line in level_lines:
                tile_line = line.split()
                
                while len(self.tiles) < len(tile_line):
                    # widen tiles 2d array to match loaded level
                    self.tiles.append([])
                
                for xn in range(len(tile_line)):
                    self.tiles[xn].append(int(tile_line[xn]))
    
    def save_level(self):
        with open(self.level_filename, 'w') as lvl_file:
            # write dimensions on first line
            lvl_file.write(DIM_LINE_FORMAT.format(len(self.tiles), len(self.tiles[0])))
            
            for yn in range(len(self.tiles[0])):
                lvl_file.write(' '.join(str(col[yn]) for col in self.tiles) + '\n')
    
    def blank_level(self):
        start_xn = WINDOW_WIDTH_TILES
        start_yn = WINDOW_HEIGHT_TILES
        self.tiles = [[EMPTY_TILE_NUM] * start_yn for _ in range(start_xn)]
    
    def loop(self):
        while self.running:
            self.handle_events()
            self.render()
    
    def handle_events(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                # exit game
                self.running = False
            elif event.type == pygame.MOUSEBUTTONUP:
                # cycle tiles
                self.handle_click(event)
            elif event.type == pygame.KEYUP:
                # move viewable portion of the level
                if event.key == pygame.K_LEFT:
                    self.shift_screen(-1, 0)
                elif event.key == pygame.K_RIGHT:
                    self.shift_screen(1, 0)
                elif event.key == pygame.K_UP:
                    self.shift_screen(0, -1)
                elif event.key == pygame.K_DOWN:
                    self.shift_screen(0, 1)
                # save the level to file
                elif event.key == pygame.K_s:
                    self.save_level()
    
    def handle_click(self, event):
        increment = 0
        if event.button == LMB:
            increment = 1
        elif event.button == RMB:
            increment = -1
        else:
            return
        xn, yn = self.tile_n_from_pos(event.pos[0], event.pos[1])
        self.cycle_tile(xn, yn, increment)
    
    def tile_n_from_pos(self, xpos, ypos):
        xn = xpos // TILE_SIDE_PX
        yn = ypos // TILE_SIDE_PX
        xn += self.xn_off
        yn += self.yn_off
        return xn, yn
    
    def shift_screen(self, x_shift, y_shift):
        # update the offsets
        self.xn_off += x_shift
        self.yn_off += y_shift
        
        # expand the level as the user shifts right
        while len(self.tiles) < WINDOW_WIDTH_TILES + self.xn_off:
            self.tiles.append([EMPTY_TILE_NUM] * len(self.tiles[0]))
        
        # shift left
        while self.xn_off < 0:
            self.tiles.insert(0, [EMPTY_TILE_NUM] * len(self.tiles[0]))
            self.xn_off += 1
        
        # expand the level as the user shifts downward
        while len(self.tiles[0]) < WINDOW_HEIGHT_TILES + self.yn_off:
            for xn in range(len(self.tiles)):
                self.tiles[xn].append(EMPTY_TILE_NUM)
        
        # shift upward
        while self.yn_off < 0:
            for xn in range(len(self.tiles)):
                self.tiles[xn].insert(0, EMPTY_TILE_NUM)
            self.yn_off += 1
    
    def cycle_tile(self, xn, yn, increment):
        t_type = self.tiles[xn][yn]
        t_type += increment
        
        # enforce bounds
        if t_type >= TILE_TYPE_COUNT:
            t_type = 0
        if t_type < 0:
            t_type = TILE_TYPE_COUNT - 1
        
        self.tiles[xn][yn] = t_type
    
    def render(self):        
        self.clear_screen()
        self.draw_tiles()
        self.draw_ui()
        pygame.display.flip()
    
    def clear_screen(self):
        self.screen.fill((1, 1, 1))
    
    def draw_tiles(self):
        for xn in range(len(self.tiles)):
            for yn in range(len(self.tiles[0])):
                t_type = self.tiles[xn][yn]
                if t_type == EMPTY_TILE_NUM:
                    # don't draw blank tiles
                    continue
                img = self.image_bank[t_type]
                rect = self.rect_from_coord(xn, yn)
                self.screen.blit(img, rect)
    
    def rect_from_coord(self, xn, yn):
        # adjust based on screen scroll
        screen_xn = xn - self.xn_off
        screen_yn = yn - self.yn_off
        
        rect = pygame.Rect(screen_xn * TILE_SIDE_PX, screen_yn * TILE_SIDE_PX, TILE_SIDE_PX, TILE_SIDE_PX)
        return rect
    
    def draw_ui(self):
        pass
        # show total size
        # show offsets
        #TODO put offsets in title bar?


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("level_file", help="Filename of the level to create/edit")
    args = parser.parse_args()
    
    ed = Editor()
    ed.setup(args.level_file)
    ed.loop()


