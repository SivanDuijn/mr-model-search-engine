import clsx from "clsx";
import { ModelStats } from "src/lib/getModelStats";

export default function ModelInformation(props: { stats?: ModelStats }) {
    const rows = [
        { label: "class", value: props.stats?.className },
        { label: "# vertices", value: props.stats?.nVertices },
        { label: "# faces", value: props.stats?.nFaces },
        { label: "dist to center", value: Number(props.stats?.distBarycenterToOrigin?.toFixed(5)) },
        { label: "AABB size", value: Number(props.stats?.boundingBoxSize?.toFixed(5)) },
        { label: "angle x-axis", value: Number(props.stats?.angleX?.toFixed(5)) },
        { label: "angle y-axis", value: Number(props.stats?.angleY?.toFixed(5)) },
        { label: "angle z-axis", value: Number(props.stats?.angleZ?.toFixed(5)) },
        { label: "total angle", value: Number(props.stats?.totalAngle?.toFixed(5)) },
        { label: "total flip", value: Number(props.stats?.totalFlip?.toFixed(5)) },
    ];

    return (
        <div className={clsx("border-2", "border-slate-200", "mx-2", "mt-4")}>
            <p className={clsx("border-b-2", "text-center", "font-bold")}>Model Information</p>
            <table className={clsx("p-2", "ml-2")}>
                <tbody>
                    {rows.map(
                        (row) =>
                            row.value != undefined && (
                                <tr key={row.label} className="ml-2">
                                    <td>{row.label}</td>
                                    <td className={clsx("text-green-500", "pl-4")}>{row.value}</td>
                                </tr>
                            ),
                    )}
                </tbody>
            </table>
        </div>
    );
}
