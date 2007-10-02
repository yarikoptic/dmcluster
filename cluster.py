import RUMBA
import math
import random 
import sys

def printclusters(clusters):
    if len(clusters) > 1:
        for x in clusters:
            x.sort()
            print x
    else:
        print "one only"

def check_cluster(clusters):
    for L in clusters:
        for i in xrange(1,len(L)):
            assert ( L[i] != L[i-1] )


# need to split express several sets in terms of 
# disjoint union of a bunch of sets.



def union(x,y):
    L = []
    L = [i for i in x]
    for a in y:
        if a not in L:
            L.append(a)
    return L

def intersect(x,y):
    L =[]
    for i in x:
        if i in y:
            L.append(i)
    return L


def disjoint_union (L):

    result = []
    new_L = []

    while L: # iteratively chop off maximal chain 
        s = [ x for x in L[0] ] # start with first set
        flag = 1
        while flag: # keep going until no intersections found.
            flag = 0
            for t in L:
                n = intersect(s,t)
                if n:
                    s = union(s,t)
                    flag = 1 
                else:
                    new_L.append(t)
            L = new_L
            new_L = []
        result.append(s)
    return result

# print disjoint_union ( [  [99], [1,2] , [2,3], [4,5,6], [6,8], [7], [1,2,10], [10,12], [12,13], [4,14], [23,24,25], [1,25], [23,26], [7,27] ] )




def vector_norm(v):
    return reduce ( v, lambda x,y: x+y*y, 0 )
    
def euclicean_distance(p,q):
    diff = [ x[0]-x[1] for x in zip(p,q) ]
    return vector_norm(diff)

# count points within radius R
#
# p: a point
# R: radius
# D: distance matrix
def density(p,R,D):
    count = 0
    for q in xrange(len(D)):
        if D[p][q] < R:
            count += 1
    return count

# check if a point p is dense
def is_dense(p,R,D,threshold):
    return density(p,R,D) >= threshold


# a cluster is a list of indices within the list of points L.
# clusters is a list of clusters.

# return 1 if p is a member of a cluster, 0 otherwise
def is_cluster_member(p,clusters):
    for L in clusters:
        if p in L:
            return 1
    return 0

def is_near_cluster(p,L,R,D):
    for q in xrange(len(L)):
        if D[p][L[q]] <= R:
            return 1
    return 0

# is p close to any point in an existing cluster ?
# return value: 
#               N: p is within distance R of N clusters
#               1: p is within distance R of some cluster
#               0: p is within distance >R of any cluster
def check_nearby_point(p,clusters,R,D):
    count = 0
    matching_clusters = []
    for i in xrange (len(clusters)):
        if is_near_cluster(p,clusters[i],R,D):
            count += 1
            matching_clusters.append(i)
    return count,matching_clusters

# merged_clusters: list of numerical ids of clusters that need to be merged
def merge_clusters(merged_clusters,clusters):
    # precondition
    check_cluster(clusters)
    # the new list of clusters
    new_clusters = []

    # start with all the clusters that we're not merging
    for i in xrange(len(clusters)):
        if i not in merged_clusters:
            new_clusters.append(clusters[i])

    # now create the new merged cluster
    newclust = []
    
    for i in xrange(len(merged_clusters)):
        newclust += clusters[merged_clusters[i]]

    # add the merged cluster to the list
    new_clusters.append(newclust)

    # empty the list
    del clusters[:]

    # reassign clusters
    clusters += new_clusters 
    # postcondition
    check_cluster(clusters)
 
def new_dense_point(index,clusters,R,D):
    check_cluster(clusters)
    # 1: check if it's within R of some dense point
    count,matching_clusters = check_nearby_point(index,clusters,R,D)
    if not count:
        clusters.append ( [index] ) 
        check_cluster(clusters)
        return
    
    # 2: add p to first nearby cluster. 
    cluster_no = matching_clusters[0]
    clusters[cluster_no].append(index)
    check_cluster(clusters)

    # if there's only one such cluster, we're done.
    if count == 1:
        return

    # 3: otherwise have to merge lists
    assert (count > 1)
    merge_clusters(matching_clusters,clusters)

# average distance between a point and members of a cluster.
# the point is given as a global index.
def average_distance(index, cluster_no, clusters, D):
    L = clusters[cluster_no]

    sum = 0
    for i in xrange(len(L)):
        sum += D[L[i]][index]

    return sum / len(L)

# return indices corresponding to the nearest pair of points
# in two clusters
def nearest_points(cluster_no1,cluster_no2, clusters, D):
    L1 = clusters[cluster_no1]
    L2 = clusters[cluster_no2]
    current_min = D[L1[0]][L2[0]]
    current_i = 0
    current_j = 0
    for i in xrange(len(L1)):
        for j in xrange(len(L2)):
            dist = D[L1[i]][L2[j]]
            if dist < current_min:
                current_min = dist
                current_i = i
                current_j = j
    return current_i, current_j


# main step for each fixed value of R.
# optionally allow a separate distance matrix for selecting dense points
def scanpoints(clusters,R,D,threshold, D2 = None):
    if not D2:
        D2 = D
    for i in xrange(len(D)):
#        print 'clusters', clusters
        if not is_cluster_member(i,clusters):
            if is_dense(i,R,D2,threshold):
                # steps 1-3 of each pass: these deal with
                # introduction of a new dense point. The new_dense_point
                # deals with the options -- the point can initiate a new 
                # cluster, join an existing cluster, or bring together 
                # two or more existing clusters.
                check_cluster(clusters)
                new_dense_point(i,clusters,R,D)
                check_cluster(clusters)


    # step iv of algorithm

    L = [ x for x in clusters ]
    for i in xrange(len(clusters)):
        for j in xrange(i+1,len(clusters)):
            p1,p2 = nearest_points(i,j,clusters,D)
            # within-cluster difference
            d1 = average_distance( clusters[i][p1], i, clusters, D ) + \
                average_distance( clusters[j][p2], j, clusters, D )
            d1 *= 0.5
            # between cluster difference
            d2 = D[clusters[i][p1]][clusters[j][p2]]

            if d2 < d1:
                L.append ( [L[i][0],L[j][0] ]  )

    # remove overlap by chaining overlapping clusters together 
    L = disjoint_union(L)
    # element-wise assign L to clusters
    del clusters [:]
    clusters += L
    
def cluster(allpoints,f,threshold, R_start, R_end, R_step = 0, f2 = None):
    if not f2:
        f2 = f
    # first compute distance matrix
    clusters = []
    N = len(allpoints)
    if not R_step: 
        R_step = (R_end - R_start )/100
    D = [  [ 0 for i in range(N)] for i in range(N) ]
    D2 = None

    for i in xrange(N):
        for j in xrange(N):
            D[i][j] = f(allpoints[i],allpoints[j])

    if f2:
        D2 = [  [ 0 for i in range(N)] for i in range(N) ]
        for i in xrange(N):
            for j in xrange(N):
                D2[i][j] = f(allpoints[i],allpoints[j])


    R = R_start
    # gradually increase R and perform step each time.
    while R < R_end:
        scanpoints(clusters,R,D,threshold,D2)
        R+=R_step

    return clusters


# so maybe we should test this thing ... ?

def runtest():

    def randompoint ( mean1,mean2, sd1,sd2 ): 
        return random.normalvariate (mean1,sd1), random.normalvariate(mean2,sd2)

    def snow (lower1, lower2, upper1, upper2 ):
        return random.uniform(lower1,upper1), random.uniform(lower2,upper2)

    points = []

    points += [ randompoint ( 1,1, .1,.1 ) for x in range(40) ]

    # gaussian cluster around 2,1 with more dispersion
    points += [ randompoint ( 2,1, .2,.2 ) for x in range(40) ]

    # gaussian cluster around -2,-2 with sd 2,1
    points += [ randompoint ( -2,-2, 1,.1 ) for x in range(40) ]

    # gaussian cluster around 1,1
    points += [ randompoint ( 1,1, 1.1,1.1 ) for x in range(20) ]


    # snow 
    points += [ snow (-4,-4, 4,4)  for x in range(400) ]

    p = RUMBA.gnuplot.Gnuplot()
    p.execute ( "set terminal postscript" )
    p.execute ( "set output 'out.ps'" )
    p.execute ( "plot '-' with points" )
    p.data(points)

    # for p in points:
    #     print p[0], p[1]

    x = cluster(points,lambda x,y: math.sqrt((x[0]-y[0])**2 + (x[1]-y[1])**2),10, .01, 0.3, 0.01)

    printclusters(x)

    for i in x:
        print len(i)

def loadfile (name):
    lines = [ x.split() for x in open(name).readlines() ]
    data = [ [ float(x[0]), float(x[1]), float(x[2]) ] for x in lines ]
    return data

def d3 (x,y):
    return math.sqrt ( (x[0]-y[0])**2+(x[1]-y[1])**2 +(x[2]-y[2])**2 )


runtest()

# data = loadfile ( sys.argv[1] )


# clusters = cluster( data, d3, 30, 0, 9, 1 )

# for x in clusters:
#    print x

# print "number of clusters", len(clusters)
    

# vim: set ts=4 sw=4 expandtab:
