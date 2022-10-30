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

    const float A3Max = M_PI;          // max angle between edges
    const float D1Max = 0.86602540378; // sqrt(0.5^2 + 0.5^2 + 0.5^2)
    const float D2Max = 1.73205080757; // sqrt(3)
    const float D3Max = 0.84089641525; // sqrt(sqrt(2)*1*0.5)
    const float D4Max = 0.693361274;   // max volume of tetrahedron in unit cube, (0,0,0) (1,1,0) (1,0,1) (0,1,1)

    // Gets the non normalized histogram shape descriptors for one mesh
    ShapeDescriptors get_shape_descriptors(pmp::SurfaceMesh &mesh, int nBins);
    // Gets the normalized histogram, where min max is used over the all the files
    void get_shape_descriptors(std::string database, std::vector<std::string>& filenames, std::vector<ShapeDescriptors> &descriptors, int nBins);
}
