#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <time.h>
#include <math.h>

enum DIRECTIONS {
    U, // UP
    D, // DOWN
    L, // LEFT
    R  // RIGHT
};

#define SHIFT_ARGS(argc, argv) (assert((argc) > 0), (argc)--, *(argv)++)
#define CONVERT_TO_FLOAT(num) (1.0f/(num/1000.0f))
#define WIDTH 800
#define HEIGHT 600
#define GRID_SIZE 40
#define CURR_VERSION "1.0" // TODO: get the version from somewhere

typedef struct {
    int x;
    int y;
} Rect_Cords;

typedef struct {
    Rect_Cords cords;
    size_t count;
    size_t capacity;
} Vector;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Event event;
int vel = 1;
bool quit = false;
bool play = true;
int grid_cols = WIDTH / GRID_SIZE;
int grid_rows = HEIGHT / GRID_SIZE;
int move_speed = 150; // in ms

bool collided(struct SDL_FRect *snake_rect, struct SDL_FRect *food_rect) {
    if (snake_rect->x < food_rect->x + food_rect->w &&
        snake_rect->y < food_rect->y + food_rect->h &&
        snake_rect->y + snake_rect->h > food_rect->y &&
        snake_rect->x + snake_rect->w > food_rect->x) {
        return true;
    }
    return false;
}

float rand_float(void) {
    return (float) rand() / (float) RAND_MAX;
}

int rand_int(int rand_max) {
    return rand() % rand_max;
}

// TODO: Making the snake appear(Rendering multiple rects)
int main(int argc, char **argv)
{
    int move_timer = 0; // what is this doing
    srand(time(0)); // seeding the random generator

    // parsing the arguments
    while (argc > 1) {
        char *curr_arg = SHIFT_ARGS(argc, argv);
        if (curr_arg[0] == '-' && curr_arg[1] == 's') {
            char *fps_arg = SHIFT_ARGS(argc, argv);
            move_speed = atoi(fps_arg);
        } else if (curr_arg[0] == '-' && curr_arg[1] == 'v') {
            printf("current version is %s\n", CURR_VERSION);
            return 0;
        }
    }

    Rect_Cords rect_cords = {
        .x = 4 * GRID_SIZE,
        .y = 4 * GRID_SIZE
    };

    Rect_Cords food_cords = {
        .x = rand_int(grid_cols) * GRID_SIZE,
        .y = rand_int(grid_cols) * GRID_SIZE
    };
    
    // now i need to do something here
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
        float game_time = SDL_GetTicks() / 1000.0f;

        SDL_FRect snake_rect;
        snake_rect.x = rect_cords.x;
        snake_rect.y = rect_cords.y;
        snake_rect.w = GRID_SIZE;
        snake_rect.h = GRID_SIZE;

        SDL_FRect food_rect;
        food_rect.x = food_cords.x;
        food_rect.y = food_cords.y;
        food_rect.w = GRID_SIZE;
        food_rect.h = GRID_SIZE;

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

        // For now we are focusing on the FIXED_WIDTH and FIXED_HEIGHT
        SDL_GetWindowSize(window, &screenWidth, &screenHeight); // Getting the windows size

        // Rendering the Stuff
        SDL_SetRenderDrawColorFloat(renderer, 0, 0, 0, 0); // first fill the bg
        SDL_RenderClear(renderer); // this actually fillsX the color
        SDL_SetRenderDrawColorFloat(renderer, 255, 255, 255, 255); // first fill the bg
        SDL_RenderFillRect(renderer, &snake_rect);
        
        // TODO: improve this shit
        // I think this shit is true and i am totally assumming this shit is true
        if (SDL_GetTicks() - move_timer > move_speed) {
            if (movein == U) {
                if (play) rect_cords.y -= vel * GRID_SIZE;
            } else if (movein == D) {
                if (play) rect_cords.y += vel * GRID_SIZE;
            } else if (movein == R) {
                if (play) rect_cords.x += vel * GRID_SIZE;
            } else {
                if (play) rect_cords.x -= vel * GRID_SIZE;      
            }

            move_timer = SDL_GetTicks();
        }

        /* sleep(1); // just adding some time in-between the frames */
        // now draw the random food here
        SDL_SetRenderDrawColorFloat(renderer, 255, 0, 0, 0); // setting the color for the food
        SDL_RenderFillRect(renderer, &food_rect);
        // Getting bounding correct
        if (rect_cords.y + snake_rect.h > screenHeight || rect_cords.y < 0) {
            SDL_Log("%lfs: Touched the Boundary", (start_time / SDL_GetPerformanceFrequency()) / 1000.0f);
            quit = true;
        }

        if (rect_cords.x + snake_rect.w > screenWidth || rect_cords.x < 0) {
            SDL_Log("%lfs: Touched the Boundary", (start_time / SDL_GetPerformanceFrequency()) / 1000.0f);
            quit = true;
        }

        if (collided(&snake_rect, &food_rect)) {
            food_cords.x = rand_int(grid_cols) * GRID_SIZE;
            food_cords.y = rand_int(grid_rows) * GRID_SIZE;
        }

        SDL_RenderPresent(renderer);

        // after drawing do timing stuff
        Uint64 end_time = SDL_GetPerformanceCounter();
        // what is the frame delay?
        // frame_delay is the time that should be taken by the frame at 60fps
        // frame_delay is the target time per frame when running at 60 FPS :- ChatGpt
        float frame_delay = (float) (1000.0f / move_speed);

        // frame_time is the time in ms taken by each frame in certain fps
        // SDL_GetPerformanceFrequency this usually converts the High Performance counter to time
        double frame_time = (end_time - start_time) /
            (double) SDL_GetPerformanceFrequency() * 1000.0f;
    }

    // cleaning of the code
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return 0;
}
