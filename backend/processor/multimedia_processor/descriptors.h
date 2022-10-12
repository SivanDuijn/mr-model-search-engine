#pragma once

namespace descriptors
{
    // Maybe rename to DistBarycenterToRandomVertices, or add comment explaining what it calculates
    Eigen::VectorXi D1(pmp::SurfaceMesh &mesh, int bins);
}