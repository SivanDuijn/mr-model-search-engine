#include "descriptors.h"

// Divide a vector of data into bins
Eigen::VectorXf bin(Eigen::VectorXf data, int bins)
{
    // TODO
    return data;
}

namespace descriptors
{
    Eigen::VectorXf D1(pmp::SurfaceMesh &mesh, int bins)
    {
        // Get the vertices as an Eigen map
        Eigen::Map<Eigen::MatrixXf> map = utils::GetVertexMap(mesh);

        // Get all the distances to the baricenter
        Eigen::Vector3f bcenter = (Eigen::Vector3f)pmp::centroid(mesh);
        Eigen::VectorXf dist = (bcenter - map).colwise().norm();

        // Bin the result
        return bin(dist, bins);
    }
}
