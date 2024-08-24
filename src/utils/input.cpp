#include <SDL.h>
#include "init.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <iostream>
#include "defs.h"



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
                
                // MOVEMENT
                case SDLK_w:
                        app.playerPosition3dMap.y -= PLAYER_MOVE_3D_STEP;
                    break;
                case SDLK_a:
                        app.playerPosition3dMap.x -= PLAYER_MOVE_3D_STEP;
                    break;
                case SDLK_s:
                        app.playerPosition3dMap.y += PLAYER_MOVE_3D_STEP;
                    break;
                case SDLK_d:
                        app.playerPosition3dMap.x += PLAYER_MOVE_3D_STEP;
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