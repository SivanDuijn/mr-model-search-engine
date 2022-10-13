#include "headers.h"

// Divide a vector of mesh data among a given number of bins
Eigen::VectorXi bin(Eigen::VectorXf data, size_t bins)
{
    // Initialize the histogram
    Eigen::VectorXi hist(bins);
    hist.setZero();

    // Divide the data
    float min = data.minCoeff(), max = data.maxCoeff() + FLT_EPSILON;
    float binsize = (max - min) / bins;
    for (size_t i = 0, size = data.size(); i < size; i++)
        hist[(size_t)((data[i] - min) / binsize)]++;

    return hist;
}

namespace descriptors
{
    // Distance from random vertices to barycenter
    Eigen::VectorXi D1(pmp::SurfaceMesh &mesh, int bins)
    {
        // Get the vertices as an Eigen map
        Eigen::Map<Eigen::MatrixXf> map = utils::GetVertexMap(mesh);

        // Get all the distances to the baricenter
        Eigen::Vector3f bcenter = (Eigen::Vector3f)pmp::centroid(mesh);
        Eigen::VectorXf dist = (map.colwise() - bcenter).colwise().norm();

        // Bin the result
        return bin(dist, bins);
    }

    // Distance from random vertices to each other
    Eigen::VectorXi D2(pmp::SurfaceMesh &mesh, int bins)
    {
        // Get the vertices as an Eigen map
        Eigen::Map<Eigen::MatrixXf> map = utils::GetVertexMap(mesh);

        // Get all the distances between random vertices
        Eigen::VectorXf dist(bins);
        //for (size_t i = 0; i < 2500; i++)

        // Bin the result
        return bin(dist, bins);
    }
}
