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

typedef std::map<bucket_t, std::set<uint> > bucketmap_t;
typedef std::map<bucket_t, std::set<uint> >::const_iterator
    bucketmap_const_iterator;



struct bucket_distance_function
{

 distance_function_t f;
 const bucketmap_t& buckets;
 const points_t & allpoints;
 bucket_distance_function
 (distance_function_t f, const bucketmap_t& buckets,
         const points_t & allpoints );

  double operator() (
         const point_t& p, const point_t & q
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
         const points_t & allpoints_ )
 : f(f_), buckets(buckets_), allpoints(allpoints_)
 {}



bucket_t getBucket ( const point_t & p, double R)
{
#if 0
// apparently it doesn't matter if we simply floor or round.
// Later on points get checked by their original coordinates
// yoh is lost where actually we thought that there is a bug...
    int x = static_cast<int> (round(p.x()/R));
    int y = static_cast<int> (round(p.y()/R));
    int z = static_cast<int> (round(p.z()/R));
#else
    int x = static_cast<int> (p.x()/R);
    int y = static_cast<int> (p.y()/R);
    int z = static_cast<int> (p.z()/R);
#endif
    return bucket_t (x,y,z,0);
}


bucketmap_t bucket (double R,
        const points_t& allpoints )
{
    // key = bin x,y,z, value = point number
    bucketmap_t result;

    bucket_t thebucket;
    std::set<uint> emptyset;

    for (uint i = 0; i < allpoints.size(); ++i)
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
indexes_t & nearby_points
  )
{
    point_t p = f.allpoints[i];
    bucket_t thebucket = getBucket ( p,R );
    VDOUT(106, "Called desnity i=" << i <<" R=" << R << "\n");

    int count = 0;
    for (int i = thebucket.x()-1; i <= thebucket.x()+1; ++i )
       for (int j = thebucket.y()-1;  j <= thebucket.y()+1; ++j)
           for (int k = thebucket.z()-1;  k <= thebucket.z()+1; ++k)
           {
                bucketmap_const_iterator tmp = f.buckets.find
                    (bucket_t(i,j,k,0) );
                if (tmp != f.buckets.end())
                {
                    for (std::set<uint>::const_iterator it =
                            tmp->second.begin();
                            it != tmp->second.end();
                            ++it)
                    {
                        assert (*it < f.allpoints.size() && *it >= 0);
                        double d = f(p, f.allpoints[*it]);
                        VDOUT(106, "Comparing " << p <<" and " << f.allpoints[*it]);

                        if (d <= R)
                        {
                            VDOUT(106, " + " << d << " < " << R << "\n");
                            ++count;
                            nearby_points.push_back(*it);
                        }
                        else
                        {
                            VDOUT(106, " - " << d << " > " << R << "\n");
                        }

                    }
                }
           }
    return count;
}

static double square (double x) { return x*x; }
double getSSBetween ( const clusters_t & clusters,
        const bucket_distance_function & f )
/* yoh: Get the sum of distances between closest points between each pair of clusters,
   sum is normalized by the total number of such pairs
   So it is average squared distance between closest points of any two clusters,
   so pretty much some obscured variance ;-)
 */
{
    std::pair<int,int> p;
    double SSbetween = 0;
    int N = clusters.size();
    for (uint i = 0; i < clusters.size(); ++i)
        for (uint j = 0; j < i; ++j)
        {
               p = nearest_points(i,j,clusters, f);
               SSbetween += square(f(clusters[i][p.first],clusters[j][p.second]));
        }
    return SSbetween/(N*(N-1)/2);
}

clusters_t cluster2
(
 points_t allpoints,
 distance_function_t f,
 uint threshold,
 double R_start,
 double R_end,
 double R_step,
 dense_points_t & dense_points,
 neighbors_t& dense_point_neighbours,
 double* ssbetween ,
 bool merge_on_introduction ,
 enum merge_rule_t merge_rule ,
 std::vector<int> * cluster_sizes = 0
 )
{
    clusters_t clusters;
    uint N = allpoints.size();
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
        vout << 5 << "R=" << R << " cluster-size=" << clusters.size() << "\n";
        R += R_step;
    } while (R < R_end && R_step > 0);

    for (uint i = 0; i < N; ++i)
        dense_points[i] = 0;

    bucketmap_t buckets = bucket(R_end, allpoints);
    bucket_distance_function D  ( euclidean3, buckets, allpoints);

    scanpoints(clusters,R_end,D, threshold,dense_points, dense_point_neighbours,
            merge_on_introduction,merge_rule);
    if (ssbetween)
        *ssbetween = getSSBetween(clusters,D);

    return clusters;
}

void filter_clusters(clusters_t &  clusters, uint minimal_cluster_size=1)
{
    if (minimal_cluster_size>1)
    {
        int nremoved=0;
        vout << 6 << "Removing clusters with # of elements <" << minimal_cluster_size << ": ";

        // to don't employ uncomprehendable trickeries of STL lets do it simple way
        for (clusters_t::iterator it = clusters.begin();
             it < clusters.end(); it++)
            if (it->size() < minimal_cluster_size)
            {
                vout << 6 << it->size() << ',';
                clusters.erase(it);
                nremoved++;
            }
        vout << 6 << "\n";
        vout << 6 << "Total removed: " << nremoved <<" clusters\n";
    }
}

/*
 Lobotomized cluster2 to do only its job - for given
 R and threshold compute clusters.
 It might be performance suboptimal to the sweep of cluster2
 over the range of values, but since we need also to compute pseudo-F
 to select 'best' and best set might be over different pairs of R,threshold
 yoh decided to neglect performance hit for now
 */
clusters_t cluster_plain
(
 points_t allpoints,
 distance_function_t f,
 uint threshold,
 double R,
 dense_points_t & dense_points,
 neighbors_t& dense_point_neighbours,
 double* ssbetween ,
 bool merge_on_introduction ,
 enum merge_rule_t merge_rule ,
 uint minimal_cluster_size = 1  // if >1 -- eliminate clusters with less than that number of voxels
 )
{
    clusters_t clusters;
    int N = allpoints.size();
    dense_points.resize(N);
    for (int i = 0; i < N; ++i)
        dense_points[i] = 0;

    bucketmap_t buckets = bucket(R, allpoints);
    bucket_distance_function D  ( euclidean3, buckets, allpoints);

    scanpoints(clusters, R, D, threshold, dense_points,
               dense_point_neighbours,
               merge_on_introduction, merge_rule);

    filter_clusters(clusters, minimal_cluster_size);

    if (ssbetween)
        *ssbetween = getSSBetween(clusters,D);

    return clusters;
}

/* identify dense points and their "electors" */
void find_dense_points(
        points_t allpoints,
        double R, uint threshold,
        dense_points_t & dense_points,
        neighbors_t & dense_point_neighbours
        )
{
    dense_points.resize(allpoints.size());
    indexes_t nearby_points;
    bucketmap_t buckets = bucket(R, allpoints);
    bucket_distance_function D  ( euclidean3, buckets, allpoints);

    for (uint i = 0; i < dense_points.size(); ++i)
        if (dense_points[i]<threshold)
        {
            dense_points[i] = density(i,R,D,nearby_points);
            VDOUT(106, "Point " << i << " density = " << dense_points[i] << "\n");
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
