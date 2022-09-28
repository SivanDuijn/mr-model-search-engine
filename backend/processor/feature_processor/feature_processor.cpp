#include "feature_processor.h"

using namespace std;

int main(int argc, char *argv[])
{
    ifstream ifs(vars::GetAssetPath() + "classes.csv");
    string row;
    string modelname;
    string classname;

    getline(ifs, row);
    
    int i = 0;
    
    string newCSV = row + ",# Vertices,# Faces,AABB minX,AABB minY,AABB minZ,AABB maxX,AABB maxY,AABB maxZ,Face type\n";

    while (getline(ifs, row)) {
        newCSV += row;

        istringstream iss(row);
        if (getline(iss, modelname, ',') && getline(iss, classname, ',')) {
            pmp::SurfaceMesh mesh;
            mesh.read(vars::GetAssetPath() + "LabeledDB_new/" + classname + "/" + modelname);
            pmp::BoundingBox aabb = mesh.bounds();
            pmp::Point min = aabb.min();
            pmp::Point max = aabb.max();
            bool isTriangleMesh = mesh.is_triangle_mesh();
            bool isQuadMesh = mesh.is_quad_mesh();
            string faceType = isTriangleMesh ? "Triangles" : isQuadMesh ? "Quads" : "Triangles & Quads";

            newCSV += "," + to_string(mesh.n_vertices()) + "," + to_string(mesh.n_faces()) + 
                "," + to_string(min[0]) + "," + to_string(min[1]) + "," + to_string(min[2]) +
                "," + to_string(max[0]) + "," + to_string(max[1]) + "," + to_string(max[2]) + "," + faceType + "\n";
        }
        i++;
    }

    cout << newCSV << endl;

    ofstream out(vars::GetAssetPath() + "classes_info.csv");
    out << newCSV;
    out.close();

	return 0;
}