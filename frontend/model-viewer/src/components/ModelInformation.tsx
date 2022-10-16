import clsx from "clsx";
import { useModel } from "src/lib/contexts/hooks";

type Props = {
    className?: string;
};

export default function ModelInformation(props: Props) {
    const {
        model: { stats },
    } = useModel();

    const rows = [
        { label: "class", value: stats?.className },
        { label: "# vertices", value: stats?.nVertices },
        { label: "# faces", value: stats?.nFaces },
        { label: "dist to center", value: Number(stats?.distBarycenterToOrigin?.toFixed(5)) },
        { label: "AABB size", value: Number(stats?.boundingBoxSize?.toFixed(5)) },
        { label: "angle x-axis", value: Number(stats?.angleX?.toFixed(5)) },
        { label: "angle y-axis", value: Number(stats?.angleY?.toFixed(5)) },
        { label: "angle z-axis", value: Number(stats?.angleZ?.toFixed(5)) },
        { label: "total angle", value: Number(stats?.totalAngle?.toFixed(5)) },
        { label: "total flip", value: Number(stats?.totalFlip?.toFixed(5)) },
    ];

    return (
        <div className={props.className}>
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
