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
	auto v = solver.eigenvalues().real();
	int majorI = 0; 
	int minorI = 0;
	if (v[0] > v[1])
		if (v[0] > v[2])
		{
			majorI = 0;
			if (v[1] > v[2])
				minorI = 1;
			else 
				minorI = 2;
		}
		else
		{
			majorI = 2;
			minorI = 0;
			// 2 > 0 > 1
		} 
	else if (v[1] > v[2])
	{
		// 1 > 0, 1 > 2
		majorI = 1;
		if (v[0] > v[2])
			minorI = 0;
		else 
			minorI = 2;
	}
	else 
	{
		majorI = 2;
		minorI = 1;
		// 2 > 1 > 0
	}

	Eigen::MatrixXf eigen = solver.eigenvectors().real();
	Eigen::Vector3f	major = eigen.col(majorI);
	Eigen::Vector3f minor = eigen.col(minorI);
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