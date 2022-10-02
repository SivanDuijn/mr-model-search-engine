// multimedia_processor.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <string.h>
#include <stdio.h>
#include <functional>
#include <pmp/SurfaceMesh.h>
#include <pmp/BoundingBox.h>
#include <pmp/algorithms/Triangulation.h>
#include <pmp/algorithms/Subdivision.h>
#include <pmp/algorithms/Decimation.h>
#include "pmp/algorithms/DifferentialGeometry.h"
//#include "pmp/visualization/MeshViewer.h"
#include "../variables.h"
#include "model_statistics.h"

void preprocess(const char* in = "PSBModels/125.off", const char* out = "PSBModels/125_processed.off", const bool enableCalcNormalizationStats = false );
void debug();
void resample(pmp::SurfaceMesh &mesh);
void normalize(pmp::SurfaceMesh &mesh);
