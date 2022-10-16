#pragma once

namespace descriptors
{
    // Data needed to construct a histogram
    // Bins range from [ min+binsize*bin, min+binsize*(bin+1) )
    struct Histogram
    {
        Eigen::VectorXi bins;
        float min, binsize;
    };

    struct ShapeDescriptors
    {
        Histogram A3, D1, D2, D3, D4;
    };

    Histogram A3(pmp::SurfaceMesh &mesh, int bins);
    Histogram D1(pmp::SurfaceMesh &mesh, int bins);
    Histogram D2(pmp::SurfaceMesh &mesh, int bins);
    Histogram D3(pmp::SurfaceMesh &mesh, int bins);
    Histogram D4(pmp::SurfaceMesh &mesh, int bins);

    ShapeDescriptors get_shape_descriptors(pmp::SurfaceMesh &mesh, int bins);
}
