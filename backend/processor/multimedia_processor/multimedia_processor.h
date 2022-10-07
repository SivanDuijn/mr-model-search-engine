// multimedia_processor.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <string.h>
#include <stdio.h>
#include <fstream>
#include <pmp/SurfaceMesh.h>
#include "../variables.h"
#include "../lib/nlohmann-json/json.hpp"
#include "preprocessor.h"
#include "model_statistics.h"

using namespace std;

void debug();
void preprocess(const string database = "PSBDatabase", const string in = "246.off", const string out = "246_processed.off", const bool debug = false );
void preprocessAll(const string database = "PSBDatabase");
