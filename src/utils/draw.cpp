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

void renderMap2dRepresentation(char map2dRepresentation[MAP_HEIGHT][MAP_WIDTH]) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++){
            if (map2dRepresentation[y][x] == 'X')
                setRenderDrawColor(BLACK);
            else
                setRenderDrawColor(WHITE);
            
            SDL_Rect mapCell;
            mapCell.x = x*CELL_2D_EDGE_SIZE;
            mapCell.y = y*CELL_2D_EDGE_SIZE;
            mapCell.w = CELL_2D_EDGE_SIZE;
            mapCell.h = CELL_2D_EDGE_SIZE;
            SDL_RenderFillRect(app.renderer, &mapCell);
        }
    }
}

void renderMap2dRepresentationFitToScreen(char map2dRepresentation[MAP_HEIGHT][MAP_WIDTH]) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++){
            if (map2dRepresentation[y][x] == 'X')
                setRenderDrawColor(BLACK);
            else
                setRenderDrawColor(WHITE);
            SDL_Rect mapCell;
            mapCell.x = x*CELL_2D_WIDTH_FTS_SIZE;
            mapCell.y = y*CELL_2D_HEIGHT_FTS_SIZE;
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


void renderSquareOn2dMap(SDL_Point centerPoint, float xScale, float yScale, int width, Color color) {
    SDL_Point scaledCenterPoint = createScaledPoint(centerPoint, xScale, yScale);
    SDL_Rect square;
    square.x = scaledCenterPoint.x - width/2;
    square.y = scaledCenterPoint.y - width/2;
    square.w = width;
    square.h = width;

    setRenderDrawColor(color);
    SDL_RenderDrawRect(app.renderer, &square);
}

void printPoint(SDL_Point point) {
    std::cout << '(' << point.x << ',' << point.y << ')' << '\n';
}

SDL_Point getFirstIntersectionPoint(char map2dRepresentation[MAP_HEIGHT][MAP_WIDTH], 
    SDL_Point start, double angle, int stepSize) 
{   
    double xUnitComponent = cos(angle);
    double yUnitComponent = sin(angle);
    double xCoordinate;
    double yCoordinate;
    for (int i = 1; i < 3; i++) {
        xCoordinate = (double) i * stepSize * xUnitComponent + (double) start.x;
        yCoordinate = (double) i * stepSize * yUnitComponent + (double) start.y;
        // std::cout << '(' << xCoordinate << ',' << yCoordinate << ')';
    }
    // std::cout << '\n';

    return {.x = (int) xCoordinate, .y = (int) yCoordinate};
}

void presentScene(void)
{   

    // TODO: Make these macros potentially
    float xScaleScreenTo3dMap = (float)CELL_3D_EDGE_SIZE*(float)MAP_WIDTH/(float)SCREEN_WIDTH;
    float yScaleScreenTo3dMap = (float)CELL_3D_EDGE_SIZE*(float)MAP_HEIGHT/(float)SCREEN_HEIGHT;

    float xScale3dMapToScreen = (float)SCREEN_WIDTH/((float)CELL_3D_EDGE_SIZE*(float)MAP_WIDTH);
    float yScale3dMapToScreen = (float)SCREEN_HEIGHT/((float)CELL_3D_EDGE_SIZE*(float)MAP_HEIGHT);

    float xScale3dMapTo2dMiniMap = (float)CELL_2D_EDGE_SIZE/(float)CELL_3D_EDGE_SIZE;
    float yScale3dMapTo2dMiniMap = (float)CELL_2D_EDGE_SIZE/(float)CELL_3D_EDGE_SIZE;

    SDL_Point playerLocation3dMap = app.playerPosition3dMap;


    double playerAngle = app.playerAngle;
    double slope = tan(app.playerAngle);

    SDL_Point playerUnitVectorFrom3dMapLocation = {
        .x = (int) (CELL_3D_EDGE_SIZE * cos(playerAngle) + (double) playerLocation3dMap.x), 
        .y = (int) (CELL_3D_EDGE_SIZE * sin(playerAngle) + (double) playerLocation3dMap.y)
    };

    double maxFovAngle = playerAngle + HALF_FOV_RADIANS;
    double minFovAngle = playerAngle - HALF_FOV_RADIANS;

    SDL_Point playerMaxFovAngleUnitVectorFrom3dMapLocation = {
        .x = (int) (CELL_3D_EDGE_SIZE * cos(maxFovAngle) + (double) playerLocation3dMap.x), 
        .y = (int) (CELL_3D_EDGE_SIZE * sin(maxFovAngle) + (double) playerLocation3dMap.y)
    };

    SDL_Point playerMinFovAngleUnitVectorFrom3dMapLocation = {
        .x = (int) (CELL_3D_EDGE_SIZE * cos(minFovAngle) + (double) playerLocation3dMap.x), 
        .y = (int) (CELL_3D_EDGE_SIZE * sin(minFovAngle) + (double) playerLocation3dMap.y)
    };


    renderMap2dRepresentationFitToScreen(app.map2dRepresentation);
    renderPlayerOn2dMap(playerLocation3dMap, xScale3dMapToScreen, yScale3dMapToScreen, 10, 10);
    renderLineOn2dMap(playerLocation3dMap, playerUnitVectorFrom3dMapLocation, xScale3dMapToScreen, yScale3dMapToScreen, DARK_RED);
    renderLineOn2dMap(playerLocation3dMap, playerMaxFovAngleUnitVectorFrom3dMapLocation, xScale3dMapToScreen, yScale3dMapToScreen, BLUE);
    renderLineOn2dMap(playerLocation3dMap, playerMinFovAngleUnitVectorFrom3dMapLocation, xScale3dMapToScreen, yScale3dMapToScreen, BLUE);

    
    renderMap2dRepresentation(app.map2dRepresentation);
    renderPlayerOn2dMap(playerLocation3dMap, xScale3dMapTo2dMiniMap, xScale3dMapTo2dMiniMap, 4, 4);
    renderLineOn2dMap(playerLocation3dMap, playerUnitVectorFrom3dMapLocation, xScale3dMapTo2dMiniMap, xScale3dMapTo2dMiniMap, DARK_RED);
    renderLineOn2dMap(playerLocation3dMap, playerMaxFovAngleUnitVectorFrom3dMapLocation, xScale3dMapTo2dMiniMap, xScale3dMapTo2dMiniMap, BLUE);
    renderLineOn2dMap(playerLocation3dMap, playerMinFovAngleUnitVectorFrom3dMapLocation, xScale3dMapTo2dMiniMap, xScale3dMapTo2dMiniMap, BLUE);

    SDL_Point testPt = getFirstIntersectionPoint(
        app.map2dRepresentation, playerLocation3dMap, playerAngle, CELL_3D_EDGE_SIZE
    );

    renderSquareOn2dMap(testPt, xScale3dMapToScreen, yScale3dMapToScreen, 10, GREEN);

    SDL_Point testPtTruncated = {
        .x = (int)round(testPt.x/CELL_3D_EDGE_SIZE)*CELL_3D_EDGE_SIZE,
        .y = (int)round(testPt.y/CELL_3D_EDGE_SIZE)*CELL_3D_EDGE_SIZE,
    };
    printPoint(testPtTruncated);
    renderSquareOn2dMap(testPtTruncated, xScale3dMapToScreen, yScale3dMapToScreen, 10, YELLOW);

    renderFPS();

    SDL_RenderPresent(app.renderer);
}