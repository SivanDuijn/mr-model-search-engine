#pragma once

class Database 
{
protected:
    Database() {}

    static std::string database_;

    static std::vector<std::string> filenames_;
    static std::vector<std::string> p_filenames_;
    static nlohmann::json classes_;
    static Eigen::MatrixXf global_fvs_;
    static std::vector<Eigen::MatrixXf> shape_fvs_;

    // Load both the global and shape feature vectors at once, so we only have to read the json once! 
    static void LoadFVS();

public:
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

    static void SetDatabase(const std::string database);
    static std::string GetDatabase();
    static std::vector<std::string>& GetFilenames(bool processed = false);
    static std::string GetClass(const std::string);
    // The global descriptors in a matrix where each row is model
    static Eigen::MatrixXf& GetGlobalFVS();
    // The shape descriptors, for each descriptor a matrix where each row represents a model 
    static std::vector<Eigen::MatrixXf>& GetShapeFVS();

    // Read a mesh from file
    static pmp::SurfaceMesh ReadMesh(const std::string file);
    // Read a mesh from a database
    static pmp::SurfaceMesh ReadMeshFromDatabase(const std::string file);
    // Write a mesh to a database
    static void WriteMesh(pmp::SurfaceMesh &mesh, const std::string file);

    // Write normalization stats to a database
    static void WriteStats(std::string in, std::string out, const NormalizationStatistics &beforeStats, const NormalizationStatistics &afterStats);
    static void WriteDescriptors(std::vector<std::string> filenames, std::vector<descriptors::GlobalDescriptors> gds, std::vector<descriptors::ShapeDescriptors> sds);
    static void WriteConfusionMatrix(const std::map<std::string, std::map<std::string, int>> confusion);

    static nlohmann::json StatsToJSON(const NormalizationStatistics &stats);
    static nlohmann::json DescriptorsToJSON(descriptors::GlobalDescriptors &gd, descriptors::ShapeDescriptors &sd);
};
