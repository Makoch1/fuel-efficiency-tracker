#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helpers.c"

#define BALLS "BALLS"

int main(void)
{
    char balls[3][5];
    char buf[4];

    strncpy(balls[0], "abcdefghijklmnopqrstuvwxyz", 4);

    printf("%s", balls[1]);
}