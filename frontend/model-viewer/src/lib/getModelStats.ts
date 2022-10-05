import modelClasses from "public/PSBDatabase/classes.json";
import modelNormStats from "public/PSBDatabase/normalizationStats.json";

export interface ModelStats {
    className?: string;
    nFaces?: number;
    nVertices?: number;
    boundingBoxSize?: number;
    distBarycenterToOrigin?: number;
    angleX?: number;
    angleY?: number;
    angleZ?: number;
    totalAngle?: number;
    totalFlip?: number;
}

export default function GetModelStats(
    modelFileName: string,
    mesh: THREE.Mesh,
): ModelStats | undefined {
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    const normStats = (modelNormStats as any)[modelFileName];
    modelFileName = modelFileName.replace("_processed", "");
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    const modelClass = (modelClasses as any)[modelFileName];
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
