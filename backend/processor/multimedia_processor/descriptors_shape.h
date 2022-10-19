#pragma once

namespace descriptors
{
    // Data needed to construct a histogram
    // Bins range from [ min+binsize*bin, min+binsize*(bin+1) )
    struct Histogram
    {
        Eigen::VectorXf bins;
        float min, binsize;
    };

    struct ShapeDescriptors
    {
        Histogram A3, D1, D2, D3, D4;
    };

    Eigen::VectorXf A3(pmp::SurfaceMesh &mesh);
    Eigen::VectorXf D1(pmp::SurfaceMesh &mesh);
    Eigen::VectorXf D2(pmp::SurfaceMesh &mesh);
    Eigen::VectorXf D3(pmp::SurfaceMesh &mesh);
    Eigen::VectorXf D4(pmp::SurfaceMesh &mesh);

    // Gets the non normalized histogram shape descriptors for one mesh
    ShapeDescriptors get_shape_descriptors(pmp::SurfaceMesh &mesh, int nBins);

    // Gets the normalized histogram, where min max is used over the all the files
    void get_shape_descriptors(std::string database, std::vector<std::string>& filenames, std::vector<ShapeDescriptors> &descriptors, int nBins);
}
