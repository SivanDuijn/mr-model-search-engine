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
                area += (tPts[1] - tPts[0]).cross(tPts[2] - tPts[0]).norm() / 2;
        }

        return area;
    }

    float CalcAABBVolume(pmp::SurfaceMesh &mesh)
    {
        pmp::BoundingBox bounds = mesh.bounds();

        pmp::Point diff = bounds.max() - bounds.min();

        return diff[0] * diff[1] * diff[2];
    }

    float CalcVolume(pmp::SurfaceMesh &mesh)
    {
        int nBoundaryHalfEdges = 1;
        while (nBoundaryHalfEdges > 0) {
            for (pmp::Halfedge he : mesh.halfedges())
            {
                if (mesh.is_boundary(he))
                {
                    pmp::HoleFilling hf = pmp::HoleFilling(mesh);
                    hf.fill_hole(he);
                    break;
                }
            }

            nBoundaryHalfEdges = 0;
            for (pmp::Halfedge he : mesh.halfedges())
                if (mesh.is_boundary(he))
                    nBoundaryHalfEdges++;
                    
            cout << nBoundaryHalfEdges << endl;
        }
    }

    float CalcEccentricity(pmp::SurfaceMesh &mesh)
    {
        Eigen::Map<Eigen::MatrixXf> map = eigen_vectors::GetVertexMap(mesh);
        Eigen::Matrix3f cov = (map * map.transpose()) / float(map.cols() - 1);
        Eigen::EigenSolver<Eigen::MatrixXf> solver;
        solver.compute(cov);

        Eigen::Vector3f eigenValues = solver.eigenvalues().real();
        std::tuple<int,int> indices = eigen_vectors::GetMajorMinorIndexEigenValues(eigenValues);
        
        return eigenValues[std::get<0>(indices)] / eigenValues[std::get<1>(indices)];
    }
}