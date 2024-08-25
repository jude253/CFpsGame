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

void renderCenteredSquareOn2dMap(SDL_Point centerPoint, float xScale, float yScale, int width, Color color) {
    SDL_Point scaledCenterPoint = createScaledPoint(centerPoint, xScale, yScale);
    SDL_Rect square;
    square.x = scaledCenterPoint.x - width/2;
    square.y = scaledCenterPoint.y - width/2;
    square.w = width;
    square.h = width;

    setRenderDrawColor(color);
    SDL_RenderDrawRect(app.renderer, &square);
}

void renderRectOn2dMap(SDL_Point point, float xScale, float yScale, int width, int height, Color color) {
    SDL_Point scaledPoint = createScaledPoint(point, xScale, yScale);
    SDL_Rect rect;
    rect.x = scaledPoint.x;
    rect.y = scaledPoint.y;
    rect.w = width;
    rect.h = height;

    setRenderDrawColor(color);
    SDL_RenderDrawRect(app.renderer, &rect);
}

void printPoint(SDL_Point point) {
    std::cout << '(' << point.x << ',' << point.y << ')' << '\n';
}


bool isOpenGrid(char map2dRepresentation[MAP_HEIGHT][MAP_WIDTH], SDL_Point map2dIndex) {
    // Add extra bounds check just to be sure, even at the cost of performance.
    if ( 
        map2dIndex.x > MAP_WIDTH  || map2dIndex.x < 0 || 
        map2dIndex.y > MAP_HEIGHT || map2dIndex.y < 0
    )
        return false;

    return map2dRepresentation[map2dIndex.y][map2dIndex.x] != 'X';
}

/**
 * Remove scaling and only do it against grid once certain that it works
 * with required functionality.  Also, there could be room for
 * optimization in checking points.
 */
SDL_Point castRay2dMapDebug(
    char map2dRepresentation[MAP_HEIGHT][MAP_WIDTH], 
    SDL_Point startPoint3dMap, 
    double angle,
    int stepSize,
    float xScale,
    float yScale
) {

    double playerXComponent = cos(angle);
    double playerYComponent = sin(angle);

    SDL_Point prevPlayerLineOf3dMapPoint = {
        .x = startPoint3dMap.x,
        .y = startPoint3dMap.y,
    };
    SDL_Point playerLineOf3dMapPoint;
    SDL_Point playerLineOfSightScreenPoint;

    for (int i = 0; i < 10000; i++) {
        playerLineOf3dMapPoint = {
            .x = (int) (i * stepSize * playerXComponent + (double) startPoint3dMap.x),
            .y = (int) (i * stepSize * playerYComponent + (double) startPoint3dMap.y)
        };
        
        if (!isOpenGrid(app.map2dRepresentation, {
            .x = (int)round(playerLineOf3dMapPoint.x/CELL_3D_EDGE_SIZE),
            .y = (int)round(playerLineOf3dMapPoint.y/CELL_3D_EDGE_SIZE)
        }))
            break;

        playerLineOfSightScreenPoint = createScaledPoint(
            playerLineOf3dMapPoint, xScale, yScale
        );
        
        setRenderDrawColor(RED);
        SDL_RenderDrawPoint(
            app.renderer, 
            playerLineOfSightScreenPoint.x, 
            playerLineOfSightScreenPoint.y
        );
        prevPlayerLineOf3dMapPoint = {
            .x = playerLineOf3dMapPoint.x,
            .y = playerLineOf3dMapPoint.y
        };
    }

    return prevPlayerLineOf3dMapPoint;
}


SDL_Point castRay(
    char map2dRepresentation[MAP_HEIGHT][MAP_WIDTH], 
    SDL_Point startPoint3dMap, 
    double angle,
    int stepSize
) {
    double playerXComponent = cos(angle);
    double playerYComponent = sin(angle);

    SDL_Point prevPlayerLineOf3dMapPoint = {
        .x = startPoint3dMap.x,
        .y = startPoint3dMap.y,
    };
    SDL_Point playerLineOf3dMapPoint;
    SDL_Point playerLineOfSightScreenPoint;

    for (int i = 0; i < 10000; i++) {
        playerLineOf3dMapPoint = {
            .x = (int) (i * stepSize * playerXComponent + (double) startPoint3dMap.x),
            .y = (int) (i * stepSize * playerYComponent + (double) startPoint3dMap.y)
        };
        
        if (!isOpenGrid(app.map2dRepresentation, {
            .x = (int)round(playerLineOf3dMapPoint.x/CELL_3D_EDGE_SIZE),
            .y = (int)round(playerLineOf3dMapPoint.y/CELL_3D_EDGE_SIZE)
        }))
            break;

        prevPlayerLineOf3dMapPoint = {
            .x = playerLineOf3dMapPoint.x,
            .y = playerLineOf3dMapPoint.y
        };
    }
    return prevPlayerLineOf3dMapPoint;
}

// Return the distance instead of the last point before colision.
int castRayDist(
    char map2dRepresentation[MAP_HEIGHT][MAP_WIDTH], 
    SDL_Point startPoint3dMap, 
    double angle,
    int stepSize
) {
    double playerXComponent = cos(angle);
    double playerYComponent = sin(angle);


    SDL_Point playerLineOf3dMapPoint;
    SDL_Point playerLineOfSightScreenPoint;
    int dist;
    int i = 0;
    while (true) {
        playerLineOf3dMapPoint = {
            .x = (int) (i * stepSize * playerXComponent + (double) startPoint3dMap.x),
            .y = (int) (i * stepSize * playerYComponent + (double) startPoint3dMap.y)
        };
        
        if (!isOpenGrid(app.map2dRepresentation, {
            .x = (int)round(playerLineOf3dMapPoint.x/CELL_3D_EDGE_SIZE),
            .y = (int)round(playerLineOf3dMapPoint.y/CELL_3D_EDGE_SIZE)
        }))
            break;
        i+=1;
        dist = i*stepSize;
    }

    return dist;
}

void render2dMapFitToScreenDebug(SDL_Point playerUnitVectorFrom3dMapLocation) {
    renderMap2dRepresentationFitToScreen(app.map2dRepresentation);
    
    double minFovAngle = app.playerAngle - HALF_FOV_RADIANS;

    // Seems to handle up to about 1000 rays without dropping frames on my computer.
    int numberOfRays = 100;
    double rayAngle;
    for (int i = 0; i < numberOfRays; i++) {
        rayAngle = minFovAngle + i * (FOV_RADIANS / (double) numberOfRays);
        castRay2dMapDebug(app.map2dRepresentation, app.playerPosition3dMap, rayAngle, 2, X_SCALE_3D_MAP_TO_S, Y_SCALE_3D_MAP_TO_S);
    }

    renderPlayerOn2dMap(app.playerPosition3dMap, X_SCALE_3D_MAP_TO_S, Y_SCALE_3D_MAP_TO_S, 10, 10);
    renderLineOn2dMap(app.playerPosition3dMap, playerUnitVectorFrom3dMapLocation, X_SCALE_3D_MAP_TO_S, Y_SCALE_3D_MAP_TO_S, DARK_RED);
}

void presentScene(void)
{   

    SDL_Point playerLocation3dMap = app.playerPosition3dMap;

    double playerAngle = app.playerAngle;
    double playerXComponent = cos(playerAngle);
    double playerYComponent = sin(playerAngle);

    SDL_Point playerUnitVectorFrom3dMapLocation = {
        .x = (int) (CELL_3D_EDGE_SIZE * playerXComponent + (double) playerLocation3dMap.x), 
        .y = (int) (CELL_3D_EDGE_SIZE * playerYComponent + (double) playerLocation3dMap.y)
    };

    // render2dMapFitToScreenDebug(playerUnitVectorFrom3dMapLocation);

    // BUG: why does it not display all columns if numberOfRays = 100?
    // BUG: why does it seem like the far corners don't show anything?
    // BUG: make it less boxy
    int numberOfRays = 256;
    int colWidth = (double) SCREEN_WIDTH / (double) numberOfRays;
    SDL_Rect drawCol;

    double rayAngle;
    double minFovAngle = app.playerAngle - HALF_FOV_RADIANS;
    int dist;
    for (int i = 0; i < numberOfRays; i++) {
        rayAngle = minFovAngle + i * (FOV_RADIANS / (double) numberOfRays);
        dist = castRayDist(app.map2dRepresentation, app.playerPosition3dMap, rayAngle, 2);
        drawCol.x = i*colWidth;
        drawCol.y = SCREEN_HEIGHT/2;
        drawCol.w = colWidth;
        drawCol.h = SCREEN_HEIGHT/dist*30;
        setRenderDrawColor({
            .r = BLACK.r,
            .g = BLACK.g,
            .b = (Uint8)(255-255*dist/(MAP_WIDTH*CELL_3D_EDGE_SIZE)),
            .a = BLACK.a,
        });

        SDL_RenderFillRect(app.renderer, &drawCol);
        drawCol.h = -SCREEN_HEIGHT/dist*30;
        SDL_RenderFillRect(app.renderer, &drawCol);
    }
    std::cout << dist << '\n';

    renderMap2dRepresentation(app.map2dRepresentation);
    
    renderPlayerOn2dMap(playerLocation3dMap, X_SCALE_3D_MAP_TO_MINIMAP, Y_SCALE_3D_MAP_TO_MINIMAP, 4, 4);
    renderLineOn2dMap(playerLocation3dMap, playerUnitVectorFrom3dMapLocation, X_SCALE_3D_MAP_TO_MINIMAP, Y_SCALE_3D_MAP_TO_MINIMAP, DARK_RED);

    renderFPS();

    SDL_RenderPresent(app.renderer);
}