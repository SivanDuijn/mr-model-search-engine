#include "preprocessor.h"

using namespace std;

namespace preprocessor
{
    // Resample a mesh at the given path
    void resample(pmp::SurfaceMesh &mesh, modelstats::NormalizationStatistics &beforeStats, modelstats::NormalizationStatistics &afterStats)
    {
        printf_debug("Mesh has %zu vertices\n", mesh.n_vertices());

        // Store stats before resampling
        beforeStats.nVertices = mesh.n_vertices();
        beforeStats.nFaces = mesh.n_faces();

        // Triangulate it if necessary
        if (!mesh.is_triangle_mesh())
        {
            printf_debug("Triangulating mesh\n");
            pmp::Triangulation tri = pmp::Triangulation(mesh);
            tri.triangulate();
        }

        // Subdivide and/or decimate depending on the vertex count
        const unsigned int des = 2500; // example desired vertex count mentioned in technical tips
        while (mesh.n_vertices() < des)
        {
            printf_debug("Subdividing mesh");
            pmp::Subdivision div = pmp::Subdivision(mesh);
            div.loop();
            printf_debug(" to %zu vertices\n", mesh.n_vertices());
        }
        if (mesh.n_vertices() > des)
        {
            printf_debug("Decimating mesh");
            pmp::Decimation dec = pmp::Decimation(mesh);
            dec.decimate(des);
            printf_debug(" to %zu vertices\n", mesh.n_vertices());
        }
        else
        {
            printf_debug("Mesh contains exactly the right amount of vertices!\n");
        }

        // Store stats after resampling
        afterStats.nVertices = mesh.n_vertices();
        afterStats.nFaces = mesh.n_faces();

        printf_debug("Done\n");
    }

    // Normalize a mesh according to 4 steps:
    // * Translate barycenter to origin
    // * Align with coordinate frame
    // * Flip based on moment test
    // * Scale to unit volume
    void normalize(pmp::SurfaceMesh &mesh, modelstats::NormalizationStatistics &beforeStats, modelstats::NormalizationStatistics &afterStats)
    {
        // Map the point list (aka array of Eigen::Matrix3f) to a single Eigen::MatrixXd
        pmp::VertexProperty points = mesh.get_vertex_property<pmp::Point>("v:point");
        Eigen::Map<Eigen::MatrixXf> map((float*)(points.data()), 3, mesh.n_vertices());

        // Translate barycenter to origin
        beforeStats.distBarycenterToOrigin = ((Eigen::Vector3f)pmp::centroid(mesh)).norm();
        printf_debug("Translating barycenter to origin");
        pmp::Point bcenter = pmp::centroid(mesh);
        map.colwise() -= (Eigen::Vector3f)bcenter;
        printf_debug(" (-[%f, %f, %f])\n", bcenter.data()[0], bcenter.data()[1], bcenter.data()[2]);
        afterStats.distBarycenterToOrigin = ((Eigen::Vector3f)pmp::centroid(mesh)).norm();


        // Align with coordinate frame
        printf_debug("Aligning with coordinate frame\n");
        // Compute the covariance matrix
        // https://stackoverflow.com/a/15142446
        // We don't have to center the sample matrix because we just centered the samples :)
        // Get the eigenvectors
        Eigen::Matrix3f cov = (map * map.adjoint()) / float(map.cols() - 1);
        auto eigenVs = utils::GetMajorMinorEigenVectors(cov);
        Eigen::Vector3f	major = get<0>(eigenVs);
        Eigen::Vector3f minor = get<1>(eigenVs);
        Eigen::Vector3f cross = get<2>(eigenVs);
        // Create the inverse rotation matrix by transposing the eigenvectors to it
        Eigen::Matrix3f rot {
            {major(0), major(1), major(2)},
            {minor(0), minor(1), minor(2)},
            {cross(0), cross(1), cross(2)},
        };
        // Store stats before rotating
        beforeStats.angleX = pmp::angle(major, pmp::Point(1,0,0));
        beforeStats.angleY = pmp::angle(minor, pmp::Point(0,1,0));
        beforeStats.angleZ = pmp::angle(cross, pmp::Point(0,0,1));
        beforeStats.totalAngle = beforeStats.angleX + beforeStats.angleY + beforeStats.angleZ;
        // Rotate the model
        for (auto v : mesh.vertices()) points[v] = rot * (Eigen::Vector3f)(points[v]);
        // and store stats after rotating
        cov = (map * map.adjoint()) / float(map.cols() - 1);
        eigenVs = utils::GetMajorMinorEigenVectors(cov);
        afterStats.angleX = pmp::angle(get<0>(eigenVs), pmp::Point(1,0,0));
        afterStats.angleY = pmp::angle(get<1>(eigenVs), pmp::Point(0,1,0));
        afterStats.angleZ = pmp::angle(get<2>(eigenVs), pmp::Point(0,0,1));
        afterStats.totalAngle = afterStats.angleX + afterStats.angleY + afterStats.angleZ;

        // Flip based on moment tests
        printf_debug("Flipping based on moment test");
        Eigen::Vector3f flip = map.cwiseSign().cwiseProduct(map.cwiseProduct(map)).rowwise().sum().cwiseSign()
            .unaryExpr([](float e) -> float { return e == 0.f ? 1.f : e; }); // Change all 0s to 1s
        // Store before stats
        beforeStats.totalFlip = flip.sum();
        printf_debug(" [%f, %f, %f]\n", flip(0), flip(1), flip(2));
        for (auto v : mesh.vertices()) points[v] = ((Eigen::Vector3f)points[v]).cwiseProduct(flip); // TODO Eigen calculations
        // Store after stats
        Eigen::Vector3f afterFlip = map.cwiseSign().cwiseProduct(map.cwiseProduct(map)).rowwise().sum().cwiseSign()
            .unaryExpr([](float e) -> float { return e == 0.f ? 1.f : e; });
        afterStats.totalFlip = afterFlip.sum();

        // Scale to unit volume
        beforeStats.boundingBoxSize = mesh.bounds().size();
        printf_debug("Scaling to unit volume");
        Eigen::MatrixXf bounds = Eigen::MatrixXf(3, 2);
        bounds << (Eigen::Vector3f)(mesh.bounds().min()), (Eigen::Vector3f)(mesh.bounds().max());
        float max = bounds.cwiseAbs().maxCoeff();
        float scale = 0.5f / max;
        map *= scale;
        printf_debug(" (%f => *%f)\n", max, scale);
        afterStats.boundingBoxSize = mesh.bounds().size();

        // If the mesh was flipped once or thrice we need to invert the normals
        int flipSum = flip.sum();
        if (flipSum == 1 || flipSum == -3)
        {
            pmp::SurfaceMesh flippedMesh;
            for (auto v : mesh.vertices())
                flippedMesh.add_vertex(mesh.position(v));
            for (auto f : mesh.faces())
            {
                vector<pmp::Vertex> vertices;
                for (auto v : mesh.vertices(f))
                    vertices.push_back(v);
                reverse(vertices.begin(), vertices.end());
                flippedMesh.add_face(vertices);
            }
            mesh = flippedMesh;
        }
        
        printf_debug("Done\n");
    }

}