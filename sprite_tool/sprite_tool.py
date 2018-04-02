#!/usr/bin/env python3

import sys
import os
import pygame
import argparse

WINDOW_WIDTH = 640
WINDOW_HEIGHT = 480

TICKS_PER_FRAME = 250
SCALE = 6

class SpriteTool(object):
    
    def __init__(self):
        self.screen = None
        self.running = True
    
    def setup(self, sprite_file, sprite_w, sprite_h, frame_start, frame_end):
        pygame.init()
        self.screen = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
        self.sprite_w = sprite_w * SCALE
        self.sprite_h = sprite_h * SCALE
        self.frame_start = frame_start
        self.frame_end = frame_end
        self.sprite_frame = self.frame_start
        
        self.load_sprite(sprite_file)
        self.last_frame_time = pygame.time.get_ticks()
    
    def load_sprite(self, sprite_file):
        # check if file exists
        if not os.path.isfile(sprite_file):
            print("sprite file not found")
            sys.exit(1)
        
        self.sprite_sheet = pygame.image.load(sprite_file)
        self.sprite_sheet = pygame.transform.scale(self.sprite_sheet,
                                                   (self.sprite_sheet.get_width() * SCALE,
                                                   self.sprite_sheet.get_height() * SCALE))
        sheet_rect = self.sprite_sheet.get_rect()
        self.frames_per_line = sheet_rect.w // self.sprite_w
        
    def loop(self):
        while self.running:
            self.handle_events()
            self.advance_sprite()
            self.render()
    
    def handle_events(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                # exit game
                self.running = False
    
    def advance_sprite(self):
        now = pygame.time.get_ticks()
        if now - self.last_frame_time > TICKS_PER_FRAME:
            self.sprite_frame += 1
            if self.sprite_frame > self.frame_end:
                self.sprite_frame = self.frame_start
            self.last_frame_time = now
    
    def render(self):        
        self.clear_screen()
        self.draw_sprite()
        self.draw_ui()
        pygame.display.flip()
    
    def clear_screen(self):
        self.screen.fill((1, 1, 1))
    
    def draw_sprite(self):
        src_rect = self.rect_from_frame(self.sprite_frame)
        dest_rect = pygame.Rect(WINDOW_WIDTH / 2 - self.sprite_w / 2,
                                WINDOW_HEIGHT / 2 - self.sprite_h / 2, 
                                self.sprite_w, self.sprite_h)
        self.screen.blit(self.sprite_sheet, dest_rect, src_rect)
    
    def rect_from_frame(self, frame):
        x_idx = frame % self.frames_per_line
        y_idx = frame // self.frames_per_line
        x_pos = x_idx * self.sprite_w
        y_pos = y_idx * self.sprite_h
        return pygame.Rect(x_pos, y_pos, self.sprite_w, self.sprite_h)
    
    def draw_ui(self):
        pass


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("sprite_file", help="Filename of the sprite sheet to view")
    parser.add_argument("sprite_width", type=int, help="Width in pixels of each frame of the sprite sheet")
    parser.add_argument("sprite_height", type=int, help="Height in pixels of each frame of the sprite sheet")
    parser.add_argument("frame_start", type=int, help="0-based index of the frame to start looping from")
    parser.add_argument("frame_end", type=int, help="0-based index of the last frame to use in the loop")
    args = parser.parse_args()
    
    st = SpriteTool()
    st.setup(args.sprite_file, args.sprite_width, args.sprite_height, args.frame_start, args.frame_end)
    st.loop()


