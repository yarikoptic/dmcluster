#include "cluster.h"
#include "verbose.h"

#include "rumba/point.hpp"
#include <set>
#include <map>
#include <iostream>
// #include <algorithm> // these are to use copy and ostream_iterator ...
// #include <iterator>

/*
 *        This code buckets points into 3 dimensions. Buckets are sized
 *        according to the expected size of each cluster. Hopefully, this
 *        substantially cuts down on the need to store large distance matrices
 *        and limits the job of the density count to computing distance of
 *        points in nearby buckets. Since the initial bucketing while
 *        intuitively cumbersome and ugly is only O(N), it is worthwhile in
 *        that it substantially reduces the scale of the O(N*N) search for
 *        dense points.
 */

typedef std::map<RUMBA::Point<int>, std::set<int> > bucketmap_t;
typedef std::map<RUMBA::Point<int>, std::set<int> >::const_iterator
    bucketmap_const_iterator;



struct bucket_distance_function
{

 distance_function_t f;
 const bucketmap_t& buckets;
 const std::vector<point_t > & allpoints;
 bucket_distance_function
 (distance_function_t f, const bucketmap_t& buckets,
         const std::vector< point_t > & allpoints );

  double operator() (
         const RUMBA::Point<double>& p, const RUMBA::Point<double> & q
         ) const
 {
     return f(p,q);
 }
  double operator() (int i, int j) const {
      return f(allpoints[i],allpoints[j]);
  }
};

bucket_distance_function::bucket_distance_function
(distance_function_t f_, const bucketmap_t& buckets_,
         const std::vector< RUMBA::Point<double> > & allpoints_ )
 : f(f_), buckets(buckets_), allpoints(allpoints_)
 {}



RUMBA::Point<int> getBucket ( const RUMBA::Point<double> & p, double R)
{
    int x = static_cast<int> (p.x()/R);
    int y = static_cast<int> (p.y()/R);
    int z = static_cast<int> (p.z()/R);

    return RUMBA::Point<int> (x,y,z,0);
}


bucketmap_t bucket (double R,
        const std::vector<RUMBA::Point<double> >& allpoints )
{
    // key = bin x,y,z, value = point number
    bucketmap_t result;

    RUMBA::Point<int> thebucket;
    std::set<int> emptyset;

    for (int i = 0; i < allpoints.size(); ++i)
    {
        thebucket = getBucket(allpoints[i], R);
        bucketmap_const_iterator tmp = result.find(thebucket);
        if (tmp == result.end())
            result.insert ( std::make_pair ( thebucket, emptyset));

        result[thebucket].insert (i);
    }
    return result;
}

/*
 * add int parameter to conform to interface
 */
int density
(
int i, double R,
const bucket_distance_function & f,
std::vector<int> & nearby_points
  )
{
    RUMBA::Point<double> p = f.allpoints[i];
    RUMBA::Point<int> thebucket = getBucket ( p,R );

    int count = 0;
    for (int i = thebucket.x()-1; i <= thebucket.x()+1; ++i )
       for (int j = thebucket.y()-1;  j <= thebucket.y()+1; ++j)
           for (int k = thebucket.z()-1;  k <= thebucket.z()+1; ++k)
           {
                bucketmap_const_iterator tmp = f.buckets.find
                    (RUMBA::Point<int> (i,j,k,0) );
                if (tmp != f.buckets.end())
                {
                    for (std::set<int>::const_iterator it =
                            tmp->second.begin();
                            it != tmp->second.end();
                            ++it)
                    {
                        assert (*it < f.allpoints.size() && *it >= 0);
                        if (f(p, f.allpoints[*it]) <= R)
                        {
                            ++count;
                            nearby_points.push_back(*it);
                        }
                    }
                }
           }
    return count;
}

static double square (double x) { return x*x; }
double getSSBetween ( const clusterlist_t & clusters,
        const bucket_distance_function & f )
{
    std::pair<int,int> p;
    double SSbetween = 0;
    int N = clusters.size();
    for (int i = 0; i < clusters.size(); ++i)
        for (int j = 0; j < i; ++j)
        {
               p = nearest_points(i,j,clusters, f);
               SSbetween += square(f(clusters[i][p.first],clusters[j][p.second]));
        }
    return SSbetween/(N*(N-1)/2);
}

clusterlist_t cluster2
(
 std::vector<point_t> allpoints,
 distance_function_t f,
 int threshold,
 double R_start,
 double R_end,
 double R_step,
 std::vector<int> & dense_points,
 std::map<int, std::vector<int> >& dense_point_neighbours,
 double* ssbetween ,
 bool merge_on_introduction ,
 enum merge_rule_t merge_rule ,
 std::vector<int> * cluster_sizes = 0
 )
{
    clusterlist_t clusters;
    int N = allpoints.size();
    dense_points.resize(N);
    if (R_step == 0) R_step = (R_end-R_start)/10;

    double R = R_start;


    do
    {
        bucketmap_t buckets = bucket(R, allpoints);
        bucket_distance_function D  ( euclidean3, buckets, allpoints);

        scanpoints(clusters,R,D,threshold,dense_points,dense_point_neighbours,merge_on_introduction, merge_rule);
        if (cluster_sizes)
            cluster_sizes->push_back ( clusters.size());
        R += R_step;
        vout << 5 << "R=" << R << " cluster-size=" << clusters.size() << "\n";
    } while (R < R_end && R_step > 0);

    for (int i = 0; i < N; ++i)
        dense_points[i] = 0;

    bucketmap_t buckets = bucket(R_end, allpoints);
    bucket_distance_function D  ( euclidean3, buckets, allpoints);

    scanpoints(clusters,R_end,D, threshold,dense_points, dense_point_neighbours,
            merge_on_introduction,merge_rule);
    if (ssbetween)
        *ssbetween = getSSBetween(clusters,D);

    return clusters;
}

/* identify dense points and their "electors" */
void find_dense_points(
        std::vector<point_t> allpoints,
        double R, int threshold,
        std::vector<int> & dense_points,
        std::map<int, std::vector<int> > & dense_point_neighbours
        )
{
    dense_points.resize(allpoints.size());
    std::vector<int> nearby_points;
    bucketmap_t buckets = bucket(R, allpoints);
    bucket_distance_function D  ( euclidean3, buckets, allpoints);

    for (int i = 0; i < dense_points.size(); ++i)
        if (dense_points[i]<threshold)
        {
            dense_points[i] = density(i,R,D,nearby_points);
            vout << 6 << "Point " << i << " density = " << dense_points[i] << "\n"; 
//            nearby_points takes up a lot of space unless we use it on a
//            per-cluster basis ... maybe we should keep a list of all nearby
//            points as opposed to one per-dense point.
//            if (dense_points[i] >= threshold)
//                dense_point_neighbours[i] = nearby_points;
        }

}


// vim: set ts=4 sw=4 expandtab:
// Local Variables:
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
