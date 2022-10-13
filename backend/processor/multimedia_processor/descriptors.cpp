#include "headers.h"

// Divide a vector of data into bins
Eigen::VectorXi bin(Eigen::VectorXf data, int bins)
{
    // Initialize the histogram
    Eigen::VectorXi hist(bins);
    hist.setZero();

    // Divide the data
    float min = data.minCoeff(), max = data.maxCoeff() + FLT_EPSILON;
    float binsize = (max - min) / bins;
    for (size_t i = 0, size = data.size(); i < size; i++)
        hist[(int)((data[i] - min) / binsize)]++;

    return hist;
}

namespace descriptors
{
    Eigen::VectorXi D1(pmp::SurfaceMesh &mesh, int bins)
    {
        // Get the vertices as an Eigen map
        Eigen::Map<Eigen::MatrixXf> map = eigen_vectors::GetVertexMap(mesh);

        // Get all the distances to the baricenter
        Eigen::Vector3f bcenter = (Eigen::Vector3f)pmp::centroid(mesh);
        Eigen::VectorXf dist = (map.colwise() - bcenter).colwise().norm();

        // Bin the result
        return bin(dist, bins);
    }
}
