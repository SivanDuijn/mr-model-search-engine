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
#include "../variables.h"

void debug();
void resample(const char* path = "LabeledDB_new/Armadillo/281.off");
void normalize(const char* path = "LabeledDB_new/Armadillo/281.off");

void map(pmp::SurfaceMesh& mesh, void (*func) (pmp::Vertex));
pmp::vec3 vreduce(pmp::SurfaceMesh& mesh, pmp::vec3(*cnv) (pmp::Vertex), pmp::vec3(*red) (pmp::vec3, pmp::vec3), pmp::vec3 bse);
