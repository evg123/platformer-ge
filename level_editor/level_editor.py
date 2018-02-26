#!/bin/env python3

import sys
import pygame

TILE_SIDE_PX = 32
TILE_TYPE_COUNT = 2
TILE_IMAGE_PATTERN = '../Assets/images/tiles/tile_{0}.png'

LMB = 1
RMB = 3

class editor:
    
    screen = None
    running = True
    
    xn_off, yn_off = 0
    
    image_bank = []
    
    
    def setup(self):
        self.screen = pygame.display.set_mode(1280, 720)
        self.load_image_bank()
    
    def load_image_bank(self):
        for num in range(TILE_TYPE_COUNT):
            filename = TILE_IMAGE_PATTERN.format(num)
            img = pygame.image.load(filename)
            image_bank.append(img)
    
    def loop(self):
        while running:
            self.handle_events()
            self.render()
    
    def handle_events(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
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
        cycle_tile(screen_xn, screen_yn)
    
    def tile_n_from_pos(xpos, ypos):
        
    
    def shift_screen(self):
        pass
        # expand the level as the user shifts left and right
    
    def cycle_tile(self, xn, yn, increment):
        t_type = self.tiles[xn][yn]
        t_type += 1
        if t_type >= TILE_TYPE_COUNT:
            t_type = 0
        self.tiles[xn][yn] = t_type
    
    def render(self):        
        screen.fill(1, 1, 1)
        self.clear_screen()
        self.draw_tiles()
        self.draw_ui()
        pygame.display.flip()
    
    def draw_tiles(self):
        for xn in range(len(self.tiles)):
            for yn in range(len(self.tiles[0])):
                t_type = self.tiles[xn][yn]
                img = image_bank[t_type]
                rect = rect_from_coord(xn, yn)
                self.screen.blit(img, rect)
    
    def rect_from_coord(self, xn, yn):
        # adjust based on screen scroll
        screen_xn = xn - xn_off
        screen_yn = yn - yn_off
        
        rect = Rect(screen_xn * TILE_SIDE_PX, screen_yn * TILE_SIDE_PX, TILE_SIDE_PX, TILE_SIDE_PX)
        return rect
    
    def draw_ui(self):
        pass
        # show total size
        # show offsets


if __name__ == "__main__":
    setup()
    loop()


