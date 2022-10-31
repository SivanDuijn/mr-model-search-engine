import * as THREE from "three";
import { RenderMaterial } from "src/components/model-viewer/viewGL";

export function GetModelfiletype(file: string): "OFF" | "OBJ" | null {
    const words = file.split(".");
    const type = words[words.length - 1].toUpperCase();
    if (type === "OFF") return "OFF";
    if (type === "OBJ") return "OBJ";
    else return null;
}

export function CreateThreeLineBox(width: number, height: number, depth: number, color = 0xff0000) {
    const geometry = new THREE.BoxGeometry(width, height, depth);
    const edges = new THREE.EdgesGeometry(geometry);
    return new THREE.LineSegments(edges, new THREE.LineBasicMaterial({ color }));
}

// Gets the render material as enum from a string
export function getRenderMaterial(material?: string): RenderMaterial | undefined {
    switch (material) {
        case "flat":
            return RenderMaterial.Flat;
        case "phong":
            return RenderMaterial.Phong;
        case "normals":
            return RenderMaterial.Normals;
        case "cartoon":
            return RenderMaterial.Cartoon;
        case "pointcloud":
            return RenderMaterial.PointCloud;
        case "wireframe":
            return RenderMaterial.WireframeOnly;
        default:
            return undefined;
    }
}

export function getURLVariableBool(v: string | string[] | undefined): boolean | undefined {
    return v === undefined ? undefined : v === "1" ? true : false;
}
