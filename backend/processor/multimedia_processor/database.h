#pragma once

namespace database 
{
    // Read a mesh from a database
    pmp::SurfaceMesh read_mesh(const std::string database, const std::string file);

    // Write a mesh to a database
    void write_mesh(pmp::SurfaceMesh &mesh, const std::string database, const std::string file);

	std::vector<std::string> get_filenames(const std::string database);
}