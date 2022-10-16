#include "headers.h"

namespace descriptors
{
    // Divide a vector of mesh data among a given number of bins
    Histogram bin(Eigen::VectorXf data, size_t count)
    {
        // Initialize the bins
        Eigen::VectorXi bins(count);
        bins.setZero();

        // Divide the data
        float min = data.minCoeff(), max = data.maxCoeff() + FLT_EPSILON;
        float binsize = (max - min) / count;
        for (size_t i = 0, size = data.size(); i < size; i++)
            bins[(size_t)((data[i] - min) / binsize)]++;

        return Histogram{ bins, min, binsize };
    }
    
    // Angle between edges of random vertices
    Histogram A3(pmp::SurfaceMesh &mesh, int bins)
    {
        // Get the vertices as an Eigen map
        VertexMap map = utils::GetVertexMap(mesh);

        // Get all the angles between random triangles
        VertexMat edge1 = utils::RandomVertices(map) - map;
        VertexMat edge2 = utils::RandomVertices(map) - map;
        Eigen::VectorXf angle = edge1.cwiseProduct(edge2).colwise().sum().array().acos();

        // Bin the result
        return bin(angle, bins);
    }

    // Distance from random vertices to barycenter
    Histogram D1(pmp::SurfaceMesh &mesh, int bins)
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
    Histogram D2(pmp::SurfaceMesh &mesh, int bins)
    {
        // Get the vertices as an Eigen map
        VertexMap map = utils::GetVertexMap(mesh);

        // Get all the distances between random vertices
        VertexMat vert = utils::RandomVertices(map);
        Eigen::VectorXf dist = (map - vert).colwise().norm();

        // Bin the result
        return bin(dist, bins);
    }

    // Surface area of triangle from random vertices
    Histogram D3(pmp::SurfaceMesh &mesh, int bins)
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

    // Volume of tetrahedron from random vertices
    Histogram D4(pmp::SurfaceMesh &mesh, int bins)
    {
        // Get the vertices as an Eigen map
        VertexMap map = utils::GetVertexMap(mesh);

        // Get all the volumes of random tetrahedron
        // https://stackoverflow.com/a/9866530
        VertexMat vert1 = utils::RandomVertices(map) - map;
        VertexMat vert2 = utils::RandomVertices(map) - map;
        VertexMat vert3 = utils::RandomVertices(map) - map;
        VertexMat div   = VertexMat();
        div.row(0) = vert2.row(1).cwiseProduct(vert3.row(2) -
            vert2.row(2).cwiseProduct(vert3.row(1)));
        div.row(1) = vert2.row(2).cwiseProduct(vert3.row(0) -
            vert2.row(0).cwiseProduct(vert3.row(2)));
        div.row(2) = vert2.row(0).cwiseProduct(vert3.row(1) -
            vert2.row(1).cwiseProduct(vert3.row(0)));
        Eigen::VectorXf volume = vert1.cwiseProduct(div).colwise().sum() / 6;

        // Bin the result
        return bin(volume, bins);
    }

    ShapeDescriptors get_shape_descriptors(pmp::SurfaceMesh &mesh, int bins)
    {
        return ShapeDescriptors
        {
            A3(mesh, bins),
            D1(mesh, bins),
            D2(mesh, bins),
            D3(mesh, bins),
            D4(mesh, bins)
        };
    }
}
