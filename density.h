clusterlist_t cluster2
( 
 std::vector<point_t> allpoints, 
 distance_function_t f, 
 int threshold, 
 double R_start, 
 double R_end, 
 double R_step, 
 std::vector<int> & dense_points,
 std::map<int, std::vector<int> > & dense_point_neighbours,
 double* ssbetween ,
 bool merge_on_introduction ,
 enum merge_rule_t merge_rule,
 std::vector<int> * cluster_sizes 
 );


clusterlist_t cluster_plain
(
    std::vector<point_t> allpoints,
    distance_function_t f,
    int threshold,
    double R,
    std::vector<int> & dense_points,
    std::map<int, std::vector<int> >& dense_point_neighbours,
    double* ssbetween ,
    bool merge_on_introduction ,
    enum merge_rule_t merge_rule ,
    int minimal_cluster_size = 1
    );


void find_dense_points(
        std::vector<point_t> allpoints,
        double R, int threshold, 
        std::vector<int> & dense_points,
        std::map<int, std::vector<int> > & dense_point_neighbours
        ) ;
