import ModelInfo from "../data/model-info.json";

export interface ModelStats {
    className: string;
    nFaces: number;
    nVertices: number;
    AABB: { min: { x: number; y: number; z: number }; max: { x: number; y: number; z: number } };
}

export default function GetModelStats(modelFileName: string): ModelStats | undefined {
    if (!(modelFileName in ModelInfo)) return undefined;
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    const info = (ModelInfo as any)[modelFileName];
    return {
        className: info[0],
        nVertices: info[1],
        nFaces: info[2],
        AABB: {
            min: { x: info[3], y: info[4], z: info[5] },
            max: { x: info[6], y: info[7], z: info[8] },
        },
    };
}
