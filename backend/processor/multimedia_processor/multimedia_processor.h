﻿// multimedia_processor.h : Include file for standard system include files,
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

// const char* path = "LabeledDB_new/tcube_long.off"
void preprocess(const char* in = "LabeledDB_new/tcube_long.off", const char* out = "LabeledDB_new/tcube_long_processed.off");
void debug();
void resample(pmp::SurfaceMesh &mesh);
void normalize(pmp::SurfaceMesh &mesh);
