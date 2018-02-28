#!/usr/bin/env python3

import sys
import pygame

WINDOW_WIDTH = 1280
WINDOW_HEIGHT = 720

TILE_SIDE_PX = 32
TILE_TYPE_COUNT = 2
NEW_TILE_NUM = 0
TILE_IMAGE_PATTERN = './Assets/images/tiles/tile_{0}.png'

WINDOW_WIDTH_XN = WINDOW_WIDTH // TILE_SIDE_PX
WINDOW_HEIGHT_XN = WINDOW_HEIGHT // TILE_SIDE_PX

LMB = 1
RMB = 3

class Editor(object):
    
    def __init__(self):
        self.screen = None
        self.running = True
        self.tiles = []
        self.xn_off = 0
        self.yn_off = 0
        self.image_bank = []
    
    def setup(self, level_file):
        self.screen = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
        self.load_image_bank()
        if level_file:
            self.load_level(level_file)
        else:
            self.blank_level()
    
    def load_image_bank(self):
        for num in range(TILE_TYPE_COUNT):
            filename = TILE_IMAGE_PATTERN.format(num)
            img = pygame.image.load(filename)
            self.image_bank.append(img)
    
    def load_level(self, level_file):
        # load an existing level from a file
        pass
    
    def blank_level(self):
        start_xn = WINDOW_WIDTH_XN
        start_yn = WINDOW_HEIGHT_XN
        self.tiles = [[NEW_TILE_NUM] * start_yn for _ in range(start_xn)]
    
    def loop(self):
        while self.running:
            self.handle_events()
            self.render()
    
    def handle_events(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                self.running = False
            elif event.type == pygame.MOUSEBUTTONUP:
                self.handle_click(event)
            # move viewable portion of the level
    
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
        return xn, yn
    
    def shift_screen(self, x_shift, y_shift):
        # update the offsets
        self.xn_off += x_shift
        self.yn_off += y_shift
        
        # expand the level as the user shifts right
        while len(self.tiles[0]) < WINDOW_WIDTH_XN + self.xn_off:
            for xn in range(len(self.tiles[0])):
                self.tiles[xn].append(0)
        
        # expand the level as the user shifts downward
        while len(self.tiles[0]) < WINDOW_HEIGHT_XN + self.yn_off:
            self.tiles.append([NEW_TILE_NUM] * len(self.tiles[0]))
    
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
    ed = Editor()
    ed.setup(None)
    ed.loop()


