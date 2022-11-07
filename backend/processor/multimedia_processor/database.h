#pragma once

namespace database 
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

    // Read a mesh from file
    pmp::SurfaceMesh read_mesh(const std::string file);
    // Read a mesh from a database
    pmp::SurfaceMesh read_mesh(const std::string database, const std::string file);
    // Write a mesh to a database
    void write_mesh(pmp::SurfaceMesh &mesh, const std::string database, const std::string file);
    // Get the class a file belongs to
    std::string file_class(const std::string database, const std::string file);
    // Write normalization stats to a database
    void write_stats(std::string database, std::string in, std::string out, const NormalizationStatistics &beforeStats, const NormalizationStatistics &afterStats);
    nlohmann::json stats_to_json(const NormalizationStatistics &stats);
    // Write model descriptors to database
    void write_descriptors(std::string database, std::vector<std::string> filenames, std::vector<descriptors::GlobalDescriptors> gds, std::vector<descriptors::ShapeDescriptors> sds);
    nlohmann::json descriptors_to_json(descriptors::GlobalDescriptors &gd, descriptors::ShapeDescriptors &sd);
    // Get the filenames of all models in a database
	std::vector<std::string> get_filenames(const std::string database, const bool processed = false);

    Eigen::MatrixXf read_all_global_fvs(const std::string database, const std::vector<std::string> &filenames);
    std::vector<Eigen::MatrixXf> read_all_shape_fvs(const std::string database, const std::vector<std::string> &filenames);
}