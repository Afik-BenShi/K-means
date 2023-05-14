#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const double EPS = 0.001;
const int ITER = 200;
const char* GENERAL_ERROR = "An Error Has Occurred\n";
int dim = 0;
int line_num = 0;


void free_2d_ch(char** mat, int rows){
    int i;
    for (i = 0; i < rows; i++){
        free(mat[i]);
    }
    free(mat);
}
void free_2d_dou(double** mat, int rows){
    int i;
    for (i = 0; i < rows; i++){
        free(mat[i]);
    }
    free(mat);
}

/* reads the string and counts dimentions*/
int get_dimention(char *st){
    int i;
    int dim_ = 0;
    for (i = 0; (st[i] != '\n') && (st[i] != '\0'); i++){
        dim_ += (st[i] == ',');
    }

    /* add one if the first char is a number*/
    dim_ += st[0] <= '9' && st[0] >='0'; 
    dim = dim_;
    return dim_;
}

/* counts the number of lines in the string*/
int count_lines(char *st){
    int i;
    int line_cnt = 0;
    if (st == NULL) {
        printf("Error: null pointer passed to count_lines\n");
        return 0;
    }
    
    for (i = 0; st[i] != '\0'; i++){
        line_cnt += (st[i] == '\n');
    }
    
    line_num = line_cnt;
    return line_cnt;
}

/* reads the file and returns a pointer to the string*/
char* read_file(){
    char *buffer;
    long size;
    FILE *file = stdin;
    if (file == NULL) {
        printf(GENERAL_ERROR);
        exit(-1);
    }


    fseek(file, 0L, SEEK_END); /* go to end of file*/
    size = ftell(file);   /* save the size*/
    fseek(file, 0L, SEEK_SET); /* go to start of file*/

    /* allocate memory*/
    buffer = (char*)malloc(size+1);
    if (buffer == NULL){
        printf(GENERAL_ERROR);
        exit(-1);
    }
    /* read and save to buffer*/
    fread(buffer, size, 1, file);
    buffer[size] = '\0';

    fclose(file);

    return buffer;
}

/* take the string and split it to lines array*/
char** split_to_lines(char *st, int line_cnt){
    char **lines_arr;
    int i, j;
    lines_arr = (char **) malloc(line_cnt * sizeof(char *));
    if (lines_arr == NULL){
        printf(GENERAL_ERROR);
        return lines_arr;
    }
    lines_arr[0] = &st[0]; /* set to first line*/
    for (i = 0, j = 1; st[i] != '\0'; i++){
        if (st[i] == '\n'){
            st[i] = '\0'; /* split the string*/
            lines_arr[j] = &st[i+1]; /* point to start of next line*/
            j++;
        }
    }
    
    return lines_arr;
}

int read_point(char *line, double *point, int dimention) {
    int i;
    for (i = 0; i < dimention; i++){
        int char_cnt;
        char* num;

        for (char_cnt = 0; line[char_cnt] != '\0'; char_cnt++){
            if (line[char_cnt] == ','){
                line[char_cnt] = '\0';
                break;
            }
        }
        num = (char *)malloc(char_cnt * sizeof(char));
        if (num == NULL){
            printf(GENERAL_ERROR);
            exit(-1);
        }
        num = line;
        line = &line[char_cnt + 1];
        point[i] = strtod(num, NULL);
    }
    return i;
}

double** line_to_points(char** lines, int line_num, int dim){
    int i;
    double **points = (double **)malloc(line_num * sizeof(double *));
    if (points == NULL){
        printf(GENERAL_ERROR);
        exit(-1);
    }
    for (i = 0; i < line_num; i++){
        char *line = lines[i];
        points[i] = (double *)malloc(dim * sizeof(double));
        if (points[i] == NULL){
            printf(GENERAL_ERROR);
            exit(-1);
        }
        if (read_point(line, points[i], dim) == -1){
            printf(GENERAL_ERROR);
            exit(-1);
        }
    }
    return points;
}
double** empty_points_arr(int dim, int len){
    int i, j;
    double **arr = (double **)malloc(len * sizeof(double *));
    if (arr == NULL){
        printf(GENERAL_ERROR);
        exit(-1);
    }

    for (i = 0; i < len; i++){
        arr[i] = (double *)malloc(dim * sizeof(double));
        if (arr[i] == NULL){
            printf(GENERAL_ERROR);
            exit(-1);
        }
        for (j = 0; j < dim; j++){
            arr[i][j] = 0.0;
        }
    }
    return arr;
}

double** centroids_init(double** points, int k, int dim){
    int i, j;
    double **centroids = (double **)malloc(k * sizeof(double *));
    if (centroids == NULL){
        printf(GENERAL_ERROR);
        exit(-1);
    }

    for (i = 0; i < k; i++){
        centroids[i] = (double *)malloc(dim * sizeof(double));
        if (centroids[i] == NULL){
            printf(GENERAL_ERROR);
            exit(-1);
        }
        for (j = 0; j < dim; j++){
            centroids[i][j] = points[i][j];
        }
    }
    return centroids;
}

double distance(double* p, double* q, int dim){
    int i;
    double dist = 0.0;
    for (i = 0; i < dim; i++)
    {
        dist += pow((p[i]-q[i]), 2);
    }
    return sqrt(dist);
}



double** kmeans(double** points, int k, int iter, double eps){
    int i,j,l;
    double **centroids = centroids_init(points, k, dim);
    for (i = 0; i < iter; i++){
        double **new_cents = empty_points_arr(dim, k);
        int *members = (int *)calloc(line_num, sizeof(int));
        int *mem_cnt = (int *)calloc(k, sizeof(int));
        int converg_cnt = 0;
        /* foreach point*/
        for (j = 0; j < line_num; j++){
            int center_idx = 0;
            double min_dist = -1;
            /* find closest cluster*/
            for (l = 0; l < k; l++){
                double dist = distance(points[j], centroids[l], dim);
                if ((min_dist == -1)||(min_dist > dist)){
                    center_idx = l;
                    min_dist = dist;
                }
            }
            members[j] = center_idx;  /* point j is member of center_idx*/
            mem_cnt[center_idx] += 1; /* center_idx has one more member*/
        }
        /*step 4*/
        for (j = 0; j < line_num; j++){
            int center_idx = members[j];
            for (l = 0; l < dim; l++){
                new_cents[center_idx][l] += points[j][l]/mem_cnt[center_idx];
            }
        }
        /*step 5*/
        for (j = 0; j < k; j++){
            if (distance(new_cents[j], centroids[j], dim) < eps){
                converg_cnt++;
            }
            free(centroids[j]);
        }
        free(centroids);
        centroids = new_cents;
        free(members);
        free(mem_cnt);
        if (converg_cnt == k){
            break;
        }
    }
    return centroids;
}

void check_num_of_clusters(int num_of_clusters, int num_of_datapoints) {
    if (num_of_clusters <= 1 || num_of_clusters >= num_of_datapoints) {
        printf("Error: Invalid number of clusters!\n");
        exit(-1);
    }
}

void check_num_of_iter(int num_of_iter){
    if (num_of_iter <= 1 || num_of_iter >= 1000) {
        printf("Error: Invalid maximum iteration!\n");
        exit(-1);
    }
}


int main(int argc, char *argv[]){
    int i, j;
    /* Parse command line arguments*/
    int k = atoi(argv[1]);
    int iter = atoi(argv[2]);
    double eps = EPS;

    char *text = read_file();
    int dim_ = get_dimention(text);
    int line_num_ = count_lines(text);
    char **lines = split_to_lines(text, line_num_);
    double **points = line_to_points(lines, line_num_, dim_);
    double **centroids;
    if (text == NULL||points == NULL||lines == NULL){
            printf(GENERAL_ERROR);
            return -1;
    }
    /* free text variable*/
    free(text);
    /* assert input*/
    if (argc < 3){
        iter = ITER;
    }
    check_num_of_iter(iter);
    check_num_of_clusters(k, line_num);
    centroids = kmeans(points, k, iter, eps);
    for (i = 0; i < k; i++)
    {
        for (j = 0; j < dim-1; j++)
        {
            printf("%.4f,",centroids[i][j]);
        }
        printf("%.4f\n",centroids[i][dim-1]);
    }
    free_2d_dou(centroids, k);
    free_2d_dou(points, line_num);
    return 200;
}


