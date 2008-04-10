#include "rumba/point.hpp"
#include <vector>
#include <algorithm>
#include <map>
#include <cassert>

enum merge_rule_t { RJ_MERGE , NN_MERGE  };

typedef RUMBA::Point<double> point_t;
typedef double (*distance_function_t)(const point_t&,const point_t&);
typedef std::vector<std::vector<int> > clusterlist_t ;

typedef std::vector<uint> dense_points_t;


double euclidean3(const RUMBA::Point<double>& p, const RUMBA::Point<double>&q);
void check_cluster(clusterlist_t & clusters);
clusterlist_t disjoint_union(clusterlist_t & L);
// void merge_clusters
// (const std::vector<int>& merged_clusters, clusterlist_t &  clusters);
void merge_clusters
(const std::vector<std::pair<int,double> > & merged_clusters,
 clusterlist_t &  clusters);


template <typename T>
void scanpoints(clusterlist_t & clusters,
        double R, const T& D, uint threshold,
        dense_points_t & dense_points );

template <typename T>
void new_dense_point(int index, clusterlist_t & clusters,
        double R, const T & D);





template <typename T>
double average_distance ( int index, unsigned int cluster_no,
        const clusterlist_t & clusters, const T& D)
{
    assert(cluster_no < clusters.size());
    const std::vector<int> & L = clusters[cluster_no];
    double sum = 0;
    for (unsigned int i = 0; i < L.size(); ++i)
        sum += D(L[i],index);
    return sum/L.size();
}

template <typename T>
std::pair<unsigned int,unsigned int> nearest_points(unsigned int cluster_no1, unsigned int cluster_no2,
        const clusterlist_t& clusters, const T& D)
{
    assert (cluster_no1 < clusters.size() && cluster_no2 < clusters.size());
    const std::vector<int> & L1 = clusters[cluster_no1];
    const std::vector<int> & L2 = clusters[cluster_no2];
    double current_min = D(L1[0],L2[0]);
    int current_i = 0;
    int current_j = 0;
    double dist = 0;

    for (unsigned int i = 0; i < L1.size(); ++i )
        for (unsigned int j = 0; j < L2.size(); ++j )
        {
            dist = D(L1[i],L2[j]);
            if (dist < current_min)
            {
                current_min = dist;
                current_i = i;
                current_j = j;
            }

        }

    return std::pair<unsigned int,unsigned int>(current_i, current_j);
}

template <typename T>
bool is_near_cluster(int index, const std::vector<int> & L, double R,
        const T & D)
{
    for (int i= 0; i < L.size(); ++i)
        if (D(index,L[i]) <=R)
            return 1;
    return 0;
}

template <typename T>
double nn_dist_to_cluster(int index, const std::vector<int> & L, double R,
        const T & D)
{
    double tmp, min=D(index,L[0]);
    for (unsigned int i= 1; i < L.size(); ++i)
    {
        tmp=D(index,L[i]);
	//std::cerr << tmp << std::endl;
        if (tmp < min)
            min = tmp;
    }
    return min;
}

bool mycomp(const std::pair<int,double>& left, const std::pair<int,double>& right) ;


// returns matching clusters
template <typename T>
std::vector<std::pair<int,double> > check_nearby_point(int index, const clusterlist_t& clusters,
        double R, const T& D, int* Count)
{
    int count = 0;
    double tmp;

    std::vector< std::pair<int,double> > matching_clusters;
    for (unsigned int i = 0; i < clusters.size(); ++i)
    {
        tmp=nn_dist_to_cluster(index, clusters[i],R,D);
	//std::cerr << "tmp,R" << tmp << ", " << R << std::endl;
        if (tmp<R)
        {
            ++count;
            matching_clusters.push_back(std::pair<int,double>(i,tmp));
        }
    }
    std::sort
	    (matching_clusters.begin(),matching_clusters.end(), mycomp);
    *Count = count;
    return matching_clusters;
}

template <typename T>
void new_dense_point(int index, clusterlist_t & clusters,
        double R, const T & D, bool merge_on_introduction)
{
    std::vector<std::pair<int,double> > matching_clusters;
    int count = 0;
    check_cluster(clusters);
    // 1. check that we're within R of some dense point.
    matching_clusters = check_nearby_point(index,clusters,R,D,&count);
    // if not, initiate a new cluster
    //std::cerr << "Count: " << count << std::endl;
    //std::cerr << "matching_clusters.size(): " << matching_clusters.size() << std::endl;
    if (!count)
    {
        std::vector<int> tmp;
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
void scanpoints(clusterlist_t & clusters,
        double R, const T& D, uint threshold,
        dense_points_t & dense_points,
        std::map<int, std::vector<int> > & dense_point_neighbours,
        bool merge_on_introduction, enum merge_rule_t merge_rule = NN_MERGE
        )
{
    std::vector<int> nearby_points;
//    enum merge_rule_t merge_rule = RJ_MERGE;
//    enum merge_rule_t merge_rule = NN_MERGE;
    for (unsigned int i = 0; i < dense_points.size(); ++i)
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

    clusterlist_t L = clusters;

    for (unsigned int i = 0; i < clusters.size(); ++i)
        for (unsigned int j = i+1; j < clusters.size(); ++j)
        {
            std::pair<unsigned int,unsigned int> p = nearest_points(i,j,clusters,D);
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
                std::vector<int> tmp;
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
