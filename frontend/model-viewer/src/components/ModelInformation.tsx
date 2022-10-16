import clsx from "clsx";
import { useModelStats } from "src/lib/contexts/hooks";

type Props = {
    className?: string;
};

export default function ModelInformation(props: Props) {
    const { stats } = useModelStats();

    const rows = [
        { label: "Class", value: stats?.className },
        { label: "# Vertices", value: stats?.nVertices },
        { label: "# Faces", value: stats?.nFaces },
        { label: "Dist to center", value: Number(stats?.distBarycenterToOrigin?.toFixed(5)) },
        { label: "AABB size", value: Number(stats?.boundingBoxSize?.toFixed(5)) },
        { label: "Angle x-axis", value: Number(stats?.angleX?.toFixed(5)) },
        { label: "Angle y-axis", value: Number(stats?.angleY?.toFixed(5)) },
        { label: "Angle z-axis", value: Number(stats?.angleZ?.toFixed(5)) },
        { label: "Total angle", value: Number(stats?.totalAngle?.toFixed(5)) },
        { label: "Total flip", value: Number(stats?.totalFlip?.toFixed(5)) },
    ];

    return (
        <div className={props.className}>
            <p className={clsx("border-b-2", "text-center", "font-bold")}>Model Information</p>
            <table className={clsx("p-2", "ml-2")}>
                <tbody>
                    {rows.map((row) => (
                        <tr key={row.label} className="ml-2">
                            <td>{row.label}</td>
                            <td className={clsx("text-green-500", "pl-4")}>
                                {row.value != undefined &&
                                ((typeof row.value == "number" && !isNaN(row.value)) ||
                                    typeof row.value == "string")
                                    ? row.value
                                    : ""}
                            </td>
                        </tr>
                    ))}
                </tbody>
            </table>
        </div>
    );
}
