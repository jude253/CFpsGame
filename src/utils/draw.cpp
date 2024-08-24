#include <SDL.h>
#include <SDL_ttf.h>
#include "init.h"
#include "drawHelperFunctions.h"
#include "defs.h"
#include "input.h"
#include <string>

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
            mapCell.x = i*10;
            mapCell.y = j*10;
            mapCell.w = 10;
            mapCell.h = 10;
            SDL_RenderDrawRect(app.renderer, &mapCell);
        }
    }

}

void presentScene(void)
{   
    renderMouseRect();

    renderMap2dRepresentation(map2dRepresentation);

    renderFPS();

    SDL_RenderPresent(app.renderer);
}