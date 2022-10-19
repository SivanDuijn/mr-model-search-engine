#pragma once

namespace descriptors
{
    struct GlobalDescriptors
    {
        float surfaceArea;
        float compactness;
        float AABBVolume;
        float volume;
        float diameter;
        float eccentricity;
        float sphericity;
        float rectangularity;
    };

    float surface_area(pmp::SurfaceMesh &mesh);
    float compactness(pmp::SurfaceMesh &mesh); // with respect to a sphere
    float compactness(float surfaceArea, float volume);
    float aabb_volume(pmp::SurfaceMesh &mesh);
    float volume(pmp::SurfaceMesh &mesh);
    float diameter(pmp::SurfaceMesh &mesh);
    float eccentricity(pmp::SurfaceMesh &mesh); // ratio of largest to smallest eigenvalues of covariance matrix

    GlobalDescriptors get_global_descriptors(pmp::SurfaceMesh &mesh);
    void get_global_descriptors(std::string database, std::vector<std::string>& filenames, std::vector<GlobalDescriptors> &descriptors);
}