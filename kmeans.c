#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int EPS = 0.001;
int ITER = 200;

typedef struct 
{
    double *coords;
    int dimension;
} Point;

Point * create_point(int dimension)
{
    Point p;
    p.dimension = dimension;
    p.coords = malloc(dimension * sizeof(double));
    return p;
}

void destroy_point(Point *p)
{
    free(p->coords);
    p->dimension = 0;
}

struct cluster
{
    Point point;
    struct cluster *next;
};

int read_point(char *line, Point *point) {
    int i = 0;
    char *token = strtok(line, ",");
    while (token != NULL && i < point->dimension) {
        point->coords[i++] = atof(token);
        token = strtok(NULL, ",");
    }
    return (i == point->dimension);
}

int get_dimension_num(FILE* file) {
    FILE* file = fopen("file.txt", "r");
    if (file == NULL) {
        printf("Failed to open file\n");
        return 1;
    }

    int num_commas = 0;
    int c;
    while ((c = fgetc(file)) != EOF && c != '\n') {
        if (c == ',') {
            num_commas++;
        }
    }

    fclose(file);
    return num_commas + 1;
}

int main(int argc, char *argv[])
{
    // Point p = create_point(3);
    // p.coords[0] = 1.0;
    // p.coords[1] = 2.0;
    // p.coords[2] = 3.0;
    // destroy_point(&p);
    // return 0;

    int k = atoi(argv[1]);   
    int max_iter = atoi(argv[2]);

    char *line = NULL;
    size_t len = 0;
    size_t read;

    FILE* file = fopen("file.txt", "r");
    int num_of_deimensions = get_dimension_num(file);

    while ((read = getline(&line, &len, stdin)) != -1) {
        printf("Line: %s \n", line);
        Point *point = create_point(5);

        if (read_point(line, point)) {
            // Do something with the point
            for (int i = 0; i < point->dimension; i++) {
                printf("%f ", point->coords[i]);
            }
            printf("\n");
        } else {
            printf("Failed to read point\n");
        }
        destroy_point(point);

    }

    free(line);

    return 0;
}