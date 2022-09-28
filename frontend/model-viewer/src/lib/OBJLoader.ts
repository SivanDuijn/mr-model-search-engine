import OBJParser from "obj-file-parser-ts";
import * as THREE from "three";
import { OBJLoader } from "three/examples/jsm/loaders/OBJLoader";

/** Parses and loads a .obj model */
export default function LoadOBJModel(model: string): THREE.BufferGeometry {
    const objParser = new OBJParser(model);

    const output = objParser.parse();

    // Collect all models in one mesh
    const vertices: number[] = [];
    const faces: number[] = [];
    output.models.forEach((model) => {
        model.vertices.forEach((v) => vertices.push(v.x, v.y, v.z));

        model.faces.forEach((f) => {
            const vi1 = f.vertices[0].vertexIndex - 1;
            let vi2 = f.vertices[1].vertexIndex - 1;
            let vi3 = f.vertices[2].vertexIndex - 1;

            faces.push(vi1, vi2, vi3);

            if (f.vertices.length == 4) {
                vi2 = vi3;
                vi3 = f.vertices[3].vertexIndex - 1;
                faces.push(vi1, vi2, vi3);
            }
        });
    });

    const geometry = new THREE.BufferGeometry();
    geometry.setIndex(faces);
    geometry.setAttribute("position", new THREE.Float32BufferAttribute(vertices, 3));

    geometry.computeVertexNormals();
    geometry.center();

    return geometry;
}

export function LoadOBJModelWithThreeJS(modelStr: string, mat: THREE.Material): THREE.Group {
    const loader = new OBJLoader();
    const model = loader.parse(modelStr);

    model.traverse((child) => {
        if (child instanceof THREE.Mesh) {
            child.geometry.center();
            child.material = mat;
        }
    });

    return model;
}
