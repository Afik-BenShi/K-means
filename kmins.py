import sys # for getting CLI arguments
EPS = 0.001
ITER = 200

def input_loader(filename:str) -> list[str]:
    # load input file as list of strings
    with open(filename, 'r') as f:
        lines = f.readlines()
    return lines

def lines_to_points(lines:str) -> list[tuple[int]]:
    '''turns lines into list of points'''
    pass

class Point:
    def __init__(self, *coord) -> None:
        self.coord = coord
        self.dimention = len(coord)

    @staticmethod
    def distance(p1:"Point",p2:"Point") -> float:
        #verify dimentions
        pass

class Cluster:
    def __init__(self, p) -> None:
        self.center:Point = p
        self.members = [p]
    
    def add(self, p):
        pass
    
    def recalc_center()->float:
        '''returns eclidean Distance, between the updated centroid to the previous one'''
        pass
    
    def __repr__(self) -> str:
        return self.center.coord[1:-1].replace(" ", "")


def kmins(points:list[Point], K:int, iter:int=ITER, eps:int=EPS) -> list[Cluster]:
    clusters = [Cluster(points[i]) for i in range(K)]
    for i in range(iter):
        for p in points:
            min_cluster = clusters[0]
            min_dist = Point.distance(p, min_cluster.center)
            
            for cl in clusters:
                curr_dist = Point.distance(p, cl.center)
                if curr_dist < min_dist:
                    min_dist = curr_dist
                    min_cluster = cl
            
            min_cluster.add(p)
            if (min_cluster.recalc_center() < eps):
                return clusters
            
    return clusters
        


def print_clusters(clusters:list[Cluster]) -> None:
    for cl in clusters:
        print(cl)

def load_args(args):
    max_iter = ITER
    filename = ""
    assert str.isnumeric(args[0]), "Invalid number of clusters!"
    K = int(args[0])
    if args == None or len(args) < 2:
        raise Exception("an error has oocured")
    elif len(args) == 2:
        filename = args[1]
    else:
        assert str.isnumeric(args[1]), "Invalid maximum iteration!"
        max_iter = int(args[1])
        filename = args[2]
        check_num_of_iter(max_iter)

    return K, max_iter, filename


def main(args = sys.argv):
    K, max_iter, filename = load_args(args)

    try:
        points = lines_to_points(input_loader(filename))
    except:
        raise Exception("an error has oocured")
    
    check_num_of_clusters(K,len(points))

    clusters = kmins(points, K, max_iter)
    print_clusters(clusters)


def check_num_of_clusters(num_of_clusters, num_of_datapoints):
    if num_of_clusters <= 1 or num_of_clusters >= num_of_datapoints:
        raise Exception("Invalid number of clusters!")

def check_num_of_iter(num_of_iter):
    if 1 <= num_of_iter or num_of_iter >= 1000:
        raise Exception("Invalid maximum iteration!")    
    return iter

if __name__=="__main__":
    main()