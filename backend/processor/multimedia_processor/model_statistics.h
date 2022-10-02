#pragma once

#include <pmp/SurfaceMesh.h>

struct ModelStatistics
{
    float nVertices;
    float nFaces;
    float distBarycenterToOrigin;
    float boundingBoxVolume;
    float xRotation; // Rotations needed to rotate the model so the local axis align with the world axis
    float yRotation;
    float zRotation;
;}

class StatisticsCalculator
{
    static int Calculate(pmp::SurfaceMesh &mesh);
}