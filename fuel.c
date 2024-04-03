#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "helpers.c"

#define SAVE_FILE_EXT 		"txt"
#define MAIN_MENU_N 		2 /* number of options for the main menu */
#define EXT_LENGTH 			4 // this is including the dot
#define TIMESTAMP_LENGTH 	11

const char *MAIN_MENU_OPTS[] = { 
	"Choose a specific vehicle", 
	"Add new vehicle" 
};
const char *CAR_MENU_OPTIONS[] = { 
	"Calculate and save fuel efficiency",
	"View car's fuel efficiency history",
	"Delete car",
	"Exit"
};

typedef struct record {
	char timestamp[TIMESTAMP_LENGTH + 1]; // date when it was recorded
	float kms; // kilometers travelled
	float liters; // liters consumed
	float kml; // kilometer per liter
};

/* functions for the menus */
int main_menu(char *carname);
int car_select_menu(char *carname);
int add_car_menu(char *carname);
int car_menu(char *carname);
void calculate_fuel_efficiency_menu(FILE *carfile);
void view_car_history_menu(FILE *carfile);
int delete_car_menu(FILE *file, char *carname, char *filename);

int main(int argc, char **argv)
{
	char carname[MAX_CAR_LENGTH + 1] = {'\0'};
	if (argc == 2)
		strcpy(carname, argv[1]);

	switch (argc) {
		case 1: 
			while(main_menu(carname));
		case 2:
			car_menu(carname); 
			break;
		default:
			printf( RED "Incorrect usage, too many arguments.\n" RESET);
			printf("Usage: ./fuel or ./fuel <car name>\n");
			return 1;
	}

	return 0;
}

/*
 * Main menu. Used for when user doesnt input a car.
 * Returns 0 when the user chooses a car.
*/
int main_menu(char *carname)
{
	system("cls");

	print_options(MAIN_MENU_OPTS, MAIN_MENU_N);
	int choice = prompt_user(MAIN_MENU_N);

	switch (choice) {
		case 1: 
			return car_select_menu(carname);
		case 2: 
			return add_car_menu(carname);
	}

	return 1;
}

/*
 * Car selection menu.
 * Returns 0 when the user has chosen a car. Returns 1 if the user
 * does not have a car.
*/
int car_select_menu(char *carname)
{
	/*
	 * Create the car array, and populate it
	 * crawls through the directory
	*/
	int n = 0;
	char **carsarray = malloc((MAX_CAR_LENGTH + 1) * MAX_CARS);
	for (int i = 0; i < MAX_CARS; i++)
		carsarray[i] = malloc(MAX_CAR_LENGTH + 1);

	HANDLE hndl;
    WIN32_FIND_DATA filedata;
    hndl = FindFirstFile(".\\cars\\*.*", &filedata);

    // if it failed
    if (hndl == INVALID_HANDLE_VALUE) {
        error("Can't find cars directory");
		exit(2);
    }

	char name[4] = {'a', 'b', 'c', '\0'};

    FindNextFile(hndl, &filedata);
    while (FindNextFile(hndl, &filedata)) {
        snprintf(carsarray[n], MAX_CAR_LENGTH + 1, "%s", filedata.cFileName);
        char *dot = strchr(carsarray[n], '.');
        *dot = '\0';

        n++;
    }
	
	// if the user does not have a car
	if (n == 0) {
		error("You do not have any cars saved yet! Add one first.");
		return 1;
	}

	/*
	 * Print out every car, prompt the user to choose one
	*/
	system("cls");

	int car_i;
	print_options(carsarray, n);
	car_i = prompt_user(n);
	strcpy(carname, carsarray[car_i - 1]);

	/*
	 * Freeing memory heehee 
	*/
	for (int i = 0; i < MAX_CARS; i++)
		free(carsarray[i]);
	free(carsarray);
	FindClose(hndl);

	return 0;
}

/*
 * Menu for adding cars.
 * Returns 0 when user adds a car, and wishes to use that car. 
 * Returns 1 if they choose not to.
*/
int add_car_menu(char *carname)
{
	system("cls");

	/* prompt user for the name of car, and prepare its filename */
	char newcar[MAX_CAR_LENGTH + 1];
	printf( YELLOW "Note: Name cannot be longer than 20 characters.\n" RESET );
	printf("Enter name of the car: ");
	scanf("%20s", newcar);

	char filename[7 + MAX_CAR_LENGTH + EXT_LENGTH + 1];
	snprintf(filename, 7 + MAX_CAR_LENGTH + EXT_LENGTH + 1, ".\\cars\\%s.%s", newcar, SAVE_FILE_EXT);

	/* Check if it already exists */
	if (_access(filename, 00) == 0) {
		error("This car already exists.");
		return 1;
	}

	/* Create its own save file */
	FILE *carfile;
	carfile = fopen(filename, "w");
	fclose(carfile);

	/* Ask the user if they want to go back to main menu, or go into their new car */
	printf( GREEN "\nCar added successfully!\n" RESET );
	printf("Do you wish to use this car's file? ");
	printf( YELLOW "Answering [N] will return you to the main menu.\n" RESET );

	if (prompt_yes_or_no()) {
		strcpy(carname, newcar);
		return 0;
	} else {
		return 1;
	}
}

int car_menu(char *carname)
{	
	/* Open the car's file */
	char filename[7 + MAX_CAR_LENGTH + EXT_LENGTH + 1];
	snprintf(filename, 7 + MAX_CAR_LENGTH + EXT_LENGTH + 1, "./cars/%s.%s", carname, SAVE_FILE_EXT);
	printf("%s", filename);

	FILE *carfile;
	carfile = fopen(filename, "a+");

	if (carfile == NULL) {
		error("Cant find / open the car's file");
		return 1;
	}

	/* menu block */
	int exited = 0;
	while (!exited) {
		/* clear screen, print the options and prompt the user */
		system("cls");
		print_options(CAR_MENU_OPTIONS, 4);

		int choice;
		choice = prompt_user(4);
		switch (choice) {
			case 1: 
				calculate_fuel_efficiency_menu(carfile);
				break;
			case 2: 
				view_car_history_menu(carfile);
				break;
			case 3: 
				exited = delete_car_menu(carfile, carname, filename);
				break;
			case 4: 
				exited = 1;
		}        
	}

	fclose(carfile);
	return 0;
}

void calculate_fuel_efficiency_menu(FILE *carfile)
{
	system("cls");

	/* ask user for their kms run and liters consumed and calculate km/l */
	float kilometers;
	float liters;
	float kml;
	kilometers = get_float_input("Enter the amount of Kilometers (km) you travelled: ");
	liters = get_float_input("Enter the amount of liters (l) you consumed: ");
	kml = kilometers / liters;

	/* get the date today, to timestamp the addition */
	time_t t = time(NULL);
	struct tm *localtm = localtime(&t);

	char timestamp[TIMESTAMP_LENGTH + 1];
	strftime(timestamp, TIMESTAMP_LENGTH + 1, "%b %d %Y", localtm);

	/* add new record to the car file */
	fprintf(carfile, "%s,%.2f,%.2f,%.2f\n", timestamp, kilometers, liters, kml);

	printf("\n");
	printf("Your fuel efficiency for %s is: %.2f km/L\n", timestamp, kml);
	printf("Press any key to continue... ");
	getch();
}

void view_car_history_menu(FILE *carfile)
{
	/*
	 * read the file and load its contents to an array of structs
	 * display every record
	*/
	fseek(carfile, 0, SEEK_SET);
	int array_len = 5;
	struct record *records = malloc(sizeof(struct record) * array_len);

	char buffer[50];
	int n = 0;
	while (fgets(buffer, 50, carfile) != NULL) {
		char *comma;

		strncpy(records[n].timestamp, buffer, TIMESTAMP_LENGTH);
		comma = strchr(buffer, ',');
		strcpy(buffer, comma + 1);
		
		char temp[50];
		comma = strchr(buffer, ',');
		*comma = '\0';
		sprintf(temp, "%s", buffer);
		records[n].kms = atof(temp);
		sprintf(buffer, comma + 1);

		comma = strchr(buffer, ',');
		*comma = '\0';
		sprintf(temp, "%s", buffer);
		records[n].liters = atof(temp);
		strcpy(buffer, comma + 1);
		
		buffer[strcspn(buffer, "\n")] = '\0';
		records[n].kml = atof(buffer);
		
		// check if i need to update the memory allocation for records
		if (++n == array_len) {
			array_len += 5;
			realloc(records, sizeof(struct record) * array_len);
		}
	}
	
	// display records
	system("cls");
	for (int i = 0; i < n; i++)
		printf("[%d] %s | %.2f km | %.2f L | %.2f km/L\n", i + 1, records[i].timestamp,
			records[i].kms, records[i].liters, records[i].kml);

	free(records);

	printf("\nPress any key to go back... ");
	getch();
}

/*
 * Asks the user to confirm if they want to delete the car.
 * 
 * Returns 1 if it succeeds in deleting the file.
 * Returns 0 if it encounters an error, or the user changes their mind.
*/
int delete_car_menu(FILE *file, char *carname, char *filename)
{
	system("cls");
	
	int yes;
	printf(RED "Are you sure you wish to delete this car? [%s]\n" RESET, carname);
	yes = prompt_yes_or_no();

	if (!yes)
		return 0;

	fclose(file);
	if(remove(filename) == 0) {
		FILE *carsfile;
		carsfile = fopen("cars.txt", "r");

		// get the size of cars file
		int size;
		fseek(carsfile, 0L, SEEK_END);
		size = ftell(carsfile);
		fseek(carsfile, 0L, SEEK_SET);

		// copy every car except the one to delete
		char *newlist = malloc(size);
		char buffer[MAX_CAR_LENGTH + 1];
		sprintf(newlist, ""); // set to empty string unless cthulu will respond
		strcat(carname, "\n"); // all the lines have a newline, so this would make it easier to compare
		while (fgets(buffer, MAX_CAR_LENGTH + 1 + 1, carsfile)) { // 1 + 1 here is \0 + \n
			if (strcmp(carname, buffer) != 0)
				strcat(newlist, buffer);
		}

		// copy the new list to the file
		carsfile = freopen("cars.txt", "w", carsfile);
		fprintf(carsfile, newlist);
		fclose(carsfile);

		printf(GREEN "\nCar successfully removed\n" RESET);
		return 1;		
	} else {
		error("Cannot delete the car's file.");
		return 0;
	}	
}
