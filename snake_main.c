#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <time.h>

enum DIRECTIONS {
    U, // UP 
    D, // DOWN
    L, // LEFT
    R  // RIGHT
};

#define WIDTH 640
#define HEIGHT 480
#define RECT_WIDTH 25
#define RECT_HEIGHT 25
#define SHIFT_ARGS(argc, argv) (assert((argc) > 0), (argc)--, *(argv)++)
#define CONVERT_TO_FLOAT(num) (1.0f/(num/1000.0f))
#define CURR_VERSION "1.0" // TODO: get the version from somewhere 

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Event event;
int vel = 1;
bool quit = false;
bool play = true; // play/pause button
int desiredFps = 350;

float rand_float(); // getting the random numbers
bool collided(struct SDL_FRect *snake_rect, struct SDL_FRect *food_rect);

int main(int argc, char **argv) {
    srand(time(0)); // seeding the random generator

    // parsing the arguments
    while (argc > 0) {
        char *curr_arg = SHIFT_ARGS(argc, argv);
        if (curr_arg[0] == '-' && curr_arg[1] == 'f') {
            char *fps_arg = SHIFT_ARGS(argc, argv);
            desiredFps = atoi(fps_arg);
        }
        if (curr_arg[0] == '-' && curr_arg[1] == 'v') {
            printf("current version is %s\n", CURR_VERSION);
            return 0;
        }
   }

    int rectX = 100;
    int rectY = 100;
    int foodX = (rand_float() * WIDTH);
    int foodY = (rand_float() * HEIGHT);
    int screenWidth; int screenHeight;
    enum DIRECTIONS movein = 1;
    
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("making_something", WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Main gameloop
    while (!quit) {
        Uint64 start_time = SDL_GetPerformanceCounter();

        SDL_FRect snake_rect;
        snake_rect.x = rectX;
        snake_rect.y = rectY;
        snake_rect.w = RECT_WIDTH;
        snake_rect.h = RECT_HEIGHT;

        SDL_FRect food_rect;
        food_rect.x = foodX;
        food_rect.y = foodY;
        food_rect.w = RECT_WIDTH;
        food_rect.h = RECT_HEIGHT;

        // Event Loop
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_KEY_UP) { 
                if (event.key.key == SDLK_UP) movein = 0;
                if (event.key.key == SDLK_DOWN) movein = 1;
                if (event.key.key == SDLK_LEFT) movein = 2;
                if (event.key.key == SDLK_RIGHT) movein = 3;
                if (event.key.key == SDLK_SPACE) play = !play;
            }
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }
        SDL_GetWindowSize(window, &screenWidth, &screenHeight); // Getting the windows size
    
        // Rendering the Stuff
        SDL_SetRenderDrawColorFloat(renderer, 255, 0, 0, 0); // first fill the bg
        SDL_RenderClear(renderer); // this actually fillsX the color
        SDL_SetRenderDrawColorFloat(renderer, 255, 255, 0, 0); // first fill the bg
        SDL_RenderFillRect(renderer, &snake_rect);
        
        // TODO: improve this shit
        if (movein == U) {
            if (play) rectY -= vel;
        } else if (movein == D) {
            if (play) rectY += vel;
        } else if (movein == R) {
            if (play) rectX += vel;
        } else {
            if (play) rectX -= vel;
        }

        /* sleep(1); // just adding some time in-between the frames  */
        // now draw the random food here
        SDL_SetRenderDrawColorFloat(renderer, 0, 0, 0, 255); // setting the color for the food
        SDL_RenderFillRect(renderer, &food_rect);
        
        // Getting bounding correct
        if (rectY + snake_rect.h > screenHeight || rectY < 0) {
            SDL_Log("%lds: Touched the Boundary", start_time);
            quit = true;
        }

        if (rectX + snake_rect.w > screenWidth || rectX < 0) {
            SDL_Log("%lds: Touched the Boundary", start_time);
            quit = true;
        }

        if (collided(&snake_rect, &food_rect)) {
            foodX = rand_float() * screenWidth;
            foodY = rand_float() * screenHeight;
        }

        SDL_RenderPresent(renderer);
        
        Uint64 end_time = SDL_GetPerformanceCounter();
        // what is the frame delay?
        // frame_delay is the time that should be taken by the frame at 60fps
        // frame_delay is the target time per frame when running at 60 FPS :- ChatGpt
        float frame_delay = (float) (1000.0f / desiredFps);
        
        // frame_time is the time in ms taken by each frame
        double frame_time = (end_time - start_time) /
            (double) SDL_GetPerformanceFrequency() * 1000.0f; // SDL_GetPerformanceFrequency this usually converts the High Performance counter to time
        
        if (frame_time < frame_delay) SDL_Delay(frame_delay - frame_time); // actually capping the fps
    }
    
    // cleaning of the code
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return 0;
}
  
float rand_float(void) {
    return (float) rand() / (float) RAND_MAX;
}

bool collided(struct SDL_FRect *snake_rect, struct SDL_FRect *food_rect) {
    // but let's look at this like how is this working
    if (snake_rect->x < food_rect->x + food_rect->w &&
        snake_rect->y < food_rect->y + food_rect->h &&
        snake_rect->y + snake_rect->h > food_rect->y &&
        snake_rect->x + snake_rect->w > food_rect->x) {
        return true;
    }
    return false;
}
