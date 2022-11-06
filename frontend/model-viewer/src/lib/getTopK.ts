import topClosest from "public/PSBDatabase/closest_models.json";

export default function GetTopK(modelInDatabase: string) {
    return (topClosest as unknown as Record<string, (string | number)[][]>)[modelInDatabase].map(
        (cm) => ({ name: cm[0] as string, dist: cm[1] as number }),
    );
}
