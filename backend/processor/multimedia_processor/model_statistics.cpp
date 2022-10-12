#include "headers.h"

using namespace std;

namespace modelstats
{
    void WriteNormalizationStatistics(string database, string in, string out, const NormalizationStatistics &beforeStats, const NormalizationStatistics &afterStats)
    {
        // Write the normalizationStats to json
        ifstream ifs(vars::GetAssetPath(database + "/normalizationStats.json"));
        nlohmann::json normStats = nlohmann::json::parse(ifs);
        normStats[in] = { 
            {"nVertices", beforeStats.nVertices}, 
            {"nFaces", beforeStats.nFaces},
            {"aabbSize", beforeStats.boundingBoxSize},
            {"position", beforeStats.distBarycenterToOrigin},
            {"angleX", beforeStats.angleX},
            {"angleY", beforeStats.angleY},
            {"angleZ", beforeStats.angleZ},
            {"totalAngle", beforeStats.totalAngle},
            {"totalFlip", beforeStats.totalFlip}
        };
        normStats[out] = { 
            {"nVertices", afterStats.nVertices}, 
            {"nFaces", afterStats.nFaces},
            {"aabbSize", afterStats.boundingBoxSize},
            {"position", afterStats.distBarycenterToOrigin},
            {"angleX", afterStats.angleX},
            {"angleY", afterStats.angleY},
            {"angleZ", afterStats.angleZ},
            {"totalAngle", afterStats.totalAngle},
            {"totalFlip", afterStats.totalFlip}
        };
        ofstream ofs(vars::GetAssetPath(database + "/normalizationStats.json"));
        ofs << setw(4) << normStats << endl;
    }
}