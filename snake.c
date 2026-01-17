#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>

#define WIDTH 640
#define HEIGHT 480
#define MIN ((a, b) (((x) < (y)) ? (x) : (y))

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static int x = 50;
static int y = 50;
static int win_h, win_w;
double vel_x = 1.0;
double vel_y = 1.0;
double targetFps = 120;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    SDL_SetAppMetadata("Example Renderer Clear", "1.0", "com.example.renderer-clear");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("making_something", WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(renderer, 640, 480, SDL_LOGICAL_PRESENTATION_LETTERBOX); 
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* this function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }  
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    const double now = ((double)SDL_GetTicks()) / 1000.0;  /* convert from milliseconds to seconds. */
    double TicksPerFrame = 1000 / targetFps;
    double frameTime = now / TicksPerFrame;
    if (frameTime < TicksPerFrame) {
        SDL_Delay(TicksPerFrame - frameTime);
    }
        
    SDL_FRect fill_rect;
    fill_rect.x = x; // why not try to move this little bit
    fill_rect.y = y;
    fill_rect.w = 50;
    fill_rect.h = 50;

    /* choose the color for the frame we will draw. The sine wave trick makes it fade between colors smoothly. */
    SDL_SetRenderDrawColorFloat(renderer, 255, 0, 0, 0); // first fill the bg
    SDL_RenderClear(renderer); // this actually fills the color
    SDL_SetRenderDrawColorFloat(renderer, 255, 255, 0, 0); // setting the color for the rect
    SDL_RenderFillRect(renderer, &fill_rect);
    
    SDL_RenderPresent(renderer); // this actually renders on window
    SDL_GetWindowSize(window, &win_w, &win_h); // actually getting the current window size

    printf("%lf\n", vel_y);
    x += vel_x; y += vel_y; // updating the x, y
    if (x + fill_rect.h > HEIGHT) {
        vel_y = -vel_y; // now i don't why is this not making the vel_y negative
    }
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    /* SDL will clean up the window/renderer for us. */
}
