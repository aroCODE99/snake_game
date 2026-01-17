#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define WIDTH 640
#define HEIGHT 480
#define RECT_WIDTH 50
#define RECT_HEIGHT 50

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Event event;
bool quit = false;

int main() {
    int rectX = 50;
    int rectY = 50;
    int vel_x = 1;
    int vel_y = 1;
    int screenWidth; int screenHeight;

    SDL_SetAppMetadata("Making the snake game from scratch with SDL", "1.0", "");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("making_something", WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // now this is the game loop
    while (!quit) {
        SDL_FRect fill_rect;
        fill_rect.x = rectX;
        fill_rect.y = rectY;
        fill_rect.w = RECT_WIDTH;
        fill_rect.h = RECT_HEIGHT;
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }

        // Rendering the Stuff
        SDL_SetRenderDrawColorFloat(renderer, 255, 0, 0, 0); // first fill the bg
        SDL_RenderClear(renderer); // this actually fills the color
        SDL_SetRenderDrawColorFloat(renderer, 255, 255, 0, 0); // first fill the bg
        SDL_RenderFillRect(renderer, &fill_rect);
        
        SDL_RenderPresent(renderer);
        SDL_GetWindowSize(window, &screenWidth, &screenHeight); // Getting the windows size
        
        if (rectY + fill_rect.h >= screenHeight || rectY <= 0) {
            vel_y *= -1;
        } else if (rectX + fill_rect.w >= screenWidth || rectX <= 0) {
            vel_x *= -1;
        }

        rectX += vel_x; rectY += vel_y;
    }
    return 0;
}
