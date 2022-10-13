#pragma once

namespace global_descriptors
{
    float CalcSurfaceArea(pmp::SurfaceMesh &mesh);
    // float CalcCompactness(pmp::SurfaceMesh &mesh); // with respect to a sphere
    float CalcAABBVolume(pmp::SurfaceMesh &mesh);
    // float CalcVolume(pmp::SurfaceMesh &mesh);
    // float CalcDiameter(pmp::SurfaceMesh &mesh);
    float CalcEccentricity(pmp::SurfaceMesh &mesh); // ratio of largest to smallest eigenvalues of covariance matrix
}