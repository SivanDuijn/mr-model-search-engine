#pragma once

namespace VertexProperties 
{
    struct EigenVectors
    {
        Eigen::Vector3f major;
        Eigen::Vector3f medium;
        Eigen::Vector3f minor;
    };
    struct EigenValues
    {
        float major;
        float medium;
        float minor;
    };

    VertexMap GetVertexMap(pmp::SurfaceMesh &mesh);
    VertexMat RandomVertices(VertexMap &verts);

    EigenValues GetEigenValues(VertexMap &verts);
    EigenVectors GetEigenVectors(VertexMap &verts);
}
