#include "headers.h"

namespace descriptors
{
    // Divide a vector of mesh data among a given number of bins
    Histogram bin(Eigen::VectorXf data, size_t nBins)
    {
        // Initialize the bins
        Eigen::VectorXf bins(nBins);
        bins.setZero();

        // Divide the data
        float min = data.minCoeff(), max = data.maxCoeff() + FLT_EPSILON;
        float binsize = (max - min) / nBins;
        for (size_t i = 0, size = data.size(); i < size; i++)
            bins[(size_t)((data[i] - min) / binsize)]++;

        return Histogram{ bins, min, binsize };
    }

    // Make a histogram out of data points using predifined min and binSize, also area normalizes
    Histogram bin(Eigen::VectorXf data, size_t nBins, float min, float binsize)
    {
        // Initialize the bins
        Eigen::VectorXf bins(nBins);
        bins.setZero();

        size_t nDataPoints = data.size();
        // Divide the data
        for (size_t i = 0; i < nDataPoints; i++)
            bins[(size_t)((data[i] - min) / binsize)]++;

        // normalize the histogram
        for (size_t i = 0; i < nBins; i++)
            bins[i] = bins[i] / nDataPoints;

        return Histogram{ bins, min, binsize };
    }
    
    // Angle between edges of random vertices
    Eigen::VectorXf A3(pmp::SurfaceMesh &mesh)
    {
        // Get the vertices as an Eigen map
        VertexMap map = utils::GetVertexMap(mesh);

        // Get all the angles between random triangles
        VertexMat edge1 = utils::RandomVertices(map) - map;
        VertexMat edge2 = utils::RandomVertices(map) - map;
        Eigen::VectorXf angle = edge1.cwiseProduct(edge2).colwise().sum().array().acos();

        // Bin the result
        return angle;
    }

    // Distance from random vertices to barycenter
    Eigen::VectorXf D1(pmp::SurfaceMesh &mesh)
    {
        // Get the vertices as an Eigen map
        VertexMap map = utils::GetVertexMap(mesh);

        // Get all the distances to the baricenter
        Vertex bcenter = (Vertex)pmp::centroid(mesh);
        Eigen::VectorXf dist = (map.colwise() - bcenter).colwise().norm();

        // Bin the result
        return dist;
    }

    // Distance from random vertices to each other
    Eigen::VectorXf D2(pmp::SurfaceMesh &mesh)
    {
        // Get the vertices as an Eigen map
        VertexMap map = utils::GetVertexMap(mesh);

        // Get all the distances between random vertices
        VertexMat vert = utils::RandomVertices(map);
        Eigen::VectorXf dist = (map - vert).colwise().norm();

        // Bin the result
        return dist;
    }

    // Surface area of triangle from random vertices
    Eigen::VectorXf D3(pmp::SurfaceMesh &mesh)
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
        // Use square root to normalize
        return area.cwiseSqrt();
    }


    // Volume of tetrahedron from random vertices
    Eigen::VectorXf D4(pmp::SurfaceMesh &mesh)
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
        // Use cube root to normalize
        return volume.unaryExpr([](float v) -> float { return cbrtf(v); });
    }

    // Gets the non normalized histogram shape descriptors for one mesh
    ShapeDescriptors get_shape_descriptors(pmp::SurfaceMesh &mesh, int nBins)
    {
        return ShapeDescriptors
        {
            bin(A3(mesh), nBins),
            bin(D1(mesh), nBins),
            bin(D2(mesh), nBins),
            bin(D3(mesh), nBins),
            bin(D4(mesh), nBins)
        };
    }

    void get_shape_descriptors(string database, vector<string>& filenames, vector<ShapeDescriptors> &descriptors, int nBins)
    {    
        struct RawShapeDescriptors
        {
            float min, max;
            vector<Eigen::VectorXf> v;
        };

        RawShapeDescriptors A3Desc = { __FLT_MAX__, __FLT_MIN__, vector<Eigen::VectorXf>() };
        RawShapeDescriptors D1Desc = { __FLT_MAX__, __FLT_MIN__, vector<Eigen::VectorXf>() };
        RawShapeDescriptors D2Desc = { __FLT_MAX__, __FLT_MIN__, vector<Eigen::VectorXf>() };
        RawShapeDescriptors D3Desc = { __FLT_MAX__, __FLT_MIN__, vector<Eigen::VectorXf>() };
        RawShapeDescriptors D4Desc = { __FLT_MAX__, __FLT_MIN__, vector<Eigen::VectorXf>() };
        for (string file : filenames)
        {
            pmp::SurfaceMesh mesh = database::read_mesh(database, file);

            auto v = A3(mesh);
            A3Desc.min = min(A3Desc.min, v.minCoeff());
            A3Desc.max = max(A3Desc.max, v.maxCoeff());
            A3Desc.v.push_back(v);
            v = D1(mesh);
            D1Desc.min = min(D1Desc.min, v.minCoeff());
            D1Desc.max = max(D1Desc.max, v.maxCoeff());
            D1Desc.v.push_back(v);
            v = D2(mesh);
            D2Desc.min = min(D2Desc.min, v.minCoeff());
            D2Desc.max = max(D2Desc.max, v.maxCoeff());
            D2Desc.v.push_back(v);
            v = D3(mesh);
            D3Desc.min = min(D3Desc.min, v.minCoeff());
            D3Desc.max = max(D3Desc.max, v.maxCoeff());
            D3Desc.v.push_back(v);
            v = D4(mesh);
            D4Desc.min = min(D4Desc.min, v.minCoeff());
            D4Desc.max = max(D4Desc.max, v.maxCoeff());
            D4Desc.v.push_back(v);
        }

        float A3Binsize = (A3Desc.max - A3Desc.min + FLT_EPSILON) / nBins;
        float D1Binsize = (D1Desc.max - D1Desc.min + FLT_EPSILON) / nBins;
        float D3Binsize = (D3Desc.max - D3Desc.min + FLT_EPSILON) / nBins;
        float D2Binsize = (D2Desc.max - D2Desc.min + FLT_EPSILON) / nBins;
        float D4Binsize = (D4Desc.max - D4Desc.min + FLT_EPSILON) / nBins;
        for (size_t i = 0, nFiles = filenames.size(); i < nFiles; i++)
        {
            descriptors.push_back({
                bin(A3Desc.v[i], nBins, A3Desc.min, A3Binsize),
                bin(D1Desc.v[i], nBins, D1Desc.min, D1Binsize),
                bin(D2Desc.v[i], nBins, D2Desc.min, D2Binsize),
                bin(D3Desc.v[i], nBins, D3Desc.min, D3Binsize),
                bin(D4Desc.v[i], nBins, D4Desc.min, D4Binsize),
            });
        }
    }
}
