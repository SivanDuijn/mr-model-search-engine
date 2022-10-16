#pragma once

// TODO remove
#define VERTEX_COUNT 2048
typedef Eigen::Vector3f                       Vertex;
typedef Eigen::Matrix<float, 3, VERTEX_COUNT> VertexMat;
typedef Eigen::Map<VertexMat>                 VertexMap;

namespace utils
{
    unsigned int RandomUInt();
    unsigned int RandomVertexIndex();
    VertexMat RandomVertices(VertexMap &verts);

    VertexMap GetVertexMap(pmp::SurfaceMesh &mesh);
    
    void GetAllFilenamesInDatabase(const std::string database, std::vector<std::string>& filenames);
}