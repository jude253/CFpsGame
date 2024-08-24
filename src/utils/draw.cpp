#include <SDL.h>
#include <SDL_ttf.h>
#include "init.h"
#include "drawHelperFunctions.h"
#include "defs.h"
#include "input.h"
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
            SDL_RenderDrawRect(app.renderer, &mapCell);
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

/*
x and y are intended to be relative to the 3D grid coordinate system,
and are scaled accordingly.
*/
void renderPlayerOn2dMap(int x, int y, int xScale, int yScale) {
    SDL_Rect playerLocation;
    playerLocation.x = (int)((float)x/(float)CELL_3D_EDGE_SIZE*xScale) - 5;
    playerLocation.y = (int)((float)y/(float)CELL_3D_EDGE_SIZE*yScale) - 5;
    playerLocation.w = 10;
    playerLocation.h = 10;

    setRenderDrawColor(RED);
    SDL_RenderFillRect(app.renderer, &playerLocation);
}

void renderPlayerMouseLineOn2dMap(int playerX, int playerY, int xScale, int yScale) {
    SDL_Rect playerLocation;
    int scaledPlayerX = (int)((float)playerX/(float)CELL_3D_EDGE_SIZE*xScale);
    int scaledPlayerY = (int)((float)playerY/(float)CELL_3D_EDGE_SIZE*yScale);

    setRenderDrawColor(GREEN);
    SDL_RenderDrawLine(
        app.renderer,
        scaledPlayerX, scaledPlayerY,
        app.mousePosition.x, app.mousePosition.y
    );
}

void presentScene(void)
{   
    int playerX = 650;
    int playerY = 850;

    renderMap2dRepresentationFitToScreen(map2dRepresentation);
    renderPlayerOn2dMap(playerX, playerY, CELL_2D_WIDTH_FTS_SIZE, CELL_2D_HEIGHT_FTS_SIZE);
    renderMouseRect();

    renderPlayerMouseLineOn2dMap(playerX, playerY, CELL_2D_WIDTH_FTS_SIZE, CELL_2D_HEIGHT_FTS_SIZE);

    renderFPS();

    SDL_RenderPresent(app.renderer);
}