#pragma once

#include <string>
#include <pmp/SurfaceMesh.h>
#include <pmp/BoundingBox.h>
#include <pmp/algorithms/Triangulation.h>
#include <pmp/algorithms/Subdivision.h>
#include <pmp/algorithms/Decimation.h>
#include <pmp/algorithms/DifferentialGeometry.h>
#include "model_statistics.h"
#include "utils.h"


namespace preprocessor
{
    void resample(pmp::SurfaceMesh &mesh, modelstats::NormalizationStatistics &beforeStats, modelstats::NormalizationStatistics &afterStats, const bool debug = false);
    void normalize(pmp::SurfaceMesh &mesh, modelstats::NormalizationStatistics &beforeStats, modelstats::NormalizationStatistics &afterStats, const bool debug = false);
}