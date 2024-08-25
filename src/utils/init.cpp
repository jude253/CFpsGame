#include <SDL.h>
#include <SDL_ttf.h>
#include "defs.h"
#include "structs.h"
#include <string>

App app;
TTF_Font *font;

Color RED                =   {.r = 255, .g=0,   .b=0,   .a=255};
Color DARK_RED           =   {.r = 96,  .g=0,   .b=0,   .a=255};
Color BLUE               =   {.r = 0,   .g=0,   .b=255, .a=255};
Color GREEN              =   {.r = 0,   .g=255, .b=0,   .a=255};
Color YELLOW             =   {.r = 255, .g=255, .b=0,   .a=255};
Color WHITE              =   {.r = 255, .g=255, .b=255, .a=255};
Color BLACK              =   {.r = 0,   .g=0,   .b=0,   .a=255};
Color BLACK_75_PERC      =   {.r = 0,   .g=0,   .b=192, .a=255};
Color BLACK_50_PERC      =   {.r = 0,   .g=0,   .b=128, .a=255};
Color BLACK_25_PERC      =   {.r = 0,   .g=0,   .b=64,  .a=255};
Color BACKGROUND_COLOR   =   {.r = 96,  .g=128, .b=255, .a=255};

void initSDL(void)
{
    int rendererFlags, windowFlags;

    rendererFlags = SDL_RENDERER_ACCELERATED;

    windowFlags = 0;

    // Initialize app data before game starts.
    app.gameStart = SDL_GetPerformanceCounter();
    app.frameStart = app.gameStart;
    app.secondsElapsed = 0;
    app.renderFrameSeconds = 0.0f;
    app.fps = 0.0f;

    app.playerPosition3dMap = {.x = 850, .y = 200};
    app.playerAngle = PI/2.0;

    std::string map2dRepresentation[MAP_HEIGHT] = {
        "XXXXXXXXXX",
        "X........X",
        "X..XXXX..X",
        "X..XXX...X",
        "X..XXX...X",
        "X..XXXX..X",
        "X..XXXX..X",
        "X..XXXX..X",
        "X........X",
        "X....XXXXX",
        "XX....XXXX",
        "XXXXXXXXXX",
    };

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < map2dRepresentation[y].length(); x++){
            app.map2dRepresentation[y][x] = map2dRepresentation[y][x];
        }
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    if (TTF_Init() < 0)
    {
        printf("Couldn't initialize SDL_ttf: %s\n", TTF_GetError());
        exit(1);
    }

    font = TTF_OpenFont(
        "include/adobe-fonts/source-code-pro/TTF/SourceCodePro-Regular.ttf", 
        DEFAULT_FONT_PTSIZE
    );
    if (!font){
        printf("Unable to get font: %s\n", TTF_GetError());
        return;
    }

    app.window = SDL_CreateWindow("CGameSandbox", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, windowFlags
    );

    if (!app.window)
    {
        printf("Failed to open %d x %d window: %s\n", 
            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError()
        );
        exit(1);
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    app.renderer = SDL_CreateRenderer(app.window, -1, rendererFlags);

    if (!app.renderer)
    {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        exit(1);
    }
}