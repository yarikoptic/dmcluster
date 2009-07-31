#include "rumba/point.hpp"
#include <sys/types.h>
#include <vector>
#include <set>
#include <algorithm>
#include <map>
#include <cassert>

enum merge_rule_t { RJ_MERGE , NN_MERGE  };

typedef RUMBA::Point<double> point_t;
typedef RUMBA::Point<int> bucket_t;

typedef std::vector<point_t> points_t;

typedef double (*distance_function_t)(const point_t&, const point_t&);
typedef std::vector<uint> indexes_t;
typedef std::set<uint> indexset_t;
typedef std::vector<indexes_t> clusters_t;

typedef std::pair<uint, double> indexdist_t;
typedef std::vector<indexdist_t> indexdistlist_t;

typedef std::map<uint, indexes_t> neighbors_t;

//typedef std::vector<uint> dense_points_t;
#define dense_points_t indexes_t
#define cluster_t indexes_t


double euclidean3(const point_t& p, const point_t&q);
void check_cluster(clusters_t & clusters);
clusters_t disjoint_union(clusters_t & L);
// void merge_clusters
// (const std::vector<int>& merged_clusters, clusters_t &  clusters);
void merge_clusters
(const indexdistlist_t & merged_clusters,
 clusters_t &  clusters);


template <typename T>
void scanpoints(clusters_t & clusters,
        double R, const T& D, uint threshold,
        dense_points_t & dense_points );

template <typename T>
void new_dense_point(uint index, clusters_t & clusters,
        double R, const T & D);





template <typename T>
double average_distance (uint index, uint cluster_no,
        const clusters_t & clusters, const T& D)
{
    assert(cluster_no < clusters.size());
    const cluster_t & L = clusters[cluster_no];
    double sum = 0;
    for (uint i = 0; i < L.size(); ++i)
        sum += D(L[i],index);
    return sum/L.size();
}

template <typename T>
std::pair<uint,uint> nearest_points(uint cluster_no1, uint cluster_no2,
        const clusters_t& clusters, const T& D)
{
    assert (cluster_no1 < clusters.size() && cluster_no2 < clusters.size());
    const cluster_t & L1 = clusters[cluster_no1];
    const cluster_t & L2 = clusters[cluster_no2];
    double current_min = D(L1[0],L2[0]);
    int current_i = 0;
    int current_j = 0;
    double dist = 0;

    for (uint i = 0; i < L1.size(); ++i )
        for (uint j = 0; j < L2.size(); ++j )
        {
            dist = D(L1[i],L2[j]);
            if (dist < current_min)
            {
                current_min = dist;
                current_i = i;
                current_j = j;
            }

        }

    return std::pair<uint,uint>(current_i, current_j);
}

template <typename T>
bool is_near_cluster(uint index, const cluster_t & L, double R,
        const T & D)
{
    for (uint i= 0; i < L.size(); ++i)
        if (D(index,L[i]) <=R)
            return 1;
    return 0;
}

template <typename T>
double nn_dist_to_cluster(uint index, const cluster_t & L, double R,
        const T & D)
{
    double tmp, min=D(index,L[0]);
    for (uint i= 1; i < L.size(); ++i)
    {
        tmp=D(index,L[i]);
	//std::cerr << tmp << std::endl;
        if (tmp < min)
            min = tmp;
    }
    return min;
}

bool mycomp(const indexdist_t& left, const indexdist_t& right) ;


// returns matching clusters
template <typename T>
indexdistlist_t check_nearby_point(uint index, const clusters_t& clusters,
        double R, const T& D, int* Count)
{
    int count = 0;
    double tmp;

    indexdistlist_t matching_clusters;
    for (uint i = 0; i < clusters.size(); ++i)
    {
        tmp=nn_dist_to_cluster(index, clusters[i],R,D);
	//std::cerr << "tmp,R" << tmp << ", " << R << std::endl;
        if (tmp<R)
        {
            ++count;
            matching_clusters.push_back(indexdist_t(i,tmp));
        }
    }
    std::sort
	    (matching_clusters.begin(),matching_clusters.end(), mycomp);
    *Count = count;
    return matching_clusters;
}

template <typename T>
void new_dense_point(uint index, clusters_t & clusters,
        double R, const T & D, bool merge_on_introduction)
{
    indexdistlist_t matching_clusters;
    int count = 0;
    check_cluster(clusters);
    // 1. check that we're within R of some dense point.
    matching_clusters = check_nearby_point(index,clusters,R,D,&count);
    // if not, initiate a new cluster
    //std::cerr << "Count: " << count << std::endl;
    //std::cerr << "matching_clusters.size(): " << matching_clusters.size() << std::endl;
    if (!count)
    {
        cluster_t tmp;
        tmp.push_back(index);
        clusters.push_back(tmp);
        return;
    }

    // 2. if so, append to a matching cluster
    int cluster_no = matching_clusters[0].first;
    clusters[cluster_no].push_back(index);
    check_cluster(clusters);

    // if only one such cluster exists, we're done!
    if (count==1)
        return;
    //else std::cerr <<"got one\n";

    // 3. otherwise we have to merge lists.
    if (merge_on_introduction)
    {
        assert(count>1);
        merge_clusters(matching_clusters,clusters);
    }
}



template <typename T>
void scanpoints(clusters_t & clusters,
        double R, const T& D, uint threshold,
        dense_points_t & dense_points,
        neighbors_t & dense_point_neighbours,
        bool merge_on_introduction, enum merge_rule_t merge_rule = NN_MERGE
        )
{
    indexes_t nearby_points;
//    enum merge_rule_t merge_rule = RJ_MERGE;
//    enum merge_rule_t merge_rule = NN_MERGE;
    for (uint i = 0; i < dense_points.size(); ++i)
        if (dense_points[i]<threshold)
        {
            dense_points[i] = density(i,R,D,nearby_points);
            if (dense_points[i] >= threshold)
            {
                check_cluster(clusters);
                new_dense_point(i,clusters,R,D, merge_on_introduction);
                dense_point_neighbours[i] = nearby_points;
                check_cluster(clusters);
            }
            nearby_points.clear();
        }

    clusters_t L = clusters;

    for (uint i = 0; i < clusters.size(); ++i)
        for (uint j = i+1; j < clusters.size(); ++j)
        {
            std::pair<uint,uint> p = nearest_points(i,j,clusters,D);
            double d1;
            if (merge_rule == RJ_MERGE)
                d1 =
                0.5*average_distance(clusters[i][p.first],i,clusters,D) +
                0.5*average_distance(clusters[j][p.second],j,clusters,D);
            else
                d1 = R;
            double d2 = D(clusters[i][p.first],clusters[j][p.second]);

            if (d2 < d1)
            {
                cluster_t tmp;
                tmp.push_back(L[i][0]);
                tmp.push_back(L[j][0]);
                L.push_back(tmp);
            }
        }
    check_cluster(clusters);
    L = disjoint_union(L);
    clusters.swap(L); // assign. We don't need L anymore
    check_cluster(clusters);
}




// vim: set ts=4 sw=4 expandtab:
// Local Variables:
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
