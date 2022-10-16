import modelClasses from "public/PSBDatabase/classes.json";
import modelNormStats from "public/PSBDatabase/normalizationStats.json";
import { ModelState } from "./contexts/reducer";

export default function GetModelStats(
    modelName: string,
    mesh: THREE.Mesh,
): ModelState["model"]["stats"] {
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    const normStats = (modelNormStats as any)[modelName];
    modelName = modelName.replace("_processed", "");
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    const modelClass = (modelClasses as any)[modelName];
    return {
        className: modelClass ? modelClass : undefined,
        nVertices: mesh.geometry.getAttribute("position").count,
        nFaces: mesh.geometry.index ? mesh.geometry.index.count / 3 : undefined,
        ...(normStats
            ? {
                  distBarycenterToOrigin: normStats.position,
                  boundingBoxSize: normStats.aabbSize,
                  angleX: normStats.angleX,
                  angleY: normStats.angleY,
                  angleZ: normStats.angleZ,
                  totalAngle: normStats.totalAngle,
                  totalFlip: normStats.totalFlip,
              }
            : {}),
    };
}
