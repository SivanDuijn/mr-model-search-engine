#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <assert.h>

#include <Eigen/Dense>
#include <pmp/algorithms/Decimation.h>
#include <pmp/algorithms/Remeshing.h>
#include <pmp/algorithms/DifferentialGeometry.h>
#include <pmp/algorithms/HoleFilling.h>
#include <pmp/algorithms/Subdivision.h>
#include <pmp/algorithms/Triangulation.h>
#include <pmp/BoundingBox.h>
#include <pmp/SurfaceMesh.h>
#include "../lib/nlohmann-json/json.hpp"
#include "../lib/libgdiam/gdiam.hpp"

// Must be a power of 2!
#define VERTEX_COUNT 2048

#define FLT_EPSILON 1.19209290E-07F
#define FLT_MIN -3.40282e+038
#define FLT_MAX 3.40282e+038

typedef Eigen::Vector3f                       Vertex;
typedef Eigen::Matrix<float, 3, VERTEX_COUNT> VertexMat;
typedef Eigen::Map<VertexMat>                 VertexMap;

#if 1
    #define printf_debug printf
#else
    #define printf_debug 0 && printf
#endif

#include "../variables.h"
#include "database.h"
#include "utils.h"
#include "eigen_vectors.h"
#include "model_statistics.h"
#include "descriptors_global.h"
#include "descriptors_shape.h"
#include "preprocessor.h"
#include "multimedia_processor.h"
