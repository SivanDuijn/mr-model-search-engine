#include "feature_processor.h"

using namespace std;

int main(int argc, char *argv[])
{
    ifstream ifs("../database_class_data.csv");
    string row;
    string modelname;


    getline(ifs, row);
    string newCSV = row + "\n";

    int i = 0;
    while (getline(ifs, row)) {
        newCSV += row;

        istringstream iss(row);
        if (getline(iss, modelname, ',')) {
            cout << i << " " << modelname << endl;
            pmp::SurfaceMesh mesh;

            mesh.read("../../frontend/model-viewer/public/models/" + modelname + ".obj");

            newCSV += "," + to_string(mesh.n_vertices()) + "," + to_string(mesh.n_faces()) + "\n";
            cout << newCSV << endl;
        }

    
        i++;
    }

    cout << newCSV << endl;

    ofstream out("../database_mesh_data.csv");
    out << newCSV;
    out.close();

	return 0;
}