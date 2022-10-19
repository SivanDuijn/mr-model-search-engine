#include "headers.h"

namespace descriptors
{
    float surface_area(pmp::SurfaceMesh &mesh)
    {
        printf_debug("Calculating surface area... ");
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

        printf_debug("%f\n", area);
        return area;
    }

    float compactness(pmp::SurfaceMesh &mesh)
    {
        return compactness(surface_area(mesh), volume(mesh));
    }
    float compactness(float surfaceArea, float volume)
    {
        printf_debug("Calculating compactness...  ");

        float compactness = (surfaceArea * surfaceArea * surfaceArea) / (36*M_PI*volume*volume);
        printf_debug("%f\n", compactness);
        return compactness;
    }

    float aabb_volume(pmp::SurfaceMesh &mesh)
    {
        printf_debug("Calculating AABB volume...  ");
        pmp::BoundingBox bounds = mesh.bounds();

        pmp::Point diff = bounds.max() - bounds.min();
        float volume = diff[0] * diff[1] * diff[2];

        printf_debug("%f\n", volume);
        return volume;
    }

    float volume(pmp::SurfaceMesh &mesh)
    {
        printf_debug("Calculating volume...\n");
        printf_debug("  Filling holes...\n");
        // Fill holes, i think it's not necessary for PSBDatabase though
        int nBoundaryHalfEdges = 1;
        while (nBoundaryHalfEdges > 0)
        {
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
        }

        pmp::VertexProperty<pmp::Point> points = mesh.get_vertex_property<pmp::Point>("v:point");

        float volume = 0;
        for (pmp::Face f : mesh.faces())
        {
            std::vector<Eigen::Vector3f> tPts;
            for (pmp::Vertex v : mesh.vertices(f))
                tPts.push_back(Eigen::Vector3f(points[v]));
            
            volume += tPts[0].cross(tPts[1]).dot(tPts[2]);
        }

        volume = abs(volume) / 6;

        printf_debug("  Volume:                   %f\n", volume);
        return volume;
    }

    float diameter(pmp::SurfaceMesh &mesh)
    {
        printf_debug("Calculating diameter...     ");
        pmp::VertexProperty points = mesh.get_vertex_property<pmp::Point>("v:point");

        // Using code from Sariel Har-Peled, https://sarielhp.org/research/papers/00/diameter/diam_prog.html
        gdiam::GPointPair pair = gdiam::gdiam_approx_diam_pair((float*)points.data(), mesh.n_vertices(), 0.0);

        printf_debug("%f\n", pair.distance);
        return pair.distance;
    }

    float eccentricity(pmp::SurfaceMesh &mesh)
    {
        printf_debug("Calculating eccentricity... ");
        VertexMap map = utils::GetVertexMap(mesh);
        Eigen::Matrix3f cov = (map * map.transpose()) / float(map.cols() - 1);
        Eigen::EigenSolver<Eigen::MatrixXf> solver;
        solver.compute(cov);

        Eigen::Vector3f eigenValues = solver.eigenvalues().real();
        std::tuple<int, int, int> indices = eigen_vectors::GetMajorMinorIndexEigenValues(eigenValues);

        float eccentricity = eigenValues[std::get<0>(indices)] / eigenValues[std::get<2>(indices)];

        printf_debug("%f\n", eccentricity);
        return eccentricity;
    }

    GlobalDescriptors get_global_descriptors(pmp::SurfaceMesh &mesh)
    {
        GlobalDescriptors gd =
        {
            surface_area(mesh),
            0.0f,
            aabb_volume(mesh),
            volume(mesh),
            diameter(mesh),
            eccentricity(mesh),
            0.0f,
            0.0f
        };
        gd.compactness = compactness(gd.surfaceArea, gd.volume);
        gd.sphericity = 1 / gd.compactness;
		gd.rectangularity = gd.volume / gd.AABBVolume;
        return gd;
    }
    void get_global_descriptors(string database, vector<string>& filenames, vector<GlobalDescriptors> &descriptors)
    {
        for (string file : filenames)
        {
            pmp::SurfaceMesh mesh = database::read_mesh(database, file);

            GlobalDescriptors gd =
            {
                surface_area(mesh),
                0.0f,
                aabb_volume(mesh),
                volume(mesh),
                diameter(mesh),
                eccentricity(mesh),
                0.0f,
                0.0f
            };
            gd.compactness = compactness(gd.surfaceArea, gd.volume);
            gd.sphericity = 1 / gd.compactness;
            gd.rectangularity = gd.volume / gd.AABBVolume;

            descriptors.push_back(gd);
        }
    }
}