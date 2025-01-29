#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct maptype
{
	char **mapdata;
	int rows;
	int cols;
};

struct positiontype
{
   	int x;
	int y; 
};

struct cartype
{
    char symbol;
	struct positiontype position;
};

struct positiontype moverules[] = { {0,1}, {1,0}, {-1,0}, {0,-1} };

struct maptype* readmap()
{
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	// Open the file
	fp = fopen("map.txt", "r");
	if (fp == NULL) {
		perror("Error opening file");
		return NULL;
	}

	struct maptype* map = (struct maptype*)malloc(sizeof(struct maptype));

	// Read the first line to determine dimensions
	read = getline(&line, &len, fp);
	map->rows = 1;
	map->cols = strlen(line) - 1; // -1 to exclude newline

	// Allocate the 2D array
	map->mapdata = (char **)malloc(map->rows * sizeof(char *));
	for (int i = 0; i < map->rows; i++) {
		map->mapdata[i] = (char *)malloc(map->cols * sizeof(char));
	}

	// Populate the first row
	strncpy(map->mapdata[0], line, map->cols);

	// Read the remaining lines and populate the array
	while ((read = getline(&line, &len, fp)) != -1) {
		map->rows++;
		map->mapdata = (char **)realloc(map->mapdata, map->rows * sizeof(char *));
		map->mapdata[map->rows - 1] = (char *)malloc(map->cols * sizeof(char));
		strncpy(map->mapdata[map->rows - 1], line, map->cols);
	}


	free(line);
	fclose(fp);
	return map;
}

void freemap(struct maptype* map)
{
	// Free the allocated memory
	for (int i = 0; i < map->rows; i++) {
		free(map->mapdata[i]);
	}
	free(map->mapdata);
	free(map);
}

void showmap(struct maptype* map, struct cartype* car)
{
	// Now you can use the map array
	for (int i = 0; i < map->rows; i++) {
		for (int j = 0; j < map->cols; j++) {
			if (car->position.x == j && car->position.y == i)
				printf("C ");
			else
				printf("%c ", map->mapdata[i][j]);
		}
		printf("\n");
	}
}

/*
 * sensor return 0 if it can move
 * or none zero if there is an obstacle
 */

int sensor(struct maptype* mapinfo, int moveX, int moveY)
{
    if (moveX < 0 || moveY < 0)
        return -1; 
        
    return mapinfo->mapdata[moveY][moveX] == '.' ? 0 : -1;
}

void drive_car(struct maptype* mapinfo, struct cartype* car) {
	char **map = mapinfo->mapdata;
	int rows = mapinfo->rows;
	int cols = mapinfo->cols;

	while (car->position.x >= 0 && car->position.x < rows && car->position.y >= 0 && car->position.y < cols) {
		sleep(2);
		system("clear");
		showmap(mapinfo, car);
        for(int i =0; i < 4; i++)
        {
            struct positiontype move = moverules[i];
            int moveX = car->position.x + move.x; 
            int moveY =car->position.y + move.y; 

            if (sensor(mapinfo, moveX, moveY) == 0)
            {
                car->position.x = moveX;
                car->position.y = moveY;
                printf("Success (%d,%d)\n", moveX, moveY);
                break;
            }
        }
	}
}

int main() {
	struct maptype* map = readmap();
	struct cartype car;

	car.position.x = 1;
	car.position.y = 1;

	drive_car(map, &car);
	freemap(map);
	return 0;
}
