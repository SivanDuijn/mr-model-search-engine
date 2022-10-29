#pragma once

namespace utils
{
    VertexMat RandomVertices(VertexMap &verts);
    VertexMap GetVertexMap(pmp::SurfaceMesh &mesh);
    float EarthMoversDistance(Eigen::VectorXf a, Eigen::VectorXf b);
}
