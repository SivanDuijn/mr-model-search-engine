import clsx from "clsx";
import { ModelStats } from "src/lib/getModelStats";

export default function ModelInformation(props: { stats?: ModelStats }) {
    const rows = [
        { label: "class", value: props.stats?.className },
        { label: "# vertices", value: props.stats?.nVertices },
        { label: "# faces", value: props.stats?.nFaces },
    ];

    return (
        <div className={clsx("border-2", "border-slate-200", "mx-2", "mt-4")}>
            <p className={clsx("border-b-2", "text-center", "font-bold")}>Model Information</p>
            <table className={clsx("p-2")}>
                <tbody>
                    {rows.map(
                        (row) =>
                            row.value != undefined && (
                                <tr key={row.label}>
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
