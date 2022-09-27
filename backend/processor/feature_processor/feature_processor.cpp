#include "feature_processor.h"

using namespace std;

int main(int argc, char *argv[])
{
    ifstream ifs("../../frontend/model-viewer/public/classes.csv");
    string row;
    string modelname;
    string classname;

    getline(ifs, row);
    
    int i = 0;
    
    string newCSV = row + ",# Vertices,# Faces,AABB minX,AABB minY,AABB maxX,AABB maxY\n";

    while (getline(ifs, row)) {
        newCSV += row;

        istringstream iss(row);
        if (getline(iss, modelname, ',') && getline(iss, classname, ',')) {
            // cout << i << " " << modelname << " " << classname;
            pmp::SurfaceMesh mesh;
            mesh.read("../../frontend/model-viewer/public/LabeledDB_new/" + classname + "/" + modelname);
            pmp::BoundingBox aabb = mesh.bounds();
            pmp::Point min = aabb.min();
            pmp::Point max = aabb.max();
            // cout << " success" << endl;

            newCSV += "," + to_string(mesh.n_vertices()) + "," + to_string(mesh.n_faces()) + 
                "," + to_string(min[0]) + "," + to_string(min[1]) +
                "," + to_string(max[0]) + "," + to_string(max[1]) + "\n";
        }
        i++;
    }

    cout << newCSV << endl;

    ofstream out("../../frontend/model-viewer/public/classes_info.csv");
    out << newCSV;
    out.close();

	return 0;
}