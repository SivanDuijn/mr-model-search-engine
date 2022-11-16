#pragma once

#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <assert.h>

#include <Eigen/Dense>
#include <pmp/algorithms/Decimation.h>
#include <pmp/algorithms/DifferentialGeometry.h>
#include <pmp/algorithms/HoleFilling.h>
#include <pmp/algorithms/Subdivision.h>
#include <pmp/algorithms/Triangulation.h>
#include <pmp/BoundingBox.h>
#include <pmp/SurfaceMesh.h>
#include "../lib/nlohmann-json/json.hpp"
#include "../lib/libgdiam/gdiam.hpp"
#include <annoylib.h>
#include <kissrandom.h>

// Must be a power of 2!
#define VERTEX_COUNT 2048   // 2^11
#define SAMPLE_COUNT 131072 // 2^17

#define EPSILON 1e-5

typedef Annoy::AnnoyIndex<size_t, float, Annoy::Angular, Annoy::Kiss32Random, Annoy::AnnoyIndexSingleThreadedBuildPolicy>
                                              AnnoyIndex;
typedef Eigen::Vector3f                       Vertex;
typedef Eigen::Matrix<float, 3, VERTEX_COUNT> VertexMat;
typedef Eigen::Map<VertexMat>                 VertexMap;

#if 0
    #define printf_debug printf
#else
    #define printf_debug 0 && printf
#endif

// TODO move
template<typename Iterator>
std::vector<size_t> n_smallest_indices(Iterator it, Iterator end, size_t n) {
    struct Element {
        Iterator it;
        size_t index;
    };

    std::vector<Element> top_elements;
    top_elements.reserve(n + 1);

    for(size_t index = 0; it != end; ++index, ++it) {
        top_elements.insert(std::upper_bound(top_elements.begin(), top_elements.end(), *it, [](auto value, auto element){return value < *element.it;}), {it, index});
        if (index >= n)
            top_elements.pop_back();
    }

    std::vector<size_t> result;
    result.reserve(top_elements.size());

    for(auto &element: top_elements)
        result.push_back(element.index);

    return result;
}

#include "utils.h"
#include "distance.h"
#include "descriptors_global.h"
#include "descriptors_shape.h"
#include "database.h"
#include "vertex_properties.h"
#include "preprocessor.h"
#include "multimedia_processor.h"
