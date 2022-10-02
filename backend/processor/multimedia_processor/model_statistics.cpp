#include "model_statistics.h"

NormalizationStatistics CalculateNormalizationStats(pmp::SurfaceMesh &mesh)
{
    const int nVertices = mesh.n_vertices();
    const int nFaces = mesh.n_faces();

    auto aabb = mesh.bounds();
    
    const float boundingBoxSize = mesh.bounds().size();

    // const pmp::Point barycenter = ((Eigen::Vector3f)pmp::centroid(mesh)).norm();
    const float distBarycenterToOrigin = ((Eigen::Vector3f)pmp::centroid(mesh)).norm();

	pmp::VertexProperty points = mesh.get_vertex_property<pmp::Point>("v:point");
	Eigen::Map<Eigen::MatrixXf> map((float*)(points.data()), 3, mesh.n_vertices());

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

    // Rotation between major eigenvector and x axis;
    double x1 = minor(0);
    double y1 = minor(1);
    double z1 = minor(2);
    double x2 = 0;
    double y2 = 1;
    double z2 = 0;
    double dot = x1 * x2 + y1 * y2 + z1 * z2;
    double cross_x = (y1 * z2 - z1 * y2);
    double cross_y = (z1 * x2 - x1 * z2);
    double cross_z = (x1 * y2 - y1 * x2);
    double det = std::sqrt(cross_x * cross_x + cross_y * cross_y + cross_z * cross_z);
    float angle = std::atan2f(det, dot);
    printf("eigens: ([%f, %f, %f] / [%f, %f, %f] / [%f, %f, %f])\n", major(0), major(1), major(2), minor(0), minor(1), minor(2), cross(0), cross(1), cross(2));


    NormalizationStatistics stats = {
        nVertices,
        nFaces,
        boundingBoxSize,
        distBarycenterToOrigin,
        angle,
        5,
        6,
    };


    return stats;
}