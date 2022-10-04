#pragma once

#include <pmp/SurfaceMesh.h>
#include "pmp/algorithms/DifferentialGeometry.h"
#include <cmath>

struct NormalizationStatistics
{
    // number of sampling points
    int nVertices;
    int nFaces;
    // size of bounding box, computed via its diagonal
    float boundingBoxSize;
    // positionof bounding box
    float distBarycenterToOrigin;
    // pose
    float angleX; // absolute value of cosine of angle between major eigenvector and the X axis
    float angleY; // absolute value of cosine of angle between minor eigenvector and the Y axis
    float angleZ; // absolute value of cosine of angle between cross(major and minor eigenvector) and the Z axis
};


NormalizationStatistics CalculateNormalizationStats(pmp::SurfaceMesh mesh);
