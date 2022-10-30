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
        for (size_t i = 0, size = data.size(); i < size; i++) {
            int bin = (size_t)((data[i] - min) / binsize);
            assert(bin < nBins - 1);
            bins[(size_t)((data[i] - min) / binsize)]++;
        }

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
        printf_debug("Calculating A3...");
        // Get the vertices as an Eigen map
        VertexMap map = VertexProperties::GetVertexMap(mesh);

        // Get all the angles between random triangles
        VertexMat nedge1 = (VertexProperties::RandomVertices(map) - map).colwise().normalized();
        VertexMat nedge2 = (VertexProperties::RandomVertices(map) - map).colwise().normalized();
        Eigen::VectorXf angle = nedge1.cwiseProduct(nedge2).colwise().sum().array().acos();

        // Bin the result
        printf_debug("           done\n");
        return angle;
    }

    // Distance from random vertices to barycenter
    Eigen::VectorXf D1(pmp::SurfaceMesh &mesh)
    {
        printf_debug("Calculating D1...");
        // Get the vertices as an Eigen map
        VertexMap map = VertexProperties::GetVertexMap(mesh);

        // Get all the distances to the baricenter
        Vertex bcenter = (Vertex)pmp::centroid(mesh);
        Eigen::VectorXf dist = (map.colwise() - bcenter).colwise().norm();

        // Bin the result
        printf_debug("           done\n");
        return dist;
    }

    // Distance from random vertices to each other
    Eigen::VectorXf D2(pmp::SurfaceMesh &mesh)
    {
        printf_debug("Calculating D2...");
        // Get the vertices as an Eigen map
        VertexMap map = VertexProperties::GetVertexMap(mesh);

        // Get all the distances between random vertices
        VertexMat vert = VertexProperties::RandomVertices(map);
        Eigen::VectorXf dist = (map - vert).colwise().norm();

        // Bin the result
        printf_debug("           done\n");
        return dist;
    }

    // Surface area of triangle from random vertices
    Eigen::VectorXf D3(pmp::SurfaceMesh &mesh)
    {
        printf_debug("Calculating D3...");
        // Get the vertices as an Eigen map
        VertexMap map = VertexProperties::GetVertexMap(mesh);

        // Get all the areas of random triangles
        // https://math.stackexchange.com/a/1951650
        VertexMat edge1 = VertexProperties::RandomVertices(map) - map;
        VertexMat edge2 = VertexProperties::RandomVertices(map) - map;
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
        printf_debug("           done\n");
        return area.cwiseSqrt();
    }

    // Volume of tetrahedron from random vertices
    Eigen::VectorXf D4(pmp::SurfaceMesh &mesh)
    {
        printf_debug("Calculating D4...");
        // Get the vertices as an Eigen map
        VertexMap map = VertexProperties::GetVertexMap(mesh);

        // Get all the volumes of random tetrahedron
        // https://stackoverflow.com/a/9866530
        VertexMat vert1 = VertexProperties::RandomVertices(map) - map;
        VertexMat vert2 = VertexProperties::RandomVertices(map) - map;
        VertexMat vert3 = VertexProperties::RandomVertices(map) - map;
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
        printf_debug("           done\n");
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

    void get_shape_descriptors(string database, vector<string> &filenames, vector<ShapeDescriptors> &descriptors, int nBins)
    {    
        vector<Eigen::VectorXf> A3Desc;
        vector<Eigen::VectorXf> D1Desc;
        vector<Eigen::VectorXf> D2Desc;
        vector<Eigen::VectorXf> D3Desc;
        vector<Eigen::VectorXf> D4Desc;
        float A3Max = M_PI;
        float D1Max = 0.86602540378; // sqrt(0.5^2 + 0.5^2 + 0.5^2)
        float D2Max = 1.73205080757; // sqrt(3)
        float D3Max = 0.84089641525; // sqrt(sqrt(2)*1*0.5)
        float D4Max = 0.693361274; // max volume of tetrahedron in unit cube, (0,0,0) (1,1,0) (1,0,1) (0,1,1)
        float A3Binsize = (A3Max + FLT_EPSILON) / nBins;
        float D1Binsize = (D1Max + FLT_EPSILON) / nBins;
        float D2Binsize = (D2Max + FLT_EPSILON) / nBins;
        float D3Binsize = (D3Max + FLT_EPSILON) / nBins;
        float D4Binsize = (D4Max + FLT_EPSILON) / nBins;

        for (string file : filenames)
        {
            pmp::SurfaceMesh mesh = database::read_mesh(database, file);
            descriptors.push_back({
                bin(A3(mesh), nBins, 0, A3Binsize),
                bin(D1(mesh), nBins, 0, D1Binsize),
                bin(D2(mesh), nBins, 0, D2Binsize),
                bin(D3(mesh), nBins, 0, D3Binsize),
                bin(D4(mesh), nBins, 0, D4Binsize),
            });
        }
    }
}
