#include "headers.h"

namespace descriptors
{
    // Create a histogram from a vector of data
    Histogram bin(Eigen::VectorXf data, size_t bincount)
    {
        // Initialize the bins
        Eigen::VectorXf bins(bincount);
        bins.setZero();

        // Divide the data
        float min = data.minCoeff(), max = data.maxCoeff() + EPSILON;
        float binsize = (max - min) / bincount;
        for (size_t i = 0, size = data.size(); i < size; i++)
            bins[(size_t)((data[i] - min) / binsize)]++;

        return Histogram{ bins, min, binsize };
    }

    // Create a normalized histogram from a vector of data using predifined min and binSize
    Histogram bin(Eigen::VectorXf data, size_t bincount, float min, float binsize)
    {
        // Initialize the bins
        Eigen::VectorXf bins(bincount);
        bins.setZero();

        // Divide the data
        size_t datacount = data.size();
        for (size_t i = 0; i < datacount; i++)
            bins[(size_t)((data[i] - min) / binsize)]++;

        // Normalize the histogram
        bins /= datacount;

        return Histogram{ bins, min, binsize };
    }

    // Create a normalized histogram from live data
    // Samplecount must be a power of 2!
    Histogram rolling_bin(pmp::SurfaceMesh &mesh,
                          Eigen::VectorXf (*desc)(pmp::SurfaceMesh&),
                          size_t samplecount,
                          size_t bincount, float min, float binsize, const string name = "")
    {
        printf_debug("Calculating %s...", name.c_str());
        // Initialize the bins
        Eigen::VectorXf bins(bincount);
        bins.setZero();

        // Divide the data
        Eigen::VectorXf data(VERTEX_COUNT);
        for (size_t i = 0, count = samplecount / VERTEX_COUNT; i < count; i++)
        {
            data = desc(mesh);
            for (size_t j = 0, size = VERTEX_COUNT; j < size; j++)
                bins[(size_t)((data[j] - min) / binsize)]++;
        }
        
        // Normalize the histogram
        bins /= samplecount;

        printf_debug("  done\n");
        return Histogram{ bins, min, binsize };
    }
    
    // Apply a descriptor to every vertex on a mesh
    // Angle between edges of random vertices
    Eigen::VectorXf A3(pmp::SurfaceMesh &mesh)
    {
        // printf_debug("Calculating A3...");
        // Get the vertices as an Eigen map
        VertexMap map = VertexProperties::GetVertexMap(mesh);

        // Get all the angles between random triangles
        VertexMat nedge1 = (VertexProperties::RandomVertices(map) - map).colwise().normalized();
        VertexMat nedge2 = (VertexProperties::RandomVertices(map) - map).colwise().normalized();
        Eigen::VectorXf angle = nedge1.cwiseProduct(nedge2).colwise().sum().array().acos();

        // printf_debug("           done\n");
        return angle;
    }

    // Apply a descriptor to every vertex on a mesh
    // Distance from random vertices to barycenter
    Eigen::VectorXf D1(pmp::SurfaceMesh &mesh)
    {
        // printf_debug("Calculating D1...");
        // Get the vertices as an Eigen map
        VertexMap map = VertexProperties::GetVertexMap(mesh);

        // Get all the distances to the baricenter
        Vertex bcenter = (Vertex)pmp::centroid(mesh);
        Eigen::VectorXf dist = (map.colwise() - bcenter).colwise().norm();

        // printf_debug("           done\n");
        return dist;
    }

    // Apply a descriptor to every vertex on a mesh
    // Distance from random vertices to each other
    Eigen::VectorXf D2(pmp::SurfaceMesh &mesh)
    {
        // printf_debug("Calculating D2...");
        // Get the vertices as an Eigen map
        VertexMap map = VertexProperties::GetVertexMap(mesh);

        // Get all the distances between random vertices
        VertexMat vert = VertexProperties::RandomVertices(map);
        Eigen::VectorXf dist = (map - vert).colwise().norm();

        // printf_debug("           done\n");
        return dist;
    }

    // Apply a descriptor to every vertex on a mesh
    // Surface area of triangle from random vertices
    Eigen::VectorXf D3(pmp::SurfaceMesh &mesh)
    {
        // printf_debug("Calculating D3...");
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

        // Use square root to normalize
        // printf_debug("           done\n");
        return area.cwiseSqrt();
    }

    // Apply a descriptor to every vertex on a mesh
    // Volume of tetrahedron from random vertices
    Eigen::VectorXf D4(pmp::SurfaceMesh &mesh)
    {
        // printf_debug("Calculating D4...");
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

        // Use cube root to normalize
        // printf_debug("           done\n");
        return volume.unaryExpr([](float v) -> float { return cbrtf(abs(v)); });
    }

    // Gets the non normalized histogram shape descriptors for one mesh
    ShapeDescriptors get_shape_descriptors(pmp::SurfaceMesh &mesh, int nBins)
    {
        const float A3Binsize = (A3Max + EPSILON) / nBins;
        const float D1Binsize = (D1Max + EPSILON) / nBins;
        const float D2Binsize = (D2Max + EPSILON) / nBins;
        const float D3Binsize = (D3Max + EPSILON) / nBins;
        const float D4Binsize = (D4Max + EPSILON) / nBins;

        return ShapeDescriptors
        {
            rolling_bin(mesh, A3, SAMPLE_COUNT, nBins, 0, A3Binsize, "A3"),
            rolling_bin(mesh, D1, SAMPLE_COUNT, nBins, 0, D1Binsize, "D1"),
            rolling_bin(mesh, D2, SAMPLE_COUNT, nBins, 0, D2Binsize, "D2"),
            rolling_bin(mesh, D3, SAMPLE_COUNT, nBins, 0, D3Binsize, "D3"),
            rolling_bin(mesh, D4, SAMPLE_COUNT, nBins, 0, D4Binsize, "D4"),
        };
    }

    void get_shape_descriptors(vector<string> &filenames, vector<ShapeDescriptors> &descriptors, int nBins)
    {
        const float A3Binsize = (A3Max + EPSILON) / nBins;
        const float D1Binsize = (D1Max + EPSILON) / nBins;
        const float D2Binsize = (D2Max + EPSILON) / nBins;
        const float D3Binsize = (D3Max + EPSILON) / nBins;
        const float D4Binsize = (D4Max + EPSILON) / nBins;

        for (string file : filenames)
        {
            pmp::SurfaceMesh mesh = Database::ReadMeshFromDatabase(file);
            descriptors.push_back({
                rolling_bin(mesh, A3, SAMPLE_COUNT, nBins, 0, A3Binsize, "A3"),
                rolling_bin(mesh, D1, SAMPLE_COUNT, nBins, 0, D1Binsize, "D1"),
                rolling_bin(mesh, D2, SAMPLE_COUNT, nBins, 0, D2Binsize, "D2"),
                rolling_bin(mesh, D3, SAMPLE_COUNT, nBins, 0, D3Binsize, "D3"),
                rolling_bin(mesh, D4, SAMPLE_COUNT, nBins, 0, D4Binsize, "D4"),
            });
        }
    }

    void standardize_dists(const vector<descriptors::ShapeDescriptors>& descriptors, Eigen::MatrixXf& standardized_dists, Eigen::VectorXf& dists_mean, Eigen::VectorXf& dists_sd)
    {
        size_t n_models = descriptors.size();
        size_t n_descriptors = 5;

        // Calculate mean and SD of shape descriptor distance
        // shape descriptor distances, row for each shape descriptor
        Eigen::MatrixXf dists(n_descriptors, (n_models*(n_models - 1)) / 2);
        for (int i = 0, d_i = 0; i < n_models; i++) 
            for (int j = i + 1; j < n_models; j++, d_i++)
            {
                // i j loop through all the models but i != j and j > i, so we don't have duplicate distances
                // d_i is the distance index, just a counter for the distances

                // calculate
                dists(0, d_i) = distance::shape_vf_distance(descriptors[i].A3.bins, descriptors[j].A3.bins);
                dists(1, d_i) = distance::shape_vf_distance(descriptors[i].D1.bins, descriptors[j].D1.bins);
                dists(2, d_i) = distance::shape_vf_distance(descriptors[i].D2.bins, descriptors[j].D2.bins);
                dists(3, d_i) = distance::shape_vf_distance(descriptors[i].D3.bins, descriptors[j].D3.bins);
                dists(4, d_i) = distance::shape_vf_distance(descriptors[i].D4.bins, descriptors[j].D4.bins);
            }
        
        dists_mean = dists.rowwise().mean();
        dists_sd = ((dists.colwise() - dists_mean).array().square().rowwise().sum() / (dists.cols() - 1)).sqrt();

        // auto standardized_shape_dists = (shape_dists.colwise() - shape_dists_mean).array().colwise() / shape_dists_sd.array();
        standardized_dists = dists.array().colwise() / dists_sd.array();
    }
}
