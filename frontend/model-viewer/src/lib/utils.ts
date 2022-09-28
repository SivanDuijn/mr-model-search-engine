export default function GetModelfiletype(file: string): "OFF" | "OBJ" | null {
    const words = file.split(".");
    const type = words[words.length - 1].toUpperCase();
    if (type === "OFF") return "OFF";
    if (type === "OBJ") return "OBJ";
    else return null;
}
