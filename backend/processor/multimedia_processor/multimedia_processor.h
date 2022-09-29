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
#include "../variables.h"
#include "pmp_ext.h"

void debug();
void resample(const char* path = "LabeledDB_new/tcube.off");
void normalize(const char* path = "LabeledDB_new/tcube.off");
