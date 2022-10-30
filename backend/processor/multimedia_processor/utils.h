#pragma once

namespace utils
{
    unsigned int RandomUInt();
    float EarthMoversDistance(Eigen::VectorXf a, Eigen::VectorXf b);
    Eigen::VectorXf JSONArrayToVector(nlohmann::json jsonArray);
}
