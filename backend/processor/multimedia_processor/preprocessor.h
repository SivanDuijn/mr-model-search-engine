#pragma once

namespace preprocessor
{
    bool is_resampled(pmp::SurfaceMesh &mesh);
    void resample(pmp::SurfaceMesh &mesh, Database::NormalizationStatistics &beforeStats, Database::NormalizationStatistics &afterStats);
    void normalize(pmp::SurfaceMesh &mesh, Database::NormalizationStatistics &beforeStats, Database::NormalizationStatistics &afterStats);
}