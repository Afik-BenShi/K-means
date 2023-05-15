#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const double EPS = 0.001;
const int ITER = 200;
const char* GENERAL_ERROR = "An Error Has Occurred\n";
int dim = 0;
int line_num = 0;

/* ============ Array Tools ============ */

/*  frees 2d arrays. Array needs to be cast into void** */
void free_2d(void** mat, int rows){
    int i;
    for (i = 0; i < rows; i++){
        free(mat[i]);
    }

    free(mat);
}

/* Checks that an allocated pointer actually has somewhere to point to */
void pointer_check(void* ptr, const char* error_msg){
    if (ptr == NULL) {
        printf("%s", error_msg);
        exit(1);
    }

    return;
}

/* Creates an array of points that are all 0.
 * len - length of the array
 * Checks new pointers  */
double** empty_points_arr(int len){
    int i, j;
    double **arr = (double **)malloc(len * sizeof(double *));
    pointer_check((void *)arr, GENERAL_ERROR);

    for (i = 0; i < len; i++){
        arr[i] = (double *)malloc(dim * sizeof(double));
        pointer_check((void *)arr[i], GENERAL_ERROR);
        
        for (j = 0; j < dim; j++){
            arr[i][j] = 0.0;
        }
    }

    return arr;
}

/* Copies the first k points from one array to a new one.
 * Checks new pointers */
double** point_array_copy(double** points, int k){
    int i, j;
    double **new_points = (double **)malloc(k * sizeof(double *));
    pointer_check((void *)new_points, GENERAL_ERROR);

    for (i = 0; i < k; i++){
        new_points[i] = (double *)malloc(dim * sizeof(double));
        pointer_check((void *)new_points[i], GENERAL_ERROR);
        /* copy the values directly and not by pointing */
        for (j = 0; j < dim; j++){
            new_points[i][j] = points[i][j];
        }
    }

    return new_points;
}

/* ============ Global Variables Assignment ============*/

/* Counts the dimention of the points 
 * and updates global variable dim */
int get_dimention(char *st){
    int i;
    int dim_ = 0;
    for (i = 0; (st[i] != '\n') && (st[i] != '\0'); i++){
        /* count when seeing a comma */
        dim_ += (st[i] == ',');
    }

    /* add one if the first char is a number
     * this distinguishes if line has a number or is empty */
    dim_ += (st[0] <= '9' && st[0] >='0') || st[0] == '-'; 
    dim = dim_;

    return dim_;
}

/* Counts the number of lines in the string
 * and updates global variable lines_cnt */
int count_lines(char *st){
    int i;
    int line_cnt = 0;
    pointer_check((void *)st, "Error: null pointer passed to count_lines\n");
    
    for (i = 0; st[i] != '\0'; i++){
        line_cnt += (st[i] == '\n');
    }
    
    line_num = line_cnt;

    return line_cnt;
}

/* ============ File Read and String Manipulations ============*/

/* Reads the file and saves it to a string.
 * Checks new pointers */
char* read_file(){
    char *buffer;
    long size;
    FILE *file = stdin;
    pointer_check((void *)file, GENERAL_ERROR);
    fseek(file, 0L, SEEK_END); /* go to end of file*/
    size = ftell(file);   /* save the size*/
    fseek(file, 0L, SEEK_SET); /* go to start of file*/

    /* allocate string to hold the file data */
    buffer = (char*)malloc(size+1);
    pointer_check((void *)buffer, GENERAL_ERROR);

    /* read and save to buffer*/
    fread(buffer, size, 1, file);
    buffer[size] = '\0';

    fclose(file);

    return buffer;
}

/* Take the string and split it to lines array.
 * Warning: This replaces any '\n' with a '\0' in the original string,
 * as the data is not copied to a new location in memory.
 * Checks new pointers */
char** split_to_lines(char *st, int lines_count){
    char **lines_arr;
    int i, j;

    lines_arr = (char **) malloc(lines_count * sizeof(char *));
    pointer_check((void *)lines_arr, GENERAL_ERROR);

    lines_arr[0] = &st[0]; /* point to first line*/
    for (i = 0, j = 1; st[i] != '\0'; i++){
        if (st[i] == '\n'){
            st[i] = '\0'; /* split the string*/
            lines_arr[j] = &st[i+1]; /* point to start of next line*/
            j++;
        }
    }

    return lines_arr;
}

/* Takes a single line, splits it to numbers and returns a double array representing a point.
 * Warning: This splits the string while running and mends it back in the end.
 * Checks new pointers */
double* read_point(char *line, int dimention) {
    int i;
    double* point = (double *)malloc(dim * sizeof(double));
    pointer_check((void *)point, GENERAL_ERROR);

    for (i = 0; i < dimention; i++){
        int char_cnt;
        double number;
        char curr;
        /* count the number of chars until the next ',' or '\0' */
        for (char_cnt = 0;; char_cnt++){
            curr = line[char_cnt];
            if (curr == ',' || curr == '\0'){
                line[char_cnt] = '\0';       /* mark end of number */
                number = strtod(line, NULL); /* convert */
                line[char_cnt] = curr;       /* revert changes */
                break;
            }
        }
        if (curr != '\0'){
            line = &line[char_cnt + 1];
        }
        point[i] = number;
    }

    return point;
}

/* Takes an array of lines, splits them to numbers 
 * and returns an array of points from the lines.
 * Warning: This replaces any ',' with a '\0' in each of the lines.
 * as the data is not copied to a new location in memory.
 * Checks new pointers */
double** lines_to_points(char** lines, int line_num, int dim){
    int i;
    double **points = (double **)malloc(line_num * sizeof(double *));
    pointer_check((void *)points, GENERAL_ERROR);
    
    for (i = 0; i < line_num; i++){
        char *line = lines[i];
        points[i] = read_point(line, dim);
    }

    return points;
}

/* Euclidian distance between p and q.
 * Formula d=sqrt(pow(p[0]-q[0], 2) + ... + pow(p[n]-q[n], 2)) */
double distance(double* p, double* q, int dim){
    int i;
    double dist = 0.0;
    for (i = 0; i < dim; i++){
        dist += pow((p[i]-q[i]), 2);
    }

    return sqrt(dist);
}

/* Calculate k-means of a 2d array representing poinds */
double** kmeans(double** points, int k, int iter, double eps){
    int i,j,l;
    /* init centroids to first k points */
    double **centroids = point_array_copy(points, k);
    for (i = 0; i < iter; i++){
        double **new_cents = empty_points_arr(k);
                /* members saves what center assigned to each point.
                * point[i] is assigned to centroids[members[i]] */
        int *members = (int *)calloc(line_num, sizeof(int));
                /* mem_cnt counts the number of points assigned to a center */
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
            mem_cnt[center_idx] += 1; /* count member*/
        }

        /*step 4*/
        for (j = 0; j < line_num; j++){
            /* foreach point find its assigned index */
            int center_idx = members[j];
            for (l = 0; l < dim; l++){
                /* sum the point components normalized by member count (avg point) */
                new_cents[center_idx][l] += points[j][l]/mem_cnt[center_idx];
            }
        }

        /*step 5*/
        for (j = 0; j < k; j++){
            /* count unchanged distances */
            if (distance(new_cents[j], centroids[j], dim) < eps){
                converg_cnt++;
            }
            /* also, we won't need the old centers anymore */
            free(centroids[j]);
        }

        /* free all helping memory */
        free(centroids);
        free(members);
        free(mem_cnt);

        /* save new centers */
        centroids = new_cents;
        if (converg_cnt == k){
            /* convergence */
            break;
        }
    }

    return centroids;
}

/*  Checks that number of clusters is less than number of datapoints */
void check_num_of_clusters(int num_of_clusters, int num_of_datapoints) {
    if (num_of_clusters <= 1 || num_of_clusters >= num_of_datapoints) {
        printf("Error: Invalid number of clusters!\n");
        exit(1);
    }
}

/*  Checks that maximal number of iteration is within [1,1000] */
void check_num_of_iter(int num_of_iter){
    if (num_of_iter <= 1 || num_of_iter >= 1000) {
        printf("Error: Invalid maximum iteration!\n");
        exit(1);
    }
}

int parse_k(int argc, char *argv[]){
    int k;
    /* parse and assert imput */
    if (argc < 2){
        printf("%s", GENERAL_ERROR);
        return -1;
    }
    k = atoi(argv[1]);

    return k;
}

int parse_iter(int argc, char *argv[]){
    int iter = ITER;
    if (argc >= 3){
        iter = atoi(argv[2]);
    }

    return iter;
}

int main(int argc, char *argv[]){
    int i, j; 
    int k = parse_k(argc, argv);
    int iter = parse_iter(argc, argv);
    double eps = EPS;

    char *text = read_file();
    int dim_ = get_dimention(text);
    int line_num_ = count_lines(text);
    char **lines = split_to_lines(text, line_num_);
    double **points = lines_to_points(lines, line_num_, dim_);
    double **centroids;
    
    /* free text variable*/
    free(text);
    free(lines);

    /* check validity of k and iter */
    check_num_of_clusters(k, line_num);
    check_num_of_iter(iter);
    centroids = kmeans(points, k, iter, eps);

    for (i = 0; i < k; i++)
    {
        for (j = 0; j < dim-1; j++)
        {
            printf("%.4f,",centroids[i][j]);
        }
        printf("%.4f\n",centroids[i][dim-1]);
    }

    free_2d((void **)centroids, k);
    free_2d((void **)points, line_num);

    return 0;
}


