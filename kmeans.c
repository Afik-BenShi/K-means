#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int EPS = 0.001;
const int ITER = 200;
const char *GENERAL_ERROR = "An Error Has Occurred\n";

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
        printf(GENERAL_ERROR);
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

// reads the string and counts dimentions
int get_dimention(char *st){
    int dim = 0;
    for (int i = 0; (st[i] != '\n') && (st[i] != '\0'); i++){
        dim += (st[i] == ',');
    }
    // add one if the first char is a number
    dim += st[0] <= '9' && st[0] >='0'; 
    return dim;
}

// counts the number of lines in the string
int count_lines(char *st){
    int line_cnt = 0;
    for (int i = 0; st[i] != '\0'; i++){
        line_cnt += (st[i] == '\n');
    }
    return line_cnt;
}

// reads the file and returns a pointer to the string
char* read_file(FILE *file){
    char *buffer;
    long size;
    FILE *file;
    file = fopen("file.txt", "r");

    if (file == NULL) {
        printf(*GENERAL_ERROR);
        return 0;
    }

    fseek(file, OL, SEEK_END); // go to end of file
    size = ftell(file);   // save the size
    fseek(file, OL, SEEK_SET); // go to start of file

    // allocate memory
    buffer = (char*)malloc(size+1)
    if (*buffer == NULL){
        printf(*GENERAL_ERROR);
        return 0;
    }
    // read and save to buffer
    fread(buffer, size, 1, file);
    buffer[size] = '\0';

    fclose(file);
    return buffer;
}

// take the string and split it to lines array
char** split_to_lines(char *st, int line_cnt){
    char **lines_arr;
    lines_arr = calloc(lines_cnt, sizeof(char *));
    
    lines_arr[0] = &st[0]; // set to first line
    for (int i = 0, j = 1; st[i] != '\0'; i++){
        if (st[i] == '\n'){
            st[i] = '\0'; // split the string
            lines_arr[j] = &st[i+1]; // point to start of next line
            j++;
        }
    }
    return lines_arr;
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