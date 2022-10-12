#include "headers.h"

namespace global_descriptors
{
    float CalcSurfaceArea(pmp::SurfaceMesh &mesh)
    {
        pmp::VertexProperty<pmp::Point> points = mesh.get_vertex_property<pmp::Point>("v:point");

        float area = 0;
        for (pmp::Face f : mesh.faces())
        {
            std::vector<Eigen::Vector3f> tPts;
            for (pmp::Vertex v : mesh.vertices(f))
                tPts.push_back(Eigen::Vector3f(points[v]));
            if (tPts.size() >= 3)
                area += (tPts[1] - tPts[0]).cross(tPts[2] - tPts[1]).norm() / 2;
        }

        return area;
    }
}