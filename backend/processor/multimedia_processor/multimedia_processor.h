// multimedia_processor.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <string.h>
#include <stdio.h>
#include <fstream>
#include <functional>
#include <pmp/SurfaceMesh.h>
#include <pmp/SurfaceMeshIO.h>
#include <pmp/BoundingBox.h>
#include <pmp/algorithms/Triangulation.h>
#include <pmp/algorithms/Subdivision.h>
#include <pmp/algorithms/Decimation.h>
#include "pmp/algorithms/DifferentialGeometry.h"
//#include "pmp/visualization/MeshViewer.h"
#include "../variables.h"
#include "model_statistics.h"
#include "../lib/nlohmann-json/json.hpp"

using namespace std;

void preprocess(const string database = "PSBDatabase", const string in = "125.off", const string out = "125_processed.off", const bool debug = true );
void preprocessAll(const string database = "PSBDatabase");
void debug();
void resample(pmp::SurfaceMesh &mesh, const bool debug = true);
void normalize(pmp::SurfaceMesh &mesh, const bool debug = true);
