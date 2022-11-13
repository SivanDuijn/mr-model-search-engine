#pragma once

namespace utils
{
    unsigned int RandomUInt();
    float EarthMoversDistance(Eigen::VectorXf a, Eigen::VectorXf b);
    Eigen::VectorXf json_array_to_vector(nlohmann::json jsonArray);
    std::string mesh_to_off_string(const pmp::SurfaceMesh &mesh);
    std::string to_processed(const std::string unProcessedFileName);
}

