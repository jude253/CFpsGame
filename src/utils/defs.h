#pragma once

#define SCREEN_WIDTH   1280
#define SCREEN_HEIGHT  720

// Only support one map shape for prototyping.
#define MAP_WIDTH 10   // Number of cells
#define MAP_HEIGHT 12  // Number of cells

#define CELL_2D_WIDTH_FTS_SIZE SCREEN_WIDTH/MAP_WIDTH  // 2D cell width to fill screen
#define CELL_2D_HEIGHT_FTS_SIZE SCREEN_HEIGHT/MAP_HEIGHT // 2D cell height to fill screen

#define CELL_2D_EDGE_SIZE 10  // Cell width and height size for 2d square representation
#define CELL_3D_EDGE_SIZE 100  // Cell width and height size for 3d square representation

#define TARGET_FPS 60
#define TARGET_SECONDS_PER_FRAME 1.0f/TARGET_FPS


#define DEFAULT_FONT_PTSIZE 24 //point size to use for font.

#define PI 3.1415926535
#define FOV_DEGREES 45.0
#define FOV_RADIANS FOV_DEGREES*PI/180
#define HALF_FOV_RADIANS FOV_RADIANS/2.0


#define PLAYER_MOVE_3D_STEP 30
#define PLAYER_ROTATE_3D_STEP PI/16.0  // Radians

#define X_SCALE_S_TO_3D_MAP (float)CELL_3D_EDGE_SIZE*(float)MAP_WIDTH/(float)SCREEN_WIDTH // Scale in x direction: screen coordinates to 3d map coordinates
#define Y_SCALE_S_TO_3D_MAP (float)CELL_3D_EDGE_SIZE*(float)MAP_HEIGHT/(float)SCREEN_HEIGHT // Scale in y direction: screen coordinates to 3d map coordinates

#define X_SCALE_3D_MAP_TO_S (float)SCREEN_WIDTH/((float)CELL_3D_EDGE_SIZE*(float)MAP_WIDTH) // Scale in x direction: 3d map coordinates to screen coordinates
#define Y_SCALE_3D_MAP_TO_S (float)SCREEN_HEIGHT/((float)CELL_3D_EDGE_SIZE*(float)MAP_HEIGHT) // Scale in y direction: 3d map coordinates to screen coordinates

#define X_SCALE_3D_MAP_TO_MINIMAP (float)CELL_2D_EDGE_SIZE/(float)CELL_3D_EDGE_SIZE // Scale in x direction: 3d map coordinates to mini map coordinates
#define Y_SCALE_3D_MAP_TO_MINIMAP (float)CELL_2D_EDGE_SIZE/(float)CELL_3D_EDGE_SIZE // Scale in y direction: 3d map coordinates to mini map coordinates
