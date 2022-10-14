#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <pmp/SurfaceMesh.h>
#include <pmp/BoundingBox.h>
#include <pmp/algorithms/Triangulation.h>
#include <pmp/algorithms/Subdivision.h>
#include <pmp/algorithms/Decimation.h>
#include <pmp/algorithms/DifferentialGeometry.h>
#include <pmp/algorithms/HoleFilling.h>
#include <Eigen/Dense>
#include "../variables.h"
#include "../lib/nlohmann-json/json.hpp"

#include "eigen_vectors.h"
#include "model_statistics.h"
#include "global_descriptors.h"
#include "descriptors.h"
#include "preprocessor.h"

#include "multimedia_processor.h"

#define FLT_EPSILON 1.19209290E-07F

#if 1
    #define printf_debug printf
#else
    #define printf_debug 0 && printf
#endif