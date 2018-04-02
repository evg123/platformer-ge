# Part 2

# The X-Factor

Shigeru Miyamoto is impressed with the progress on your platformer. He has in fact never seen so much done so quickly! He thinks the tools you have developed should help create a very polished game and help you iterate much more quickly. He suggests something is missing, some sort of "X-Factor" that will make your game stand out from all the other platformers.


## Your tasks

### Special Mechanic

From part 1 *You should think about a 'special feature' or gameplay mechanic that makes your game unique.*

Part 2 will be implmementing this 'special feature' that makes your game unique. Some game mechanic, some graphical effect that makes not only your game unique, but your 'game engine' have a  special feature.

Some ideas of unique game mechanics.
- A fancy graphical effect with lighting
- Implementing a parallax scrolling system
- If a powerup is hit, perhaps the world changes
  - Because we are using tilemaps, perhaps every tilemap has a corresponding tile that also changes (i.e. a day and night version of your tiles could be one example, or winter and summer)
- Procedurally generated tilemaps
- A character who can fly or float
- Music or sound effects are played (or sped up) based on some action.

The idea is that you do something creative here!

### Enemy AI

From part 1 *You should think about 'enemy characters' (This will be implemented in part 2)*

Enemies in platformers are typically characters that you can eliminate by jumping on top of. If the enemy however runs into you first, then your character loses a life (or otherwise it is game over if only one life is given)!

Enemies additionally move back and forth, and do not move through tiles (unless they are specifically ghosts!).

The goal here is to implement your enemy AI system. Nothing complex, just get enemies that work!

## Rubric (5% of overall grade)

* (50%) Special Feature (X-Factor).
  * A brief description of the feature and implementation should be documented in part 3.
* (50%) Are there enemies? 
  * (And do they collide and interact properly with the world. Can a player reliably eliminate them, or if not eliminate them, dose the player collide with enemies and lose the game appropriately?)
