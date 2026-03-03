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
#define SNAKE_LENGTH_CAP 4

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Event event;
int vel = 1;
bool quit = false;
bool play = true;
int grid_cols = WIDTH / GRID_SIZE;
int grid_rows = HEIGHT / GRID_SIZE;
int move_speed = 300; // in ms

// How do i use this
// i still don't know how to use this shit
// This is the Dynamic Array
typedef struct {
    SDL_FRect *body; // Array of the Rects
    size_t length;
    size_t capacity;
} Snake;

float rand_float(void) {
    return (float) rand() / (float) RAND_MAX;
}

int rand_int(int rand_max) {
    return rand() % rand_max;
}

void init_snake(Snake *s, size_t initial_capacity)
{
    s->body = malloc(initial_capacity * sizeof(SDL_FRect));
    s->capacity = initial_capacity;
    s->length = 3;

    // Initialize head position
    s->body[0].x = 4 * GRID_SIZE;
    s->body[0].y = 4 * GRID_SIZE;
    s->body[0].w = GRID_SIZE;
    s->body[0].h = GRID_SIZE;
}

void snake_append(Snake *s)
{
    if (s->length >= s->capacity) {
        s->capacity *= 2;
        s->body = realloc(s->body, s->capacity * sizeof(SDL_FRect));
    }

    // actually growing the snake
    s->body[s->length] = s->body[s->length - 1];
    s->length += 1;
}

bool food_collision(struct SDL_FRect *snake_head, struct SDL_FRect *food)
{
    if (snake_head->x < food->x + food->w &&
        snake_head->y < food->y + food->h &&
        snake_head->y + snake_head->h > food->y &&
        snake_head->x + snake_head->w > food->x) {
        return true;
    }
    return false;
}

bool snake_collision(Snake *snake)
{
    SDL_FRect head = snake->body[0];
    for (size_t i = 1; i < snake->length; ++i) {
        if (head.y == snake->body[i].y && head.x == snake->body[i].x) {
            return true;
        }
    }
    return false;
}

// TODO: Movement in opp direction should not be allowed
void snake_move(Snake *snake, enum DIRECTIONS dir)
{
    // This is the smartest logic that i have seen
    // when head moves, automatically the whole snake moves
    for (size_t i = snake->length - 1; i > 0; --i) {
        snake->body[i] = snake->body[i - 1];
    }
    
    // moving the snake_head
    if (dir == U) snake->body[0].y -= vel * GRID_SIZE;
    if (dir == D) snake->body[0].y += vel * GRID_SIZE;
    if (dir == L) snake->body[0].x -= vel * GRID_SIZE;
    if (dir == R) snake->body[0].x += vel * GRID_SIZE;
}

void free_snake(Snake *s) {
    free(s->body);
    s->body = NULL;
}

bool check_food_appearing_on_snake(Snake *s, SDL_FRect *f)  {
    for (size_t i = 0; i < s->length; ++i) {
        if (s->body[i].x == f->x && s->body[i].y == f->y) {
            return true;
        }
    }
    return false;
}

void move_food(Snake *s, SDL_FRect *f) {
    do {
        f->x = rand_int(grid_cols) * GRID_SIZE;
        f->y = rand_int(grid_rows) * GRID_SIZE;
    } while (check_food_appearing_on_snake(s, f));
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

    // now i need to do something here
    int screenWidth; int screenHeight;

    enum DIRECTIONS movein = 3; // moving right first
    
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("making_something", WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    
    SDL_FRect food_rect;
    food_rect.x = rand_int(grid_cols) * GRID_SIZE;
    food_rect.y = rand_int(grid_rows) * GRID_SIZE;
    food_rect.w = GRID_SIZE;
    food_rect.h = GRID_SIZE;
    
    // Initializing the Snake struct
    Snake snake;
    init_snake(&snake, SNAKE_LENGTH_CAP);
    assert(snake.body); // asserting the snake

    // Main gameloop
    while (!quit) {
        /* Uint64 start_time = SDL_GetPerformanceCounter(); */
        /* float game_time = SDL_GetTicks() / 1000.0f; */

        // Event Loop
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_KEY_UP) {
                if (event.key.key == SDLK_UP)    movein = U;
                if (event.key.key == SDLK_DOWN)  movein = D;
                if (event.key.key == SDLK_LEFT)  movein = L;
                if (event.key.key == SDLK_RIGHT) movein = R;
                if (event.key.key == SDLK_SPACE) play = !play;
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

        if ((int)SDL_GetTicks() - move_timer > move_speed && play) {
            snake_move(&snake, movein);
            if (snake_collision(&snake)) {
                quit = true;
            }
            move_timer = SDL_GetTicks();
        }
        
        // Getting bounding correct
        SDL_FRect *head = &snake.body[0];

        if (head->y < 0 || head->y + head->h > screenHeight ||
            head->x < 0 || head->x + head->w > screenWidth) {
            quit = true;
        }

        // TODO: Fix the food appearing on the snake
        if (food_collision(head, &food_rect)) {
            snake_append(&snake);
            move_food(&snake, &food_rect);
        }
        
        // Rendering the snake
        for (size_t i = 0; i < snake.length; ++i) {
            SDL_RenderFillRect(renderer, &snake.body[i]);
        }

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
    free_snake(&snake); // freeing the snake.bodyg
    return 0;
}
