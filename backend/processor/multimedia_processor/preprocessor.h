#pragma once

namespace preprocessor
{
    bool is_resampled(pmp::SurfaceMesh &mesh);
    void resample(pmp::SurfaceMesh &mesh, modelstats::NormalizationStatistics &beforeStats, modelstats::NormalizationStatistics &afterStats);
    void normalize(pmp::SurfaceMesh &mesh, modelstats::NormalizationStatistics &beforeStats, modelstats::NormalizationStatistics &afterStats);
}