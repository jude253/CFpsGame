#include <SDL.h>
#include "init.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <iostream>
#include "defs.h"

void updatePlayerLocationForward(void) {
    double newX = app.playerPosition3dMap.x + cos(app.playerAngle)*PLAYER_MOVE_3D_STEP;
    double newY = app.playerPosition3dMap.y + sin(app.playerAngle)*PLAYER_MOVE_3D_STEP;
    app.playerPosition3dMap.x = newX;
    app.playerPosition3dMap.y = newY;
}

void updatePlayerLocationBackward(void) {
    double newX = app.playerPosition3dMap.x - cos(app.playerAngle)*PLAYER_MOVE_3D_STEP;
    double newY = app.playerPosition3dMap.y - sin(app.playerAngle)*PLAYER_MOVE_3D_STEP;
    app.playerPosition3dMap.x = newX;
    app.playerPosition3dMap.y = newY;
}

void doInput(void)
{
    SDL_GetMouseState(
        &app.mousePosition.x,
        &app.mousePosition.y
    );
    

    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                #ifdef __EMSCRIPTEN__
                emscripten_cancel_main_loop();  /* this should "kill" the app. */
                #else
                exit(0);
                #endif
                break;
            case SDL_MOUSEBUTTONDOWN:
                SDL_Log("Mouse position: x=%i y=%i",
                    app.mousePosition.x, app.mousePosition.y
                );
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                // ROTATION
                case SDLK_LEFT:
                        app.playerAngle -= PLAYER_ROTATE_3D_STEP;
                    break;
                case SDLK_RIGHT:
                        app.playerAngle += PLAYER_ROTATE_3D_STEP;
                    break;
                case SDLK_a:
                        app.playerAngle -= PLAYER_ROTATE_3D_STEP;
                    break;
                case SDLK_d:
                        app.playerAngle += PLAYER_ROTATE_3D_STEP;
                    break;

                // MOVEMENT
                case SDLK_UP:
                        updatePlayerLocationForward();
                    break;
                case SDLK_DOWN:
                        updatePlayerLocationBackward();
                    break;
                case SDLK_w:
                        updatePlayerLocationForward();
                    break;
                case SDLK_s:
                        updatePlayerLocationBackward();
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
        }
    }
}