import sys # for getting CLI arguments
EPS = 0.001
ITER = 200

def input_loader(filename:str) -> list[str]:
    # load input file as list of strings
    with open(filename, 'r') as f:
        lines = f.readlines()
    return lines

def lines_to_point(lines:str) -> list[tuple[int]]:
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
    
    def recalc_center():
        pass

def kmins(k:int, iter:int=ITER, eps:int=EPS) -> list[Cluster]:
    pass

def print_clusters(clusters:list[Cluster]) -> None:
    pass

def main(args = sys.argv):
    pass

if __name__=="__main__":
    main()