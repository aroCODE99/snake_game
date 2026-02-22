// KITNA TIME WASTE KAREGA RE

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
#define SNAKE_CAP 10

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Event event;
int vel = 1;
bool quit = false;
bool play = true;
int grid_cols = WIDTH / GRID_SIZE;
int grid_rows = HEIGHT / GRID_SIZE;
int move_speed = 200; // in ms

typedef struct {
    int x;
    int y;
} Rect;

// I think this should be the SDL_FRect
typedef struct {
    SDL_FRect *body; // this is the array
    size_t length;
    size_t capacity;
} Snake;

bool collided(struct SDL_FRect *snake_head, struct SDL_FRect *food)
{
    if (snake_head->x < food->x + food->w &&
        snake_head->y < food->y + food->h &&
        snake_head->y + snake_head->h > food->y &&
        snake_head->x + snake_head->w > food->x) {
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

// something is not working correctly
void snake_move(Snake *snake, enum DIRECTIONS dir)
{
    for (int i = snake->length - 1; i > 0; --i) {
        snake->body[i] = snake->body[i - 1];
    }

    // moving the snake_head
    if (dir == U) snake->body[0].y -= vel * GRID_SIZE;
    if (dir == D) snake->body[0].y += vel * GRID_SIZE;
    if (dir == L) snake->body[0].x -= vel * GRID_SIZE;
    if (dir == R) snake->body[0].x += vel * GRID_SIZE;
}

// TODO: Making the snake appear(Rendering multiple rects)
int main(int argc, char **argv)
{
    srand(time(0)); // seeding the random generator
    int move_timer = 0;

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

    // x: 4, y: 4
    Rect rect_cords = {
        .x = 4 * GRID_SIZE,
        .y = 4 * GRID_SIZE
    };

    Rect food_cords = {
        .x = rand_int(grid_cols) * GRID_SIZE,
        .y = rand_int(grid_cols) * GRID_SIZE
    };

    // now i need to do something here
    int screenWidth; int screenHeight;

    enum DIRECTIONS movein = 0;
    printf("%d\n", movein);
        
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("making_something", WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // initializing the snake
    Snake snake;
    snake.capacity = SNAKE_CAP;
    snake.length = 3;
    snake.body = (SDL_FRect *)malloc(sizeof(SDL_FRect) * SNAKE_CAP); 

    assert(snake.body); // asserting if snake got allocated or not

    snake.body[0].x = rect_cords.x;
    snake.body[0].y = rect_cords.y;
    snake.body[0].w = GRID_SIZE;
    snake.body[0].h = GRID_SIZE;

    // Main gameloop
    while (!quit) {
        Uint64 start_time = SDL_GetPerformanceCounter();
        /* float game_time = SDL_GetTicks() / 1000.0f; */
        
        SDL_FRect food_rect;
        food_rect.x = food_cords.x;
        food_rect.y = food_cords.y;
        food_rect.w = GRID_SIZE;
        food_rect.h = GRID_SIZE;

        // Event Loop
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_KEY_UP) {
                if (event.key.key == SDLK_UP)    movein = U;
                if (event.key.key == SDLK_DOWN)  movein = D;
                if (event.key.key == SDLK_LEFT)  movein = L;
                if (event.key.key == SDLK_RIGHT) movein = R;
            }
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }

        // For now we are focusing on the FIXED_WIDTH and FIXED_HEIGHT
        SDL_GetWindowSize(window, &screenWidth, &screenHeight); // Getting the windows size

        // Rendering the Stuff
        // filling the bg color
        SDL_SetRenderDrawColorFloat(renderer, 0, 0, 0, 0); // first fill the bg
        SDL_RenderClear(renderer); // this actually fillsX the
        SDL_SetRenderDrawColorFloat(renderer, 255, 255, 255, 255); // first fill the bg

        // TODO: improve this shit
        // I think this shit is true and i am totally assumming this shit is true
        if (SDL_GetTicks() - move_timer > move_speed && play) {
            snake_move(&snake, movein);
            move_timer = SDL_GetTicks();
        }

        /* sleep(1); // just adding some time in-between the frames */
        // Getting bounding correct
        SDL_FRect *head = &snake.body[0];

        if (head->y < 0 || head->y + head->h > screenHeight ||
            head->x < 0 || head->x + head->w > screenWidth) {
            quit = true;
        }
        
        if (collided(head, &food_rect)) {
            snake.length += 1;
            food_cords.x = rand_int(grid_cols) * GRID_SIZE;
            food_cords.y = rand_int(grid_rows) * GRID_SIZE;
        }

        // movement me hagg rha he ye mkc
        for (int i = 0; i < snake.length; i++) {
            SDL_RenderFillRect(renderer, &snake.body[i]);
        }
        // now draw the random food here
        SDL_SetRenderDrawColorFloat(renderer, 255, 0, 0, 0); // setting the color for the food
        SDL_RenderFillRect(renderer, &food_rect);

        SDL_RenderPresent(renderer);

        // after drawing do timing stuff
        /* Uint64 end_time = SDL_GetPerformanceCounter(); */
        // what is the frame delay?
        // frame_delay is the time that should be taken by the frame at 60fps
        // frame_delay is the target time per frame when running at 60 FPS :- ChatGpt
        /* float frame_delay = (float) (1000.0f / move_speed); */

        // frame_time is the time in ms taken by each frame in certain fps
        // SDL_GetPerformanceFrequency this usually converts the High Performance counter to time
        /* double frame_time = (end_time - start_time) / */
        /*     (double) SDL_GetPerformanceFrequency() * 1000.0f; */
    }

    // cleaning of the code
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return 0;
}
