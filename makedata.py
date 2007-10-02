
def runtest(outfile = ""):

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

    if outfile:
        p = RUMBA.gnuplot.Gnuplot()
        p.execute ( "set terminal postscript" )
        p.execute ( "set output '" + outfile + "'" )
        p.execute ( "plot '-' with points" )
        p.data(points)

    # for p in points:
    #     print p[0], p[1]

    return points

import RUMBA
import sys 
import random

points = None
if len(sys.argv) > 1:
    points = runtest(sys.argv[1])
else:
    points = runtest()

for x in points:
    print x[0], x[1], 0

# vim: set ts=4 sw=4 expandtab:
