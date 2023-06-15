#define PY_SSIZE_T_CLEAN

#include <Python.h>
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
double** kmeans(double** centroids,double** points, int k, int iter, double eps){
    int i,j,l;

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

double** fit(double** centroids, double** points, int k, int iter, double eps) {
    int i, j; 
    
    centroids = kmeans(centroids, points, k, iter, eps);

    for (i = 0; i < k; i++) {
        for (j = 0; j < dim-1; j++) {
            printf("%.4f,",centroids[i][j]);
        }
        printf("%.4f\n",centroids[i][dim-1]);
    }

    free_2d((void **)centroids, k);

    return centroids;
}

/* Function to convert Python object to double** */
double** convert_to_double_array(PyObject* obj) {
    Py_ssize_t i, j;
    Py_ssize_t rows = PyList_Size(obj);
    double** arr = (double**)malloc(rows * sizeof(double*));

    for (i = 0; i < rows; i++) {
        PyObject* row = PyList_GetItem(obj, i);
        Py_ssize_t cols = PyList_Size(row);
        arr[i] = (double*)malloc(cols * sizeof(double));

        for (j = 0; j < cols; j++) {
            PyObject* item = PyList_GetItem(row, j);
            if (!PyFloat_Check(item)) {
                /* Clean up and return NULL if item is not a float */
                for (int k = 0; k < i; k++) {
                    free(arr[k]);
                }
                free(arr);
                return NULL;
            }
            arr[i][j] = PyFloat_AsDouble(item);
        }
    }

    return arr;
}


/* Function to convert double** to Python object */
PyObject* convert_to_python_object(double** arr, Py_ssize_t rows, Py_ssize_t cols) {
    Py_ssize_t i, j;
    PyObject* obj = PyList_New(rows);

    for (i = 0; i < rows; i++) {
        PyObject* row = PyList_New(cols);

        for (j = 0; j < cols; j++) {
            PyObject* item = PyFloat_FromDouble(arr[i][j]);
            if (!item) {
                /* Clean up and return NULL if conversion fails */
                Py_DECREF(row);
                Py_DECREF(obj);
                return NULL;
            }
            PyList_SetItem(row, j, item);
        }

        PyList_SetItem(obj, i, row);
    }

    return obj;
}

static PyObject* fit_wrapper(PyObject *self, PyObject *args) {
    PyObject* centroids_obj;
    PyObject* points_obj;
    int k, iter;
    double eps;

    /* This parses the Python arguments into Python objects */
    if (!PyArg_ParseTuple(args, "OOiid", &centroids_obj, &points_obj, &k, &iter, &eps)) {
        return NULL;
    }

    /* Assign dimention and number of lines global variables */
    line_num = PyInt_AsInt(PyList_Size(points_obj));
    dim = line_num > 0? PyInt_AsInt(PyList_Size(PyList_GetItem(points_obj, 0))) : 0;

    /* Convert Python objects to double arrays */
    double** centroids = convert_to_double_array(centroids_obj);
    double** points = convert_to_double_array(points_obj);

    /* Call the fit function */
    double** result = fit(centroids, points, k, iter, eps);

    /* Convert the result back to a Python object */
    PyObject* result_obj = convert_to_python_object(result, k, dim);

    /* Free the allocated memory */
    free_2d((void**)centroids, k);
    free_2d((void**)points, line_num);
    free_2d((void**)result, k);

    return result_obj;
}

static PyMethodDef kmeansMethods[] = {
    /* {"kmeans", (PyCFunction) kmeans, METH_VARARGS, PyDoc_STR("Implementation of k-means clustering algorithem")}, */
    {
        "fit",
        fit_wrapper,
        METH_VARARGS, 
        PyDoc_STR("Implementation of k-means clustering algorithm")
    },
    /* 
     * {"point_array_copy", (PyFunction) point_array_copy, METH_VARARGS, PyDoc_STR("helper function for k-means clustering algorithem")},
     * {"empty_points_arr", (PyFunction) empty_points_arr, METH_VARARGS, PyDoc_STR("helper function for k-means clustering algorithem")},
     * {"distance", (PyFunction) distance, METH_VARARGS, PyDoc_STR("helper function for k-means clustering algorithem")},
     * {"pointer_check", (PyFunction) pointer_check, METH_VARARGS, PyDoc_STR("helper function for k-means clustering algorithem")},
     */
    {NULL, NULL, 0, NULL} 
};

static struct PyModuleDef mykmeanssp = {
    PyModuleDef_HEAD_INIT,
    "mykmeanssp", 
    "kmeans module", 
    -1,  
    kmeansMethods
};

PyMODINIT_FUNC PyInit_mykmeanssp(void){
    PyObject *m;
    
    m = PyModule_Create(&mykmeanssp);
    if (!m) {
        return NULL;
    }

    return m;
}
