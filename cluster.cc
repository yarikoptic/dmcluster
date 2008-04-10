
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <limits>
#include "cluster.h"
#include "density.h"
#include "rumba/arghandler.h"
#include "verbose.h"

#include <algorithm>
#include <cmath>
#include <set>

#include <nifti1_io.h>

std::string version = "0.1.7+scaling+sort+minclustersize";

typedef float datatype;

inline double square(double x) { return x*x; }


template <typename T>
void setarg(RUMBA::ArgHandler & argh,
            const char * name,
            T & variable,
            const bool printdefault=false)
{
    if (argh.arg(name))
        {
            argh.arg(name, variable);
            vout << 3 << "i: " << name << " is set to " << variable << "\n";
        }
    else if (printdefault)
        vout << 2 << "!: no value for parameter " << name
             << " was specified. Assuming default value " << variable << "\n";
}

bool mycomp(const std::pair<int,double>& left, const std::pair<int,double>& right)
{
    return left.second < right.second;
}

std::ostream& printpoint(const RUMBA::Point<double> & p, std::ostream& out)
{
    out << p.x() << " " << p.y() << " " << p.z() << " " << p.t();
    return out;
}

// Classes to provide easy choice of output -- either in ASCII (stdout/file), where
// simply coordinates are printed, or into brain image volume (analyze,nifti) via libnifti
// where cluster indexes are labeled by class number
class OutputResults
{
public:
    virtual void set(const RUMBA::Point<double> & p, const int optvalue=INT_MAX,
                     const int optindex=INT_MAX) = 0;
    virtual ~OutputResults(){};
};

class OutputASCII: public OutputResults
{
private:
    std::ostream& out;

public:
    OutputASCII(std::ostream& lout)
        : out(lout) {}

    void set(const RUMBA::Point<double> & p, const int optvalue=INT_MAX,
             const int optindex=INT_MAX)
        {
            printpoint(p, out);
            if (optindex != INT_MAX) out << " " << optindex;
            if (optvalue != INT_MAX) out << " " << optvalue;
            out << std::endl;
        }

};

class OutputNifti: public OutputResults
{
private:
    int ex, ey, ez, et,			// helpers to compute coordinates  out of offset
        ox,
        dx, dy, dz, dt;
    double mx, my, mz, mt;
    datatype * data;

protected:
    const std::string outfile;
    nifti_image * ni;
    bool  voxelspace;

public:
    OutputNifti(const std::string loutfile, const std::string origfile)
        : outfile(loutfile)
        {
            // XXX should not actually read all the data from origfile
            //     but let it be like that for now
            ni = nifti_image_read(origfile.c_str(), true);
            data = (datatype *) ni->data;
            // zero it out -- lets do old fashion
            std::fill( data, data + ni->nvox, (datatype)0x0 );
            // set proper output filename
            nifti_set_filenames(ni, loutfile.c_str(), 0, ni->byteorder);

            // Lazy me to create a proper wrapper for NiftiImage class in C++
            ox = ni->nbyper;
            ex = ni->nx*ox; dx=1;
            ey = ni->nx*ni->ny*ox; dy=ni->nx;
            ez = ni->nx*ni->ny*ox*ni->nz; dz=ni->nx*ni->ny;
            et = ni->nx*ni->ny*ox*ni->nz*ni->nt; dt=ni->nx*ni->ny*ni->nz;

            setVoxelSpace(false);
        }

    ~OutputNifti()
        {
            vout << 1 << "Saving results into " << ni->fname << "\n";
            nifti_image_write(ni);
        }

    void setVoxelSpace(bool isInVoxelSpace=true)
        {
            voxelspace = isInVoxelSpace;
            if (voxelspace)
            {
                mx = my = mz = mt = 1;
            }
            else
            {
                mx = ni->dx;
                my = ni->dy;
                mz = ni->dz;
                mt = ni->dt;
            }
        }

    void set(const RUMBA::Point<double> & p, const int optvalue=INT_MAX,
             const int optindex=INT_MAX)
    // Obtains voxel or space coordinates and saves into the ni->data
        {
            int x, y, z, t=0, offset;
            // convert to voxel space
            x = (int) round(p.x()/mx);
            y = (int) round(p.y()/my);
            z = (int) round(p.z()/mz);
            if (mt!=0)
                t = (int) round(p.t()/mt);
            // compute offset
            offset =  dx*x + dy*y + dz*z + dt*t;
            data[offset] = optvalue;
        }
};


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

double getVarwithin( const clusterlist_t& clusters,
                     const std::vector<RUMBA::Point<double> > & allpoints,
                     const std::map<int, std::vector<int> >& dense_point_neighbours)
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


double dovariance ( const clusterlist_t& clusters,
                    const std::vector<RUMBA::Point<double> > & allpoints,
                    const std::map<int, std::vector<int> >& dense_point_neighbours,
                    const double scaling)
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
    if (scaling != 0)
        return vartotal / (varwithin/pow(clusters.size(), scaling));
    else
        return vartotal / varwithin;

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
    VDOUT(91, "point "<<p<<"\n");
    return p;
}

// return a list of points, allpoints
std::vector<point_t> loadfile(std::istream & in)
{
    std::string s;
    std::vector<point_t> result;

    while (std::getline(in,s))
    {
        VDOUT(91, "Line " << s << "\n");
        if (s.length()>100)
            throw RUMBA::Exception (
                "Input line is too big (longer than 100 chars). It must be a binary file");
        result.push_back(readpoint(s));
    }
    return result;
}

// read points from the file using thresholds to remove bogus ones
std::vector<point_t> loadniftifile(std::string infile,
                                   double threshold,
                                   bool voxelspace = false)
/*
  @threshold -- if positive - select voxels above, if negative - select below
  @voxelspace -- if to operate on coord or voxel indexes
 */
{
    std::vector<point_t> result;
    nifti_image * ni = nifti_image_read(infile.c_str(), true);
    double x, y, z, t;
    int ex, ey, ez, et,			// helpers to compute coordinates  out of offset
        ox,
        dx, dy, dz, dt;
    datatype * data = NULL;

    switch (ni->datatype)
    {
    case DT_FLOAT32:
        data = static_cast<datatype*>(ni->data);
        break;
    default:
        throw RUMBA::Exception ("We do not yet handle datatype of the file " + infile +
                                ". File has to be of double (32bit) floating point");
    }

    // Precompute offsets
    ox = 1; //ni->nbyper;
    ex = ni->nx*ox; dx=1;
    ey = ni->nx*ni->ny*ox; dy=ni->nx;
    ez = ni->nx*ni->ny*ox*ni->nz; dz=ni->nx*ni->ny;
    et = ni->nx*ni->ny*ox*ni->nz*ni->nt; dt=ni->nx*ni->ny*ni->nz;

    vout << 2 << "Reading file " << infile << "\n";
    // Lets loop through all voxels in that "Volume"
    for (unsigned long long int offset=0;
         offset< ni->nvox;
         offset++) {
        if ((threshold>0 and data[offset] < threshold) ||
            (threshold<0 and data[offset] > threshold))
            continue;
        x = offset % ex / dx;
        y = offset % ey / dy;
        z = offset % ez / dz;
        t = offset % et / dt;
        if (!voxelspace)
        {
            x *= ni->dx;
            y *= ni->dy;
            z *= ni->dz;
            t *= ni->dt;
        }
        point_t p(x, y, z);
        VDOUT(91, "Adding point " << data[offset] << " = " << p << "\n");
        result.push_back(p);
    }
    vout << 2 << "Found " << result.size() << " points" << "\n";
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


bool _cluster_sort_comp(const std::vector<int>& left, const std::vector<int>& right)
{
    return left.size() > right.size();
}

void sort_clusters(clusterlist_t &  clusters)
{
    std::sort(clusters.begin(), clusters.end(), _cluster_sort_comp);
}


double euclidean3(const RUMBA::Point<double>& p, const RUMBA::Point<double>&q)
{
    RUMBA::Point<double> r = p-q;
    return std::sqrt( r.x()*r.x()+r.y()*r.y()+r.z()*r.z());
}

RUMBA::Argument myArgs [] =
{
    RUMBA::Argument ( "infile",RUMBA::ALPHA,'i'),
    RUMBA::Argument ( "verbose", RUMBA::NUMERIC, 'v' ),
    RUMBA::Argument ( "quiet", RUMBA::FLAG, 'q' ),
    RUMBA::Argument ( "valuesthreshold", RUMBA::NUMERIC, 'T' ),
    RUMBA::Argument ( "radius", RUMBA::NUMERIC, 'r' ),
    RUMBA::Argument ( "threshold", RUMBA::NUMERIC, 't' ),
    RUMBA::Argument ( "nonmembers", RUMBA::FLAG, 'n' ),
    RUMBA::Argument ( "voxelspace", RUMBA::FLAG, '\0' ),
    RUMBA::Argument ( "bucket", RUMBA::FLAG, 'b' ),
    RUMBA::Argument ( "density", RUMBA::FLAG, 'd' ),
    RUMBA::Argument ( "variance", RUMBA::FLAG, '\0' ),
    RUMBA::Argument ( "rescale", RUMBA::FLAG, 's' ),
    RUMBA::Argument ( "nnbetween", RUMBA::FLAG, '\0'),
    RUMBA::Argument ( "no-sort", RUMBA::FLAG, '\0'),
    RUMBA::Argument ( "scaling", RUMBA::FLAG, '\0'),
    RUMBA::Argument ( "scaling-power", RUMBA::NUMERIC, '\0'), // power to which bring number of clusters
    RUMBA::Argument ( "minimal-cluster-size", RUMBA::NUMERIC, 'm'), // prune clusters if size less than given
    RUMBA::Argument ( "densepoints", RUMBA::FLAG, '\0'),
    RUMBA::Argument ( "inputpoints", RUMBA::FLAG, '\0'),
    RUMBA::Argument ( "radiusstep", RUMBA::NUMERIC, '\0'), // step size for iterations
    RUMBA::Argument ( "radiusstart", RUMBA::NUMERIC, '\0'), // initial value if multiple iterations are used
    RUMBA::Argument ( "thresholdstep", RUMBA::NUMERIC, '\0'),
    RUMBA::Argument ( "thresholdstart", RUMBA::NUMERIC, '\0'),
    RUMBA::Argument ("rjmerge", RUMBA::FLAG, '\0'),
    RUMBA::Argument()
};

void help(const char* progname)
{
    std::cerr <<
        "Dense Mode Clustering. Version " << version << std::endl <<
        "Usage:\n" << progname <<
        " [-i|--infile <file>] [-o|--outfile <file>]\n" <<
        " [-r|--radius <radius>] [-t|--threshold <N>]\n" <<
        " [-b|--bucket] [--density|-d] [--rjmerge] [--rescale] ]\n" <<
        " [<additional options>]\n" <<
        std::endl <<
        "Options altering search for optimal value\n" <<
        " (X can be threshold and/or radius):\n" <<
        " [--Xstart]: start value for X\n" <<
        " [--Xstep]:  step value for X\n" <<
        std::endl <<
        "Options available when working with brain volumes:\n" <<
        "  [--voxelspace]: operate on voxel coordinates, not in mm's\n" <<
        "  [--valuesthreshold|-T <value>]: value to threshold at. Can be\n" <<
        "     negative to select negative (only) voxels\n" <<
        std::endl <<
        "Additional common options:\n" <<
        "  [--verbose <level>]: how verbose output should be (default 1)\n" <<
        "  [--quiet|-q]: analog to --verbose 0\n" <<
        "  [--inputpoints]: only output list of input points, so it could be\n" <<
        "                   into elderly cluster tool\n" <<
        "  [--no-sort]: do not sort cluster indexes in descending order\n" <<
        "  [--densepoints]: only output list of dense points, don't cluster\n" <<
        "  [-n|--nonmembers]: show points that don't meet density threshold\n" <<
        "     (these are assigned to \"cluster <numberofclusters+1>\")\n" <<
        "  [--variance]: show the sum mean squared between clusters divided\n" <<
        "     by mean squared between centroids\n" <<
        "  [--nnbetween]: Defines merge rule either to be nearest neighbor distance\n" <<
        "     if set. If not -- defines it as a centroid distance.\n" <<
        "  [--scaling]: scaling the criterion with the number of clusters. If not\n" <<
        "     set then not scale by number of clusters\n" <<
        "  [--scaling-power <value>]: bring number of clusters (enables --scaling)\n" <<
        "     to the given power.\n" <<
        "  [-m|--minimal-cluster-size <value>]: prune clusters with size less specified.\n" <<
        "\n" <<
        "\nNote that density is output before variance if both args are given\n";
}




int main(int argc, char** argv)
{
    double radius = 5.0;
    int threshold = 10;
    double valuesthreshold = 2.3;
    int voxelspace = false;    // by default -- operate in mm
    bool operatenifti = false;  // by default operate with ascii and stdin/stdout

    int show_nonmembers = false;
    std::string infile, outfile;

    OutputResults * out = NULL;
    std::ofstream fout;
    double scaling = 0.0;
    double between = 0;
    double * between_ptr = 0;
    bool do_sort_clusters = true;
    bool merge_on_introduction = false;
    double radiusstep = 0.5;
    double radiusstart = 0.01;
    int thresholdstep = 1;
    int thresholdstart = 10;
    int minimal_cluster_size = 1;

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

        if (argh.arg("verbose"))
        {
            int verbosity;
            argh.arg("verbose", verbosity);
            vout.setLevel(verbosity);
        }

        vout << 1 << "Dense Mode Clustering. Version " << version << "\n";

        if (argh.arg("quiet"))
            vout.setLevel(0);

        if (argh.arg("radiusstep"))
        {
            setarg(argh, "radiusstep", radiusstep);
            if (radiusstep <= 0)
                throw RUMBA::Exception ("Radius step size must be positive");
            merge_on_introduction = true;
        }

        if (argh.arg("rjmerge"))
            merge_rule = RJ_MERGE;

        if (argh.arg("no-sort"))
            do_sort_clusters = false;

        setarg(argh, "radius", radius, true);

        if (argh.arg("radiusstart"))
        {
            setarg(argh, "radiusstart", radiusstart);
            if (radiusstart <= 0 || radiusstart > radius)
                throw RUMBA::Exception ("start radius must be positive and less than radius");
            if (!argh.arg("radiusstep"))
            {
                radiusstep = (radius-radiusstart)/10;
                setarg(argh, "radiusstep", radiusstep, true);
            }
        }
        else
        {   // There is no sense to keep default value.
            radiusstart = radius;
            vout << 3 << "i: radiusstart assigned radius value "
                 << radiusstart << " since no radiusstart is given\n";
        }

        setarg(argh, "threshold", threshold, true);
        thresholdstart = threshold;
        // XXX Do checks
        setarg(argh, "thresholdstep", thresholdstep);
        setarg(argh, "thresholdstart", thresholdstart);
        setarg(argh, "minimal-cluster-size", minimal_cluster_size);

        if (thresholdstart != threshold) // so if thresholdstart was provided
            if (!argh.arg("thresholdstep"))
            {
                thresholdstep = (threshold - thresholdstart)/10;
                vout << 3 << "i: thresholdstep assigned " << thresholdstep << "\n";
            }

        if (argh.arg("infile"))
        {
            setarg(argh, "valuesthreshold", valuesthreshold, true);
            setarg(argh, "voxelspace", voxelspace, true);
            //voxelspace = argh.arg("voxelspace");
        }

        if (threshold < 1)
            throw RUMBA::Exception
                ("--threshold argument must be a positive integer");

        setarg(argh, "nonmembers", show_nonmembers, true);

        std::vector<point_t> allpoints;
        std::vector<int> dense_points;
        std::map<int, std::vector<int> > dense_point_neighbours;
        clusterlist_t clusters;

        vout << 1 << "Loading data " << "\n";

        if (argh.arg("infile"))
        {
            argh.arg("infile", infile);
            allpoints = loadniftifile(infile, valuesthreshold, voxelspace);
            operatenifti = true;
        }
        else
        {
            // Read ASCII input from stdin
            vout << 2 << "Reading coordinates from standard input (in ASCII form)\n";
            allpoints = loadfile (std::cin);
        }

        vout << 1 << "Setting up output\n";

        if (argh.arg("outfile"))
        {
            argh.arg("outfile", outfile);

            if (operatenifti && !argh.arg("inputpoints"))
            {
                out = new OutputNifti(outfile, infile);
                dynamic_cast<OutputNifti*>(out)->setVoxelSpace(voxelspace);
            }
            else
            {
                fout.open(outfile.c_str());
                if (!fout)
                    throw RUMBA::Exception("Couldn't open output file for writing");
                out = new OutputASCII(fout);
            }
        }
        else
            out = new OutputASCII(std::cout);



        // if (argh.arg("bucket"))
        if (argh.arg("nnbetween"))
            between_ptr = &between;

        if (argh.arg("scaling") || argh.arg("scaling-power"))
            if (argh.arg("scaling-power"))
                argh.arg("scaling-power", scaling);
            else
                scaling = 1.0;

        vout << 1 << "Processing\n";

        if (argh.arg("inputpoints")) {
            // special case: only return input points, do nothing
            vout << 1 << "Output input points" << "\n";
            vout << 2 << "Total number of points = " << allpoints.size() << "\n";
            for (uint i = 0; i < allpoints.size(); ++i)
            {
                out->set(allpoints[i]);
            }
        }
        else if (argh.arg("densepoints")) {
            // special case: only return input points, do nothing
            uint actual_dense_points = 0;
            vout << 3 << "Calling find_dense_points" << "\n";
            find_dense_points(allpoints, radius,threshold, dense_points,
                    dense_point_neighbours);
            // now print 'em out ...
            vout << 2 << "Total number of points = " << dense_points.size() << "\n";
            for (uint i = 0; i < dense_points.size(); ++i)
            {
                if (dense_points[i] >= threshold)
                {
                    //out->set(allpoints[i], 1); //dense_points[i]);
                    out->set(allpoints[i], dense_points[i]);
                    actual_dense_points++;
                }
                //printpoint(allpoints[i],*out) << " " << 1 << std::endl;
            }
            vout << 2 << "Number of dense points = " << actual_dense_points << "\n";
        }
        else if (1) // radiusstart!= radius || thresholdstart != threshold )
        {
            vout << 3 << "Sweeping over different values of threshold and radius\n";
            int t = thresholdstart, bestt=0, tried=0;
            double bestr=0, bestcrit=0, crit=0;

            double r = radiusstart;
            vout << -5 << "        ";
            while (1)
            {
                vout << -5  << "     " << std::left << std::setw(7) << r;
                if (r>=radius) break;
                r+=radiusstep;
            }
            vout << -5 << "\n";
            do
            {
                double r = radiusstart;
                vout << -5 << "thr=" << std::setw(3)<< t << ": ";
                do
                {
                    vout << 6 << "Calling cluster_plain() " << "\n";

                    // find clusters
                    clusters = cluster_plain(
                        allpoints, euclidean3, t, r,
                        dense_points, dense_point_neighbours, between_ptr,
                        merge_on_introduction, merge_rule, minimal_cluster_size);

                    // check the quality ;-)
                    if (between_ptr)
                        crit = getVarwithin(clusters, allpoints, dense_point_neighbours) / between;
                    else
                        crit = dovariance(clusters, allpoints, dense_point_neighbours, scaling);
                    if ( crit>bestcrit ) // asssume we are maximizing
                    {
                        bestcrit = crit;
                        bestr = r;
                        bestt = t;
                    }
                    vout << -5  << " "//<< "\t" //<< "R=" << r << " "
                         << std::setw(5) << std::setprecision(3) << std::right << crit
                         << "/" << std::setw(5) << std::left << clusters.size();
                    tried++;
                    r += radiusstep;
                } while (r <= radius);

                vout << -5 << "\n";
                t += thresholdstep;
            } while (t <= threshold);

            if (bestcrit>0 && tried > 1)
            {                   // recompute for optimal values
                clusters = cluster_plain(
                    allpoints, euclidean3, bestt, bestr,
                    dense_points, dense_point_neighbours, between_ptr,
                    merge_on_introduction, merge_rule, minimal_cluster_size);

                // check the quality ;-)
                if (between_ptr)
                    crit = getVarwithin(clusters, allpoints, dense_point_neighbours) / between;
                else
                    crit = dovariance(clusters, allpoints, dense_point_neighbours, scaling);
                // we run deterministic algorithm!
                assert(crit==bestcrit);
            }
            vout << 1 << "crit=" << crit <<
                " #clusters=" << clusters.size() <<
                " threshold=" << bestt << " radius="<<bestr << "\n";

        }
        else
        {
            return -1;
            vout << 3 << "Calling cluster2() " << "\n";
            clusters = cluster2(
                allpoints, euclidean3, threshold,
                radiusstart, radius, radiusstep , dense_points, dense_point_neighbours, between_ptr,
                merge_on_introduction, merge_rule, &cluster_sizes);
            vout << 3 << "cluster2() has completed with " << clusters.size() << " clusters found." << "\n";
        }

        // we might need just to sort them
        if (do_sort_clusters)
        {
            vout << 2 << "Sorting clusters by the number of members\n";
            sort_clusters(clusters);
        }

        if (show_nonmembers)
        {
            vout << 3 << "Output non-members\n";
            for (uint i = 0; i < dense_points.size(); ++i)
            {
                if (dense_points[i] < threshold)
                    out->set(allpoints[i], clusters.size()+1);
                //printpoint(allpoints[i],*out) << " " << 0 << std::endl;
            }
        }

        vout << 3 << "Output clusters\n";
        for (uint i = 0; i < clusters.size(); ++i )
        {
            for (uint j = 0; j < clusters[i].size(); ++j )
            {
                // j is disregarded in nifti file output
                out->set(allpoints[clusters[i][j]], i+1, j);
                //printpoint(allpoints[clusters[i][j]],*out)
                //    << " " << j << " " << i+1 << std::endl;
            }
        }


        int num_members_rescale = 1;
        if (argh.arg("rescale"))
            vout << 3 << "Rescaling\n";
            num_members_rescale = countmembers(dense_points,threshold);

        if (argh.arg("density"))
        {
            vout << 2 << "Output density\n";
            std::cout << density_ratio(dense_points,threshold)
                      << std::endl;
        }

        if (argh.arg("variance"))
            if (clusters.size()>0)
            {
                vout << 3 << "Output variance\n";
                if (between_ptr)
                {
                    std::cout << getVarwithin(clusters,allpoints,dense_point_neighbours) / between
                              << std::endl;
                }
                else
                {
                    std::cout << dovariance(clusters,allpoints, dense_point_neighbours, scaling)
                              << std::endl;
                }
            }
            else
                std::cout << "0\nNo clusters found. Variance cannot be computed\n";

        if (argh.arg("radiusstart"))
        {
            double rtmp = radiusstart;
            for (uint i = 0; i < cluster_sizes.size(); ++i )
            {
                std::cout << rtmp << " " << cluster_sizes[i] << std::endl;
                rtmp += radiusstep;
            }
        }
    }
    catch(RUMBA::Exception& e)
    {
        vout << 0 << e.error() << "\n";
        help(argv[0]);
        return 1;
    }
    delete out;
    return 0;
}


// vim: set ts=4 sw=4 expandtab:
// Local Variables:
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
