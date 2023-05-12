#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const double EPS = 0.001;
const int ITER = 200;
const char* GENERAL_ERROR = "An Error Has Occurred\n";

typedef struct {
    double *coords;
    int dimension;
    struct Point *next;
} Point;

Point *create_point(int dimension)
{
    Point *p = malloc(sizeof(Point));
    if (p == NULL) {
        printf("Error: Failed to allocate memory for Point\n");
        return NULL;
    }
    
    p->dimension = dimension;
    p->coords = malloc(dimension * sizeof(double));
    if (p->coords == NULL) {
        printf("Error: Failed to allocate memory for Point coordinates\n");
        free(p);
        return NULL;
    }
    
    // Initialize all coordinates to zero
    for (int i = 0; i < dimension; i++) {
        p->coords[i] = 0.0;
    }
    
    return p;
}

void destroy_point(Point *p)
{
    free(p->coords);
    p->dimension = 0;
}

typedef struct Cluster {
    Point *members;
    Point center;
} Cluster;

int read_point(char *line, Point *point) {
    int i = 0;
    int j = 0;
    char num_str[256];
    double num_val;

    while (line[j] != '\0' && i < point->dimension) {
        int k = 0;
        while (line[j] != ',' && line[j] != '\0') {
            num_str[k++] = line[j++];
        }
        num_str[k] = '\0';
        num_val = 0.0;
        int decimal_place = 0;
        int is_negative = 0;

        for (int l = 0; l < k; l++) {
            if (num_str[l] == '-') {
                is_negative = 1;
            } else if (num_str[l] == '.') {
                decimal_place = k - l - 1;
            } else {
                num_val = num_val * 10 + (num_str[l] - '0');
            }
        }

        if (is_negative) {
            num_val = -num_val;
        }

        num_val /= pow(10, decimal_place);
        point->coords[i++] = num_val;

        if (line[j] == ',') {
            j++;
        }
    }

    return (i == point->dimension);
}


// reads the string and counts dimentions
int get_dimention(char *st){
    int dim = 0;
    for (int i = 0; (st[i] != '\n') && (st[i] != '\0'); i++){
        dim += (st[i] == ',');
    }

    // add one if the first char is a number
    dim += st[0] <= '9' && st[0] >='0'; 
    return dim + 1;
}

// counts the number of lines in the string
int count_lines(char *st){
    if (st == NULL) {
        printf("Error: null pointer passed to count_lines\n");
        return 0;
    }
    
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

    if (file == NULL) {
        printf(GENERAL_ERROR);
        return NULL;
    }

    fseek(file, 0L, SEEK_END); // go to end of file
    size = ftell(file);   // save the size
    fseek(file, 0L, SEEK_SET); // go to start of file

    // allocate memory
    buffer = (char*)malloc(size+1);
    if (buffer == NULL){
        printf(GENERAL_ERROR);
        return NULL;
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
    lines_arr = calloc(line_cnt, sizeof(char *));
    
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

int distance(Point* p1,Point* p2){
        // verify dimentions.
        if (p1->dimension != p2->dimension) {
            printf("p1 dim = %d\n", p1->dimension);
            printf("p2 dim = %d\n", p2->dimension);

            printf(GENERAL_ERROR);
            return NULL;        
        }

        int sum = 0;
        for (int i = 0; i < p1->dimension; i++) {
            sum += (p1->coords[i] - p2->coords[i]) * (p1->coords[i] - p2->coords[i]);
        }
        
        return sqrt(sum);
}

int count_points(Point *p) {
    int count = 0;
    while (p != NULL) {
        count++;
        p = p->next;
    }
    return count;
}

// returns eclidean Distance, between the updated centroid to the previous one. 
double recalc_center(Cluster *cluster) {
    double coords[cluster->center.dimension];
    for (int i = 0; i < cluster->center.dimension; i++) {
        double sum = 0;
        for (Point *p = cluster->members; p != NULL; p = p->next) {
            sum += p->coords[i];
        }
        coords[i] = sum / count_points(cluster->members);
    }

    Point* new_center = create_point(cluster->center.dimension);
    for (int i = 0; i < cluster->center.dimension; i++) {
        new_center->coords[i] = coords[i];
    }

    double delta = distance(new_center, &(cluster->center));
    destroy_point(&(cluster->center));
    cluster->center = *new_center;
    free(new_center);

    return delta;
}

double add(Cluster *cluster, Point *p) {
    Point *new_member = malloc(sizeof(Point));
    new_member->coords = p->coords;
    new_member->dimension = p->dimension;

    // Add the new point to the beginning of the linked list
    new_member->next = cluster->members;
    cluster->members = new_member;

    return recalc_center(cluster);
}

void print_point(Point *p) {
    printf("(");
    for (int i = 0; i < p->dimension - 1; i++) {
        printf("%f, ", p->coords[i]);
    }
    printf("%f)", p->coords[p->dimension - 1]);
}

void print_clusters(Cluster* clusters, int num_clusters) {
    for (int i = 0; i < num_clusters; i++) {
        printf("Cluster %d:\n", i);
        print_point(&clusters[i]);
        printf("\n");
    }
}

void clear_members(Cluster *cluster) {
    Point *p = cluster->members;
    while (p != NULL) {
        Point *next = p->next;
        destroy_point(p);
        p = next;
    }
    cluster->members = NULL;
}

void check_num_of_clusters(int num_of_clusters, int num_of_datapoints) {
    if (num_of_clusters <= 1 || num_of_clusters >= num_of_datapoints) {
        printf("Error: Invalid number of clusters!\n");
    }
}

void check_num_of_iter(num_of_iter){
    if (num_of_iter <= 1 || num_of_iter >= 1000) {
        printf("Error: Invalid maximum iteration!\n");
    }
}

Cluster* kmeans(Point* points, int num_points, int K, int iter, double eps) {
    Cluster* clusters = (Cluster*) malloc(K * sizeof(Cluster));

    // initialize clusters with random points
    for (int i = 0; i < K; i++) {
        clusters[i].center = points[i];
        clusters[i].members = &points[i];
    }

    for (int i = 0; i < iter; i++) {
        int unchanged_clusters = 0;

        // assign points to nearest cluster
        for (int j = 0; j < num_points; j++) {
            Point p = points[j];
            Cluster* min_cluster = &clusters[0];
            double min_dist = distance(&p, &min_cluster->center);
                printf("in kmeans mindist dim = %d", min_cluster->center.dimension);


            for (int k = 1; k < K; k++) {
                double curr_dist = distance(&p, &clusters[k].center);
                if (curr_dist < min_dist) {
                    min_dist = curr_dist;
                    min_cluster = &clusters[k];
                }
            }

            if (add(min_cluster, &p) < eps) {
                unchanged_clusters++;
            }
        }

        if (unchanged_clusters == K) {
            break;
        }

        // reset clusters so there aren't points in multiple clusters
        for (int k = 0; k < K; k++) {
            clear_members(&clusters[k]);
        }
    }

    return clusters;
}

int main(int argc, char *argv[])
{
    // Parse command line arguments
    int k = atoi(argv[1]);
    int max_iter = atoi(argv[2]);
    check_num_of_iter(max_iter);

    char *line = NULL;
    size_t len = 0;
    size_t read;
    
    int lines_counter = 0;
    while ((read = getline(&line, &len, stdin)) != -1) {
        lines_counter++;
    }

    fseek(stdin, 0, SEEK_SET);

    Point **points = malloc(lines_counter * sizeof(Point*));

    // Read the first line to count the number of dimensions
    if ((read = getline(&line, &len, stdin)) != -1) {
        int num_of_dimensions = get_dimention(line);
        printf("Number of dimensions: %d\n", num_of_dimensions);

        // Create a point to store the values read from each line
        Point *point = create_point(num_of_dimensions);

        fseek(stdin, 0, SEEK_SET);

        int point_index = 0;

        // Process the remaining input lines
        while ((read = getline(&line, &len, stdin)) != -1) {
            Point *point = create_point(num_of_dimensions);
            if (read_point(line, point)) {
                points[point_index++] = point;
                printf("line = %s", line);
                // Do something with the point
                for (int i = 0; i < point->dimension; i++) {
                    printf("%f ", point->coords[i]);
                }
                printf("\n");
            } else {
                printf("Failed to read point\n");
                destroy_point(point);
            }
        }

        printf("points = %f\n", *points[1]);

        destroy_point(point);
    } else {
        printf("Failed to read first line\n");
    }

    free(line);

    check_num_of_clusters(k,lines_counter);
    Cluster * clusters = kmeans(points, lines_counter, k, max_iter, EPS);
    print_clusters(clusters, k);

    return 0;
}
