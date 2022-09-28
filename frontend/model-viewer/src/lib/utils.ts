import * as THREE from "three";

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
