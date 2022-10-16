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
        VertexMap map = utils::GetVertexMap(mesh);

        // Get all the distances to the baricenter
        Vertex bcenter = (Vertex)pmp::centroid(mesh);
        Eigen::VectorXf dist = (map.colwise() - bcenter).colwise().norm();

        // Bin the result
        return bin(dist, bins);
    }

    // Distance from random vertices to each other
    Eigen::VectorXi D2(pmp::SurfaceMesh &mesh, int bins)
    {
        // Get the vertices as an Eigen map
        VertexMap map = utils::GetVertexMap(mesh);

        // Get all the distances between random vertices
        VertexMat verts = utils::RandomVertices(map);
        Eigen::VectorXf dist = (map - verts).colwise().norm();

        // Bin the result
        return bin(dist, bins);
    }

    Eigen::VectorXi D3(pmp::SurfaceMesh &mesh, int bins)
    {
        // Get the vertices as an Eigen map
        VertexMap map = utils::GetVertexMap(mesh);

        // Get all the areas of random triangles
        // https://math.stackexchange.com/a/1951650
        VertexMat edge1 = utils::RandomVertices(map) - map;
        VertexMat edge2 = utils::RandomVertices(map) - map;
        VertexMat norm  = VertexMat();
        norm.row(0) = edge1.row(1).cwiseProduct(edge2.row(2) -
            edge1.row(2).cwiseProduct(edge2.row(1)));
        norm.row(1) = edge1.row(2).cwiseProduct(edge2.row(0) -
            edge1.row(0).cwiseProduct(edge2.row(2)));
        norm.row(2) = edge1.row(0).cwiseProduct(edge2.row(1) -
            edge1.row(1).cwiseProduct(edge2.row(0)));
        Eigen::VectorXf area = norm.colwise().norm() / 2;

        // Bin the result
        return bin(area, bins);
    }
}
