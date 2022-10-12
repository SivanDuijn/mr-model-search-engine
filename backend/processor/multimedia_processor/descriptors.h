#pragma once

#include <pmp/SurfaceMesh.h>
#include <pmp/algorithms/DifferentialGeometry.h>
#include "eigen_vectors.h"


namespace descriptors
{
    Eigen::VectorXf D1(pmp::SurfaceMesh &mesh, int bins);
}