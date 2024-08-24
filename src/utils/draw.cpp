#include <SDL.h>
#include <SDL_ttf.h>
#include "init.h"
#include "drawHelperFunctions.h"
#include "defs.h"
#include "input.h"
#include "math.h"
#include <string>
#include <iostream>

void prepareScene(void)
{
    setRenderDrawColor(BACKGROUND_COLOR);
    SDL_RenderClear(app.renderer);
}


void renderText(int x, int y, std::string text) {
    SDL_Color white = {255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Solid(font, text.c_str(), white);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(app.renderer, surface);
    int textWidth = surface->w;
    int textHeight = surface->h;
    SDL_FreeSurface(surface);
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = textWidth;
    rect.h = textHeight;
    SDL_RenderCopy(app.renderer, texture, NULL, &rect);
}

void renderFPS(void) {
    std::string writeOnScreen = "FPS=";
    writeOnScreen.append(std::to_string((int)app.fps));
    renderText(0, SCREEN_HEIGHT-DEFAULT_FONT_PTSIZE, writeOnScreen);
}

void renderMouseRect(void) {
    SDL_Rect mouseRect;
    mouseRect.x = app.mousePosition.x - 5;
    mouseRect.y = app.mousePosition.y - 5;
    mouseRect.w = 10;
    mouseRect.h = 10;

    setRenderDrawColor(WHITE);
    SDL_RenderDrawRect(app.renderer, &mouseRect);
}

char map2dRepresentation[MAP_HEIGHT][MAP_WIDTH] = {
    {'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'},
    {'X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'},
    {'X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'},
    {'X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'},
    {'X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'},
    {'X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'},
    {'X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'},
    {'X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'},
    {'X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'},
    {'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'},
};

void renderMap2dRepresentation(char map2dRepresentation[MAP_HEIGHT][MAP_WIDTH]) {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++){
            if (map2dRepresentation[i][j] == 'X')
                setRenderDrawColor(BLACK);
            else
                setRenderDrawColor(WHITE);
            
            SDL_Rect mapCell;
            mapCell.x = i*CELL_2D_EDGE_SIZE;
            mapCell.y = j*CELL_2D_EDGE_SIZE;
            mapCell.w = CELL_2D_EDGE_SIZE;
            mapCell.h = CELL_2D_EDGE_SIZE;
            SDL_RenderFillRect(app.renderer, &mapCell);
        }
    }
}

void renderMap2dRepresentationFitToScreen(char map2dRepresentation[MAP_HEIGHT][MAP_WIDTH]) {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++){
            if (map2dRepresentation[i][j] == 'X')
                setRenderDrawColor(BLACK);
            else
                setRenderDrawColor(WHITE);
            
            SDL_Rect mapCell;
            mapCell.x = i*CELL_2D_WIDTH_FTS_SIZE;
            mapCell.y = j*CELL_2D_HEIGHT_FTS_SIZE;
            mapCell.w = CELL_2D_WIDTH_FTS_SIZE;
            mapCell.h = CELL_2D_HEIGHT_FTS_SIZE;
            SDL_RenderDrawRect(app.renderer, &mapCell);
        }
    }
}

SDL_Point createScaledPoint(SDL_Point point, float xScale, float yScale) {
    SDL_Point scaledPoint;
    scaledPoint.x = (int)((float)point.x*xScale);
    scaledPoint.y = (int)((float)point.y*yScale);
    return scaledPoint;
}

/* x and y are intended to be scaled to allow different representations. */
void renderPlayerOn2dMap(SDL_Point playerLocation, float xScale, float yScale, int width, int height) {
    SDL_Point scaledPlayerLocation = createScaledPoint(playerLocation, xScale, yScale);
    SDL_Rect playerLocationRect;
    playerLocationRect.x = scaledPlayerLocation.x - width/2;
    playerLocationRect.y = scaledPlayerLocation.y - height/2;
    playerLocationRect.w = width;
    playerLocationRect.h = height;

    setRenderDrawColor(RED);
    SDL_RenderFillRect(app.renderer, &playerLocationRect);
}

/* x and y are intended to be scaled to allow different representations. */
void renderLineOn2dMap(SDL_Point pt1, SDL_Point pt2, float xScale, float yScale, Color color) {
    SDL_Point scaledPt1 = createScaledPoint(pt1, xScale, yScale);
    SDL_Point scaledPt2 = createScaledPoint(pt2, xScale, yScale);
    setRenderDrawColor(color);
    SDL_RenderDrawLine(
        app.renderer,
        scaledPt1.x, scaledPt1.y,
        scaledPt2.x, scaledPt2.y
    );
}

void presentScene(void)
{   
    // Use 3d coordinates as base, so everything is in reference to 3d location.
    SDL_Point playerLocationScreen = {.x = 800, .y = 200 };
    SDL_Point mouseLocationScreen = {.x = app.mousePosition.x, .y = app.mousePosition.y };


    // TODO: Make these macros potentially
    float xScaleScreenTo3dMap = (float)CELL_3D_EDGE_SIZE*(float)MAP_WIDTH/(float)SCREEN_WIDTH;
    float yScaleScreenTo3dMap = (float)CELL_3D_EDGE_SIZE*(float)MAP_HEIGHT/(float)SCREEN_HEIGHT;

    float xScale3dMapToScreen = (float)SCREEN_WIDTH/((float)CELL_3D_EDGE_SIZE*(float)MAP_WIDTH);
    float yScale3dMapToScreen = (float)SCREEN_HEIGHT/((float)CELL_3D_EDGE_SIZE*(float)MAP_HEIGHT);

    float xScale3dMapTo2dMiniMap = (float)CELL_2D_EDGE_SIZE/(float)CELL_3D_EDGE_SIZE;
    float yScale3dMapTo2dMiniMap = (float)CELL_2D_EDGE_SIZE/(float)CELL_3D_EDGE_SIZE;

    // TODO: Store player location in app.
    SDL_Point playerLocation3dMap = createScaledPoint(playerLocationScreen, xScaleScreenTo3dMap, yScaleScreenTo3dMap);
    SDL_Point mouseLocation3dMap = createScaledPoint(mouseLocationScreen, xScaleScreenTo3dMap, yScaleScreenTo3dMap);

    renderMap2dRepresentationFitToScreen(map2dRepresentation);
    renderPlayerOn2dMap(playerLocation3dMap, xScale3dMapToScreen, yScale3dMapToScreen, 10, 10);

    renderLineOn2dMap(playerLocation3dMap, mouseLocation3dMap, xScale3dMapToScreen, yScale3dMapToScreen, GREEN);

    double slope = (
        (double)(playerLocation3dMap.y - mouseLocation3dMap.y)/
        (double)(playerLocation3dMap.x - mouseLocation3dMap.x)
    );

    double maxFovSlope = tan(atan(slope) + HALF_FOV_RADIANS);
    double minFovSlope = tan(atan(slope) - HALF_FOV_RADIANS);

    // For now leave the endpoints of the line 0 and max width b/c will change later.
    int yMaxFovSlopeX0 = (int)(maxFovSlope*(0 - playerLocation3dMap.x) + playerLocation3dMap.y);
    int yMaxFovSlopeXMax = (int)(maxFovSlope*(CELL_3D_EDGE_SIZE*MAP_WIDTH - playerLocation3dMap.x) + playerLocation3dMap.y);
    
    int yMinFovSlopeX0 = (int)(minFovSlope*(0 - playerLocation3dMap.x) + playerLocation3dMap.y);
    int yMinFovSlopeXMax = (int)(minFovSlope*(CELL_3D_EDGE_SIZE*MAP_WIDTH - playerLocation3dMap.x) + playerLocation3dMap.y);

    SDL_Point maxFovSlopeX0 = {.x=0, .y=yMaxFovSlopeX0};
    SDL_Point maxFovSlopeXMax = {.x=CELL_3D_EDGE_SIZE*MAP_WIDTH, .y=yMaxFovSlopeXMax};

    SDL_Point minFovSlopeX0 = {.x=0, .y=yMinFovSlopeX0};
    SDL_Point minFovSlopeXMax = {.x=CELL_3D_EDGE_SIZE*MAP_WIDTH, .y=yMinFovSlopeXMax};

    renderLineOn2dMap(maxFovSlopeX0, maxFovSlopeXMax, xScale3dMapToScreen, yScale3dMapToScreen, BLUE);
    renderLineOn2dMap(minFovSlopeX0, minFovSlopeXMax, xScale3dMapToScreen, yScale3dMapToScreen, BLUE);
    
    renderMap2dRepresentation(map2dRepresentation);
    renderPlayerOn2dMap(playerLocation3dMap, xScale3dMapTo2dMiniMap, xScale3dMapTo2dMiniMap, 4, 4);
    renderLineOn2dMap(playerLocation3dMap, mouseLocation3dMap, xScale3dMapTo2dMiniMap, yScale3dMapTo2dMiniMap, GREEN);

    renderLineOn2dMap(maxFovSlopeX0, maxFovSlopeXMax, xScale3dMapTo2dMiniMap, yScale3dMapTo2dMiniMap, BLUE);
    renderLineOn2dMap(minFovSlopeX0, minFovSlopeXMax, xScale3dMapTo2dMiniMap, yScale3dMapTo2dMiniMap, BLUE);
    renderMouseRect();

    renderFPS();

    SDL_RenderPresent(app.renderer);
}