#define RESET           "\033[0m"
#define RED             "\033[0;31m"
#define GREEN           "\033[0;32m"
#define YELLOW          "\033[0;33m"
#define BLUE            "\033[0;34m"
#define MAX_CAR_LENGTH 	20
#define MAX_CARS 	    20

void error(char *message);
void memory_error();
void print_options(const char *options[], int n);
int prompt_user(int n);
int prompt_yes_or_no(void);