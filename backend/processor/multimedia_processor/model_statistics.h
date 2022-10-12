#pragma once

namespace modelstats
{
    struct NormalizationStatistics
    {
        // number of sampling points
        int nVertices;
        int nFaces;
        // size of bounding box, computed via its diagonal
        float boundingBoxSize;
        // positionof bounding box
        float distBarycenterToOrigin;
        // pose
        float angleX; // absolute value of cosine of angle between major eigenvector and the X axis
        float angleY; // absolute value of cosine of angle between minor eigenvector and the Y axis
        float angleZ; // absolute value of cosine of angle between cross(major and minor eigenvector) and the Z axis
        float totalAngle;
        // mirrored?
        int totalFlip; // The summed up flip coefficients (which are between either -1 or 1) 
    };

    void WriteNormalizationStatistics(std::string database, std::string in, std::string out, const NormalizationStatistics &beforeStats, const NormalizationStatistics &afterStats);
}
