#include "model_statistics.h"

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

    // Rotation between major eigenvector and x axis;
    // dot = x1*x2 + y1*y2 + z1*z2    #between [x1, y1, z1] and [x2, y2, z2]
    // lenSq1 = x1*x1 + y1*y1 + z1*z1
    // lenSq2 = x2*x2 + y2*y2 + z2*z2
    // angle = acos(dot/sqrt(lenSq1 * lenSq2))

    Eigen::Vector3f xAxis(1,0,0);
    Eigen::Vector3f yAxis(0,1,0);
    Eigen::Vector3f zAxis(0,0,1);
    float angleX = std::atan2f(major.cross(xAxis).norm(), major.dot(xAxis));
    float angleY = std::atan2f(minor.cross(yAxis).norm(), minor.dot(yAxis));
    float angleZ = std::atan2f(cross.cross(zAxis).norm(), cross.dot(zAxis));
    if (angleX - M_PI < 0.0001 && angleX - M_PI > -0.0001) angleX = angleX - M_PI;
    if (angleY - M_PI < 0.0001 && angleY - M_PI > -0.0001) angleY = angleY - M_PI;
    if (angleZ - M_PI < 0.0001 && angleZ - M_PI > -0.0001) angleZ = angleZ - M_PI;

    printf("eigens: ([%f, %f, %f] / [%f, %f, %f] / [%f, %f, %f])\n", major(0), major(1), major(2), minor(0), minor(1), minor(2), cross(0), cross(1), cross(2));


    NormalizationStatistics stats = {
        nVertices,
        nFaces,
        boundingBoxSize,
        distBarycenterToOrigin,
        angleX,
        angleY,
        angleZ,
    };


    return stats;
}