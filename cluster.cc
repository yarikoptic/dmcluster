
#include <fstream>
#include <sstream>
#include <string>

#include "cluster.h"
#include "density.h"
#include "rumba/arghandler.h"

#include <algorithm>
#include <cmath>
#include <set>

#include <nifti1_io.h>

std::string version = "0.0.1";

inline double square(double x) { return x*x; }


bool mycomp(const std::pair<int,double>& left, const std::pair<int,double>& right)
{
    return left.second < right.second;
}

std::ostream& printpoint(const RUMBA::Point<double> & p, std::ostream& out)
{
    out << p.x() << " " << p.y() << " " << p.z() << " " << p.t();
    return out;
}

int countmembers ( std::vector<int> dense_points, int threshold)
{
    int result = 0;
    for (uint i = 0; i < dense_points.size(); ++i)
    {
        if (dense_points[i] >= threshold)
            ++result;

    }
    return result;
}

double variance
(
 const std::set<int>& cluster, const std::vector<RUMBA::Point<double> > & allpoints,
 RUMBA::Point<double> * mean = 0
 )
{
    const int N = cluster.size();
    double varX=0,varY=0,varZ=0;
    double sumX = 0, sumY = 0, sumZ = 0;
    double sumXX = 0, sumYY = 0, sumZZ = 0;
    RUMBA::Point<double> p;

    for (std::set<int>::const_iterator i = cluster.begin();
            i !=cluster.end(); ++i)
    {
        p = allpoints[*i];
        sumXX += p.x()*p.x();
        sumYY += p.y()*p.y();
        sumZZ += p.z()*p.z();
        sumX += p.x();
        sumY += p.y();
        sumZ += p.z();
    }
    varX =(sumXX/N - (sumX/N)*(sumX/N));
    varY =(sumYY/N - (sumY/N)*(sumY/N));
    varZ =(sumZZ/N - (sumZ/N)*(sumZ/N));

    if (mean)
    {
        mean->x() = sumX/N;
        mean->y() = sumY/N;
        mean->z() = sumZ/N;

    }

    return varX + varY + varZ;
}

double variance
(
 const std::vector<RUMBA::Point<double> > & points
 )
{
    const int N = points.size();
    double varX=0,varY=0,varZ=0;
    double sumX = 0, sumY = 0, sumZ = 0;
    double sumXX = 0, sumYY = 0, sumZZ = 0;
    RUMBA::Point<double> p;

    for (int i = 0; i < N; ++i)
    {
        p = points[i];
        sumXX += p.x()*p.x();
        sumYY += p.y()*p.y();
        sumZZ += p.z()*p.z();
        sumX += p.x();
        sumY += p.y();
        sumZ += p.z();
    }
    varX =(sumXX/N - (sumX/N)*(sumX/N));
    varY =(sumYY/N - (sumY/N)*(sumY/N));
    varZ =(sumZZ/N - (sumZ/N)*(sumZ/N));

    return varX + varY + varZ;
}

std::set<int> cluster_and_neighbours
( const std::vector<int> & cluster,
  const std::map<int, std::vector<int> >& neighbours )
{
    std::set<int> result;
    std::map<int, std::vector<int> >::const_iterator it;
    for (uint i = 0; i < cluster.size(); ++i )
    {
        it = neighbours.find(cluster[i]); // it had better be there!
        assert (it!=neighbours.end());
        for (uint j = 0; j < it->second.size(); ++j)
            result.insert ( it->second[j] );
    }
    return result;
}

double getVarwithin( const clusterlist_t& clusters, const std::vector<RUMBA::Point<double> > & allpoints, const std::map<int, std::vector<int> >& dense_point_neighbours)
{
    double result = 0;
    RUMBA::Point<double> m;
    for (uint i = 0; i < clusters.size(); ++i )
    {
        if (clusters[i].size() < 2)
            continue;
        std::set<int> thelist =
            cluster_and_neighbours(clusters[i],dense_point_neighbours);
        result += variance (thelist,allpoints,&m); //variance within
    }
    return result/clusters.size();
}


double dovariance ( const clusterlist_t& clusters, const std::vector<RUMBA::Point<double> > & allpoints, const std::map<int, std::vector<int> >& dense_point_neighbours)
{
    RUMBA::Point<double> m;
    std::vector<RUMBA::Point<double> > cluster_means;
    double varwithin = 0;
    double Sxx = 0;
    double Syy = 0;
    double Szz = 0;
    double Sx = 0;
    double Sy = 0;
    double Sz = 0;
    int N = 0;

    for (uint i = 0; i < clusters.size(); ++i )
    {
        if (clusters[i].size() < 2)
            continue;
        std::set<int> thelist =
            cluster_and_neighbours(clusters[i],dense_point_neighbours);
        varwithin += variance (thelist,allpoints,&m); //variance within
        cluster_means.push_back(m);
        for (uint j = 0; j < clusters[i].size(); ++j ) {
            RUMBA::Point<double> p = allpoints[clusters[i][j]];
            Sxx += square(p.x());
            Syy += square(p.y());
            Szz += square(p.z());
            Sx += p.x();
            Sy += p.y();
            Sz += p.z();
            ++N;
        }
    }

    double vartotal = Sxx/N + Syy/N + Szz/N -
        (square(Sx/N)+square(Sy/N)+square(Sz/N));

//     std::cerr << variance(cluster_means) +varwithin << " " << vartotal << std::endl;

//     return (variance(cluster_means) + varwithin) * clusters.size() / varwithin;
    return vartotal / (varwithin/clusters.size());
}



double density_ratio (const std::vector<int> & dense_points, int threshold)
{
    // density computation
    double density_within = 0;
    double density_total = 0;
    int dense_count = 0;
    for (uint i = 0; i < dense_points.size(); ++i)
    {
        density_total += dense_points[i];
        if (dense_points[i] >= threshold)
        {
            density_within += dense_points[i];
            dense_count++;
        }
    }
    if (dense_count)
        density_within /= dense_count; // otherwise density_within == 0

    density_total /= dense_points.size();

    return density_within/density_total;
}


// we make each cluster a vector<point_t>. Seems reasonable -- we often append
// to clusters and never delete elements from them. Merging clusters is a pain,
// but the python version of the algorithm already addresses the annoyances
// with this.

// so clusters is vector<vector<int> >


void printclusters ( const clusterlist_t&  clusters,
        std::ostream& out=std::cout)
{
    out << "--- begin printclusters ---" << std::endl;
    out << "--- there are " << clusters.size() << " clusters ---" << std::endl;
    for (uint i = 0; i < clusters.size(); ++i )
    {
        out << "[";
        for (uint j = 0; j < clusters[i].size(); ++j)
        {
            out << clusters[i][j] << ", ";
        }
        out << "]" << std::endl;
    }
    out << "--- end printclusters ---" << std::endl;
}


point_t  readpoint(const std::string& s)
{
    std::istringstream strin(s);
    point_t p;
    strin >> p.x();
    strin >> p.y();
    strin >> p.z();
    strin >> p.t();
    return p;
}

// return a list of points, allpoints
std::vector<point_t> loadfile(std::istream & in)
{
    std::istringstream strin;
    std::string s;
    std::vector<std::string> lines;
    std::vector<point_t> result;

    while (std::getline(in,s))
        if (s.length()>100)
            throw RUMBA::Exception (
                "Input line is too big (longer than 100 chars). It must be a binary file");
        lines.push_back(s);

    for (uint i = 0; i < lines.size(); ++i)
        result.push_back(readpoint(lines[i]));

    return result;
}

// not very efficient because we use linear searches
std::vector<int> make_union(const std::vector<int>& x,
        const std::vector<int>& y)
{
    std::vector<int> result = x;

    for (uint i = 0; i < y.size(); ++i)
        if ( std::find (result.begin(),result.end(),y[i])==result.end() )
            result.push_back(y[i]);

    return result;
}

// not very efficient because we use linear searches
// this function will be more efficient if the shorter argument is passed first
std::vector<int>
intersect(const std::vector<int>& x, const std::vector<int> & y)
{
    std::vector<int> result;
    for (uint i = 0; i < y.size(); ++i)
        if ( std::find (x.begin(),x.end(),y[i])!= x.end() )
            result.push_back(y[i]);
    return result;
}



clusterlist_t disjoint_union(clusterlist_t & L)
{
    int flag = 1;
    clusterlist_t result;
    clusterlist_t new_L;
    std::vector<int> s;

    std::vector<int> n;

    while (L.size())
    {
        s = L[0];
        flag = 1;
        while (flag)
        {
            flag = 0;
            for (uint i = 0; i < L.size(); ++i)
            {
                n = intersect(s,L[i]);
                if (n.size())
                {
                    s = make_union(s,L[i]);
                    flag = 1;
                }
                else
                {
                    new_L.push_back(L[i]);
                }
            }
            L.swap(new_L);
            new_L.clear();
        }
        result.push_back(s);
    }
    return result;
}


bool is_cluster_member(int index, const clusterlist_t & clusters)
{
    for (uint i = 0; i < clusters.size(); ++i)
    {
        if (std::find(clusters[i].begin(),clusters[i].end(),index)
                != clusters[i].end())
            return true;
    }
    return false;
}

std::vector<int> non_cluster_members
(const clusterlist_t & clusters, int npoints)
{
    std::vector<int> result;
    for (int i = 0; i < npoints; ++i)
        if (!is_cluster_member(i,clusters))
            result.push_back(i);
    return result;
}




//  throw an exception if cluster is in a bad state
//  use a DEBUG macro or something so we can turn this off.
void check_cluster(clusterlist_t & clusters)
{
    /*
    std::set<int> s;
    for (int i = 0; i < clusters.size(); ++i)
        for (int j = 1; j < clusters[i].size(); ++j )
        {
            assert (s.find(clusters[i][j]) == s.end());
            s.insert(clusters[i][j]);
        }
        */
}



void merge_clusters
(const std::vector<std::pair<int,double> > & merged_clusters,
 clusterlist_t &  clusters)
{
    check_cluster(clusters);

    clusterlist_t new_clusters;

    // create the new cluster
    std::vector<int> new_clust;

    std::vector<uint> merged_clusters_first; // projection of merged_clusters
    for (uint i = 0; i < merged_clusters.size(); ++i)
        merged_clusters_first.push_back(merged_clusters[i].first);

    // copy the cluster list
    for (uint i = 0; i < clusters.size(); ++i )
    {
        // if it's not a merged cluster, append it to the new cluster list
        if (std::find(merged_clusters_first.begin(),merged_clusters_first.end(),i
            ) == merged_clusters_first.end())
        {
            // watch out: deep copy .. expensive ?
            new_clusters.push_back(clusters[i]);
        }
    }
    check_cluster(new_clusters);

    for (uint i = 0; i < merged_clusters.size(); ++i)
    {
        assert ( i < clusters.size());
        std::copy(clusters[merged_clusters_first[i]].begin(),clusters[merged_clusters_first[i]].end(),
        std::back_inserter(new_clust));
    }
    new_clusters.push_back(new_clust);
    check_cluster(new_clusters);

    clusters = new_clusters;
}






double euclidean3(const RUMBA::Point<double>& p, const RUMBA::Point<double>&q)
{
    RUMBA::Point<double> r = p-q;
    return std::sqrt( r.x()*r.x()+r.y()*r.y()+r.z()*r.z());
}

RUMBA::Argument myArgs [] =
{
    RUMBA::Argument ( "radius", RUMBA::NUMERIC, 'r' ),
    RUMBA::Argument ( "threshold", RUMBA::NUMERIC, 't' ),
    RUMBA::Argument ( "nonmembers", RUMBA::FLAG, 'n' ),
    RUMBA::Argument ( "bucket", RUMBA::FLAG, 'b' ),
    RUMBA::Argument ( "density", RUMBA::FLAG, 'd' ),
    RUMBA::Argument ( "variance", RUMBA::FLAG, '\0' ),
    RUMBA::Argument ( "rescale", RUMBA::FLAG, 's' ),
    RUMBA::Argument ( "nnbetween", RUMBA::FLAG, '\0'),
    RUMBA::Argument ( "densepoints", RUMBA::FLAG, '\0'),
    RUMBA::Argument ( "step", RUMBA::NUMERIC, '\0'), // step size for iterations
    RUMBA::Argument ( "startradius", RUMBA::NUMERIC, '\0'), // initial value if multiple iterations are used
    RUMBA::Argument ("rjmerge", RUMBA::FLAG, '\0'),
    RUMBA::Argument()
};

void help(const char* progname)
{
    std::cerr << "Usage: " << progname
        << " -r|--radius <radius> -t|--threshold <N> -n|--nonmembers -b|--bucket[ -o|--outfile file [ --density|-d] [--rjmerge] [ --variance] [--nnbetween] [--rescale] ] --densepoints" << std::endl;
    std::cerr << "densepoints: only output list of dense points, don't cluster" << std::endl;
    std::cerr << "nonmembers: show points that don't meet density threshold (these are assigned to \"cluster 0\")" << std::endl;
    std::cerr << "--variance: show the sum mean squared between clusters divided by mean squared between centroids" << std::endl;
    std::cerr << "--nnbetween: similar to variance, but use nearest neighbor distance between clusters instead of centroid distance (recommended)" << std::endl;
    std::cerr << "Note that density is output before variance if both args are given" << std::endl;
}




int main(int argc, char** argv)
{
    double radius = 0;
    int threshold = 0;
    bool show_nonmembers = false;
    std::string outfile;

    std::ostream * out = 0;
    std::ofstream fout;
    double between = 0;
    double * between_ptr = 0;
    bool merge_on_introduction = false;
    double step = -1;
    double startradius = 0.01;
    std::vector<int> cluster_sizes; // number of clusters in the solution for
    // each value of the R parameter

    enum merge_rule_t merge_rule = NN_MERGE;
    try {
        RUMBA::ArgHandler argh(argc,argv,myArgs);
        if (argh.arg("help"))
        {
            help(argv[0]);
            return 0;
        }
        if (argh.arg("step"))
        {
            argh.arg("step",step);
            if (step <= 0)
                throw RUMBA::Exception ("Step size must be positive");
            merge_on_introduction = true;
        }
        if (argh.arg("rjmerge"))
            merge_rule = RJ_MERGE;


        if (argh.arg("radius"))
            argh.arg("radius",radius);
        else
            throw RUMBA::Exception("--radius|-r required");

        if (argh.arg("startradius"))
        {
            argh.arg("startradius",startradius);
            if (startradius <= 0 || startradius > radius)
                throw RUMBA::Exception ("start radius must be positive and less than radius");
            if (!argh.arg("step"))
                step = (radius-startradius)/10;
        }

        if (argh.arg("threshold"))
            argh.arg("threshold",threshold);
        else
            throw RUMBA::Exception("--threshold|-t required");

        if (threshold < 1)
            throw RUMBA::Exception
                ("--threshold argument must be a positive integer");

        if (argh.arg("nonmembers"))
            show_nonmembers = true;

        if (argh.arg("outfile"))
        {
            argh.arg("outfile",outfile);
            fout.open(outfile.c_str());
            if (!fout)
                throw RUMBA::Exception("Couldn't open output file for writing");
            out = &fout;
        }
        else
            out = &std::cout;

        std::vector<point_t> allpoints = loadfile (std::cin);
        std::vector<int> dense_points;
        std::map<int, std::vector<int> > dense_point_neighbours;
        clusterlist_t clusters;
//        if (argh.arg("bucket"))
        if (argh.arg("nnbetween"))
            between_ptr = &between;

        std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
// special case: only return dense points, don't cluster
        if (argh.arg("densepoints")) {
            std::cerr << "Calling find_dense_points" << std::endl;
            find_dense_points(allpoints, radius,threshold, dense_points,
                    dense_point_neighbours);
            // now print 'em out ...
            std::cerr << dense_points.size() << std::endl;
            for (uint i = 0; i < dense_points.size(); ++i)
            {
                if (dense_points[i] >= threshold)
                    printpoint(allpoints[i],*out) << " " << 1 << std::endl;
            }
        }
        else
        {
            std::cerr << "Calling cluster2() " << std::endl;
            clusters = cluster2(
                allpoints, euclidean3, threshold,
//                0.01, radius, radius/10.0 , dense_points, dense_point_neighbours, between_ptr);
                startradius, radius, step , dense_points, dense_point_neighbours, between_ptr, merge_on_introduction, merge_rule, &cluster_sizes);
            std::cerr << "done calling cluster2() " << std::endl;

        }

        if (show_nonmembers)
        {
            for (uint i = 0; i < dense_points.size(); ++i)
            {
                if (dense_points[i] < threshold)
                    printpoint(allpoints[i],*out) << " " << 0 << std::endl;
            }
        }

        for (uint i = 0; i < clusters.size(); ++i )
        {
            /*
            out << "[";
            for (int j = 0; j < clusters[i].size(); ++j)
            {
                out << allpoints[clusters[i][j]] << ", ";
            }
            out << "]" << std::endl;
            */
            for (uint j = 0; j < clusters[i].size(); ++j )
            {
                printpoint(allpoints[clusters[i][j]],*out)
                    << " " << i+1 << std::endl;
            }
        }


        int num_members_rescale = 1;
        if (argh.arg("rescale"))
            num_members_rescale = countmembers(dense_points,threshold);

        if (argh.arg("density"))
        {
            std::cout <<
                density_ratio(dense_points,threshold)
                << std::endl;
        }

        if (argh.arg("variance"))
        {
            if (between_ptr)
            {
                std::cout << getVarwithin(clusters,allpoints,dense_point_neighbours) / between << std::endl;
            }
            else
            {
                std::cout << dovariance(clusters,allpoints, dense_point_neighbours)
                << std::endl;
            }
        }
        if (argh.arg("startradius"))
        {
            double rtmp = startradius;
            for (uint i = 0; i < cluster_sizes.size(); ++i )
            {
                std::cout << rtmp << " " << cluster_sizes[i] << std::endl;
                rtmp += step;
            }
        }
    }
    catch(RUMBA::Exception& e)
    {
        std::cerr << e.error() << std::endl;
        help(argv[0]);
        return 1;
    }
    return 0;
}


// vim: set ts=4 sw=4 expandtab:
// Local Variables:
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
