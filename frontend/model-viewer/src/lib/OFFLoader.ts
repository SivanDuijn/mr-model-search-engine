import * as THREE from "three";

export default function LoadOFFModel(modelStr: string) {
    const lines = modelStr.split("\n");

    const vertices: number[] = [];
    const faces: number[] = [];

    let atModelInfoLine = false;
    let nVertices = 0;
    let nFaces = 0;
    let vi = -1;
    let fi = -1;
    for (const line of lines) {
        if (line.startsWith("#")) continue;
        const words = line.split(" ").filter((w) => w.length != 0);

        if (!atModelInfoLine && words.length >= 3) {
            nVertices = parseInt(words[0]);
            nFaces = parseInt(words[1]);
            if (!isNaN(nVertices) && !isNaN(nFaces)) {
                atModelInfoLine = true;
                vi = 0;
            }
        } else if (vi >= 0 && vi < nVertices) {
            vertices.push(parseFloat(words[0]), parseFloat(words[1]), parseFloat(words[2]));
            vi++;
        } else if (atModelInfoLine && fi < nFaces) {
            const n = parseInt(words[0]);
            const indices: number[] = [];
            for (let i = 1; i <= n; i++) indices.push(parseInt(words[i]));
            faces.push(...indices);
            fi++;
        }
    }

    const geometry = new THREE.BufferGeometry();
    geometry.setIndex(faces);
    geometry.setAttribute("position", new THREE.Float32BufferAttribute(vertices, 3));

    geometry.computeVertexNormals();

    return geometry;
}
