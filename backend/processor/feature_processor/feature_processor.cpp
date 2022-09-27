#include "feature_processor.h"

using namespace std;

int main(int argc, char *argv[])
{
    ifstream ifs("../database_class_data.csv");
    string row;
    string modelname;


    getline(ifs, row);
    
    int i = 0;
    // for (i = 0; i < 200; i++)
    // {
    //     getline(ifs, row);
    // }
    
    string newCSV = row + "\n";

    while (getline(ifs, row)) {
        newCSV += row;

        istringstream iss(row);
        if (getline(iss, modelname, ',')) {
            cout << i << " " << modelname;
            try
            {
                pmp::SurfaceMesh mesh;

                mesh.read("../../frontend/model-viewer/public/models/" + modelname + ".obj");
                cout << " success" << endl;
            }
            catch(const std::exception& e)
            {
                cout << " fail" << endl;
            }

            // newCSV += "," + to_string(mesh.n_vertices()) + "," + to_string(mesh.n_faces());
        }

    
        i++;
    }

    cout << newCSV << endl;

    ofstream out("../database_mesh_data.csv");
    out << newCSV;
    out.close();

	return 0;
}