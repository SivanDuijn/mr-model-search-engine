#include "model_statistics.h"

using namespace std;

NormalizationStatistics CalculateNormalizationStats(pmp::SurfaceMesh mesh)
{
    const int nVertices = mesh.n_vertices();
    const int nFaces = mesh.n_faces();

    auto aabb = mesh.bounds();
    
    const float boundingBoxSize = mesh.bounds().size();

    pmp::Point bcenter = pmp::centroid(mesh);
    const float distBarycenterToOrigin = ((Eigen::Vector3f)bcenter).norm();

	pmp::VertexProperty points = mesh.get_vertex_property<pmp::Point>("v:point");
	Eigen::Map<Eigen::MatrixXf> map((float*)(points.data()), 3, mesh.n_vertices());

    // center mesh
    if (distBarycenterToOrigin > 0.0001)
        map.colwise() -= (Eigen::Vector3f)bcenter;
	Eigen::Matrix3f cov = (map * map.adjoint()) / float(map.cols() - 1);
	// Get the eigenvectors
	Eigen::EigenSolver<Eigen::MatrixXf> solver;
	solver.compute(cov);
	Eigen::MatrixXf eigen = solver.eigenvectors().real();
	// Rotate the model
	// TODO optimize code
	Eigen::Vector3f	major = eigen.row(0);
	Eigen::Vector3f minor = eigen.row(1);
	Eigen::Vector3f cross = major.cross(minor);

    float angleX = pmp::angle(major, pmp::Point(1,0,0));
    float angleY = pmp::angle(minor, pmp::Point(0,1,0));
    float angleZ = pmp::angle(cross, pmp::Point(0,0,1));
    float totalAngle = angleX + angleY + angleZ;

    // Moment test
    Eigen::Vector3f flip = map.cwiseSign().cwiseProduct(map.cwiseProduct(map)).rowwise().sum().cwiseSign()
    .unaryExpr([](float e) -> float { return e == 0.f ? 1.f : e; }); // Change all 0s to 1s
    const float totalFlip = flip.sum();

    NormalizationStatistics stats = {
        nVertices,
        nFaces,
        boundingBoxSize,
        distBarycenterToOrigin,
        angleX,
        angleY,
        angleZ,
        totalAngle,
        totalFlip,
    };


    return stats;
}