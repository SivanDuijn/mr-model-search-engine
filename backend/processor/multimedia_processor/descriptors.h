#pragma once

namespace descriptors
{
    Eigen::VectorXi A3(pmp::SurfaceMesh &mesh, int bins);
    Eigen::VectorXi D1(pmp::SurfaceMesh &mesh, int bins);
    Eigen::VectorXi D2(pmp::SurfaceMesh &mesh, int bins);
    Eigen::VectorXi D3(pmp::SurfaceMesh &mesh, int bins);
}
