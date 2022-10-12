#pragma once

#include <pmp/SurfaceMesh.h>
#include <pmp/algorithms/DifferentialGeometry.h>
#include "utils.h"


namespace descriptors
{
    Eigen::VectorXi D1(pmp::SurfaceMesh &mesh, int bins);
}