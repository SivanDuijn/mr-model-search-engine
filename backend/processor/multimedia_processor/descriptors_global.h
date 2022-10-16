#pragma once

namespace descriptors
{
    struct GlobalDescriptors
    {
        float surfaceArea;
        float AABBVolume;
        float volume;
        float eccentricity;
        float compactness;
        float diameter;
    };

    float CalcSurfaceArea(pmp::SurfaceMesh &mesh);
    float CalcAABBVolume(pmp::SurfaceMesh &mesh);
    float CalcVolume(pmp::SurfaceMesh &mesh);
    float CalcCompactness(pmp::SurfaceMesh &mesh); // with respect to a sphere
    float CalcCompactness(float surfaceArea, float volume);
    float CalcDiameter(pmp::SurfaceMesh &mesh);
    float CalcEccentricity(pmp::SurfaceMesh &mesh); // ratio of largest to smallest eigenvalues of covariance matrix

    GlobalDescriptors CalcAll(pmp::SurfaceMesh &mesh);
}