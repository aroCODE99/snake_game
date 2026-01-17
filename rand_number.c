#include <stdio.h>
#include <stdlib.h>
#include <time.h>

float rand_float();

int main() {
    srand(time(0)); // seeding the rand() // i really don't know what does that mean
    int x = rand_float() * 100;
    int y = rand_float() * 100;
    printf("x is %d, y is %d \n", x, y);
    return 0;
}

float rand_float(void) {
    return (float) rand() / (float) RAND_MAX;
}

