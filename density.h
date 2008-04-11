clusters_t cluster2
(
 points_t allpoints,
 distance_function_t f,
 uint threshold,
 double R_start,
 double R_end,
 double R_step,
 dense_points_t & dense_points,
 neighbors_t & dense_point_neighbours,
 double* ssbetween ,
 bool merge_on_introduction ,
 enum merge_rule_t merge_rule,
 std::vector<int> * cluster_sizes
 );


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
    uint minimal_cluster_size = 1
    );


void find_dense_points(
        points_t allpoints,
        double R, uint threshold,
        dense_points_t & dense_points,
        neighbors_t & dense_point_neighbours
        ) ;
