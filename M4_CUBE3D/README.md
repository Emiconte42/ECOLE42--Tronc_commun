*This project has been created as part of the 42 curriculum by emiconte, hbelleuv.*

# CUB3D
## Description

Cub3D is a small first-person 3D game, rendered like Wolfenstein 3D.
You load a map from a `.cub` file, and the player can move around in it with real textured walls, a minimap, and doors that open.

The point of the project is mostly to understand how raycasting works: how you can give the impression of being in 3D when you're actually only doing 2D calculations on a map.

### Parsing

Before launching the game, we need to read the `.cub` file and check that it's correct.
We get the texture paths (NO, SO, WE, EA), the floor and ceiling colors, and the map itself.

The most annoying part here was validating the map.
The map has to be closed off by walls, there has to be exactly one starting point for the player, and only the right characters can be used.
We check every cell by looking at its neighbors (up, down, left, right) to make sure you can't walk out of the map.

### Raycasting

This is the core of the project. For every column of pixels on screen, we send out a ray from the player and calculate where it hits a wall.
We use the DDA algorithm: instead of checking pixel by pixel, the ray jumps from cell to cell on the map until it hits a wall.
That's a lot faster.

Once we know where the ray hit a wall, we calculate the real distance (not the straight-line distance, otherwise you get an ugly fisheye effect), and from that we get the height of the wall to draw on screen.
The farther the wall, the smaller it is.

### Textures

Each wall shows the right texture depending on which face it is (north, south, east, west), and depending on exactly where the ray hit it.
We calculate a horizontal coordinate on the wall (`wall_x`) to know which column of the texture to draw, column by column, pixel by pixel.

### Doors

Doors are special cells on the map (`D`).
They can open and close with the space bar.
Rendering a door is a bit trickier than a normal wall, because you have to draw the door AND whatever is behind it when it's open.
To do that we cast a second ray behind the door to find the background wall.

### Minimap

In the top left corner of the screen we show a minimap with the map seen from above, the player's position, the direction they're looking (a small beam), and the letters N/S/E/W to get your bearings.

## Instructions

### Compilation

```bash
make
./cub3d maps/valid/valid_basic.cub
```

### Controls

| Key                | Action               |
|--------------------|----------------------|
| W / up arrow       | move forward         |
| S / down arrow     | move backward        |
| A                  | strafe left          |
| D                  | strafe right         |
| left/right arrows  | turn the camera      |
| mouse              | turn the camera      |
| M                  | toggle mouse on/off  |
| space              | open/close a door    |
| F                  | shoot                |
| esc                | quit                 |

### Cleanup

```bash
make clean
make fclean
make re
```

## Resources

### Technical References

Lodev - Raycasting tutorial : https://lodev.org/cgtutor/raycasting.html
The raycasting bible. Explains the DDA, the wall_x calculation, the textures. This is directly where the logic in 2_dda.c and 3_texture.c comes from.

Permadi - Ray casting tutorial : https://permadi.com/1996/05/ray-casting-tutorial-table-of-contents/
Another explanation of the same thing, with different diagrams. Useful if Lodev isn't clear on some point.

MiniLibX docs (42 School) : https://harm-smits.github.io/42docs/libs/minilibx
Unofficial but complete doc on all the mlx functions (mlx_hook, mlx_put_image_to_window, etc).

DDA Line Drawing Algorithm : https://www.youtube.com/watch?v=NbSee-XM7WA
Good to understand the DDA algorithm on its own, without mixing it up with the raycasting part.

### AI Usage

AI (Claude, Anthropic) was used to help us understand certain technical points
and to suggest debug printf placements during development.