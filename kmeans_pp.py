#%%
import sys # for getting CLI arguments
import os
import pandas as pd
import numpy as np
import mykmeanssp

ITER = 300

class Point:
    def __init__(self, coord, index):
        self.coord = coord
        self.index = index
        self.dimension = len(self.coord)

    @staticmethod
    def distance(p1, p2):
        #verify dimensions
        if p1.dimension != p2.dimension:
            print("An Error Has Occured")
            sys.exit()

        sum = 0
        for i in range(p1.dimension):
            sum += (p1.coord[i] - p2.coord[i])**2
        
        return sum**0.5
    
    @staticmethod
    def distance_to_closest_center(p, centers):   
        closest_dist = Point.distance(p, centers[0])

        for center in centers:
            dist = Point.distance(p, center)
            if dist < closest_dist:
                closest_dist = dist

        return closest_dist

def input_loader(filename):
    # load input file as list of strings
    try:
        with open(filename, 'r') as f:
            lines = f.readlines()
    except:
        print("An Error Has Occurred")
        sys.exit()
    return lines

def lines_to_points(lines):
    '''turns lines into list of points'''
    return [
        Point(
            list(float(num) for num in line.split(",")[1:]),
            line.split(",")[0]
        ) 
        for line in lines
    ]   

def process_files(filename1, filename2): 
    df1 = pd.read_csv(filename1, header=None)
    df2 = pd.read_csv(filename2, header=None)

    result = pd.merge(df1, df2, left_on=df1.columns[0], right_on=df2.columns[0], how='inner')
    result = result.sort_values(by=result.columns[0])
    result.to_csv('files_process_output.txt', index=False, header=False)

def choose_random_point(points): 
    np.random.seed(0)
    return np.random.choice(points)

def kmeans_pp(points, k):
    points_left = points
    random_point = choose_random_point(points)
    centers = [random_point]
    indexes = [str(int(float(random_point.index)))]
    
    points_left.remove(random_point)
    while (len(centers) < k):
        total_weight = 0

        for point in points:
            total_weight += Point.distance_to_closest_center(point, centers)

        weights = []

        for i in range(len(points)):
            weights.append(Point.distance_to_closest_center(points[i], centers))

        probabilities = [weight / total_weight for weight in weights]
        random_index = np.random.choice(range(len(points_left)), size=1, p=probabilities)[0]
        random_point = points_left[random_index]
        centers.append(random_point)
        indexes.append(str(int(float(random_point.index))))
        points_left.remove(random_point)

    print(','.join(indexes))
    return centers

def is_float(string):
    try:
        float(string)
        return True
    except ValueError:
        return False

def load_args(args):
    max_iter = ITER
    filename1 = ""
    filename2 = ""

    if args == None or len(args) < 5:
        print("An Error Has Occurred")
        sys.exit()

    if not str.isnumeric(args[1]):
        print("Invalid number of clusters!")
        sys.exit()

    if not str.isnumeric(args[2]):
        print("Invalid maximum iteration!")
        sys.exit()

    if not is_float(args[3]):
        print("An Error Has Occurred")
        sys.exit()

    K = int(args[1])
    max_iter = int(args[2])
    eps = float(args[3])
    filename1, filename2 = args[4], args[5]

    return K, max_iter, eps, filename1, filename2

def check_num_of_clusters(num_of_clusters, num_of_datapoints):
    if num_of_clusters <= 1 or num_of_clusters >= num_of_datapoints:
        print("Invalid number of clusters!")
        sys.exit()

def check_num_of_iter(num_of_iter):
    if num_of_iter <= 1 or num_of_iter >= 1000:
        print("Invalid maximum iteration!")  
        sys.exit()  
    return iter

def check_input_file(filename):
    name, ext = os.path.splitext(filename)
    if not ext.lower() in ['.txt', '.csv']:
        print("Invalid type of file")  
        sys.exit() 

def main(args = sys.argv):
    K, max_iter, eps, filename1, filename2 = load_args(args)
    check_input_file(filename1)
    check_input_file(filename2)
    check_num_of_iter(max_iter)

    process_files(filename1, filename2)

    points = lines_to_points(input_loader("files_process_output.txt"))
    points_coords = [point.coord for point in points]
    centers = kmeans_pp(points, K)
    centers_coords = [center.coord for center in centers]
    result = mykmeanssp.fit(centers_coords, points_coords, K, max_iter, eps)

    print(result)

if __name__=="__main__":
    main()
