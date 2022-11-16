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

    // Calculate eccentricity based on the OBB longest and smallest axis
    // THIS IS ALTERNATIVE TO HOW IT IS DONE ON THE SLIDES
    float eccentricity(pmp::SurfaceMesh &mesh)
    {
        printf_debug("Calculating eccentricity... ");
        // VertexMap map = VertexProperties::GetVertexMap(mesh);
        // VertexProperties::EigenValues values = VertexProperties::GetEigenValues(map);

        // cout << '\n' << values.major << '\n' << values.medium << '\n' << values.minor << endl;
        // float eccentricity = values.major / values.minor;

        auto bounds = mesh.bounds();
        auto diff = static_cast<Eigen::Matrix<float, 3, 1>>(bounds.max() - bounds.min());
        float min_c = diff.minCoeff();
        float max_c = diff.maxCoeff();
        float eccentricity = max_c / min_c;

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
    
    void get_global_descriptors(vector<string>& filenames, vector<GlobalDescriptors> &descriptors)
    {
        for (string file : filenames)
        {
            pmp::SurfaceMesh mesh = Database::ReadMeshFromDatabase(file);
            descriptors.push_back(get_global_descriptors(mesh));
        }
    }

    void standardize(const std::vector<GlobalDescriptors>& descriptors, Eigen::MatrixXf& standardized_fvs, Eigen::VectorXf& mean, Eigen::VectorXf& sd)
    {
        size_t n_models = descriptors.size();

        Eigen::MatrixXf global_fvs = Eigen::MatrixXf(n_models, N_GLOBAL_FEATURES);

        for (int i = 0; i < n_models; i++)
        {
            GlobalDescriptors gd = descriptors[i];
            global_fvs.row(i) = Eigen::Matrix<float, N_GLOBAL_FEATURES, 1>(
                // gd.surfaceArea,
                // gd.AABBVolume,
                // gd.volume,
                // gd.compactness,
                gd.eccentricity,
                gd.diameter,
                gd.sphericity,
                gd.rectangularity
            );
        }

        // Standardize global descriptors using the standard deviation
        mean = global_fvs.colwise().mean();
        sd = ((global_fvs.rowwise() - mean.transpose()).array().square().colwise().sum() / (n_models - 1)).sqrt();
        standardized_fvs = (global_fvs.rowwise() - mean.transpose()).array().rowwise() / sd.transpose().array();
    }
}
