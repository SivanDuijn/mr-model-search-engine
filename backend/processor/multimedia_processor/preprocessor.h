#pragma once

namespace preprocessor
{
    bool is_resampled(pmp::SurfaceMesh &mesh);
    void resample(pmp::SurfaceMesh &mesh, database::NormalizationStatistics &beforeStats, database::NormalizationStatistics &afterStats);
    void normalize(pmp::SurfaceMesh &mesh, database::NormalizationStatistics &beforeStats, database::NormalizationStatistics &afterStats);
}