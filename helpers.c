#include <windows.h>
#include <stdio.h>
#include "helpers.h"

void error(char *message)
{
    printf( RED "\n%s\n" RESET, message);
    printf("Press any key to continue... ");
    getch();
}

int get_user_cars(char **cararray, int *ncars, int arraysize)
{
    HANDLE hndl;
    WIN32_FIND_DATA filedata;
    hndl = FindFirstFile(".\\cars\\*.*", &filedata);

    int *test = &hndl;

    // if it failed
    if (hndl == INVALID_HANDLE_VALUE) {
        return 0;
    }

    FindNextFile(hndl, &filedata);
    int i = 0;
    while (FindNextFile(hndl, &filedata)) {
        strcpy(cararray[i], filedata.cFileName);

        char *dot = strchr(cararray[i], '.');
        *dot = '\0';

        i++;
        if (i == arraysize) {
            arraysize += MAX_CARS;
            cararray = realloc(cararray, (MAX_CAR_LENGTH + 1) * arraysize);
        }
    }

    *ncars = i;
    return 1;
}

/* Informs user of memory error and exits program */
void memory_error()
{
    printf("Memory error encountered. Exiting...");
    exit(1);
}

/*
    Loops through an array and prints them out as options.
    Ex:
        [1] Option 1
        [2] Option 2
    
    char *options[] == options array
    int n == number of options
*/
void print_options(const char *options[], int n)
{
    for (int i = 0; i < n; i++)
    {
        printf("[%d] %s\n", i + 1, options[i]);
    }
}

/*
    Asks user for their choice, and returns as int.
    Checks for erroneous input and asks the user repeatedly
    until given an answer within 0 - n.
*/
int prompt_user(int n)
{
    int choice;
    do
    {
        printf("Pick an option: ");
        scanf(" %d", &choice);

        if (choice <= 0 || choice > n)
            printf( RED "\nInvalid option, out of bounds.\n" RESET);
    } while (choice <= 0 || choice > n);

    return choice;
}

/*
    Asks the user yes or no. Returns 0 for no, and 1 for yes.
    Checks for erroneous input.
*/
int prompt_yes_or_no(void)
{
    char choice;
    do
    {
        printf("Pick an option, [Y] or [N]: ");
        scanf(" %c", &choice);
        
        if (choice == 'Y' || choice == 'y')
            return 1;
        else if (choice == 'N' || choice == 'n')
            return 0;   
        else
            printf( RED "\nInvalid option, choose [Y] or [N] only.\n" RESET);
    } while (choice != 'n' || choice != 'N' || choice != 'y' || choice != 'Y');
}

float get_float_input(char *message)
{
    float input;

    do
    {
        printf("%s", message);
        scanf("%f", &input);

        if (input <= 0)
            printf( RED "\nInvalid input, cannot be negative.\n" RESET );
    } while (input <= 0);

    return input;
}