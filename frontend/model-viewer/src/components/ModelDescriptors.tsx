import clsx from "clsx";
import { useModelDescriptors } from "src/lib/contexts/hooks";

type Props = {
    className?: string;
};

export default function ModelDescriptors(props: Props) {
    const { descriptors } = useModelDescriptors();

    const rows = [
        { label: "Surface area", value: Number(descriptors?.area.toFixed(5)) },
        { label: "AABB volume", value: Number(descriptors?.AABBVolume.toFixed(5)) },
        { label: "Volume", value: Number(descriptors?.volume.toFixed(5)) },
        {
            label: "Compactness",
            value: Number(descriptors?.compactness.toFixed(5)),
            tip: "How close is the shape to a sphere",
        },
        {
            label: "Eccentricity",
            value: Number(descriptors?.eccentricity.toFixed(5)),
            tip: "Ratio between largest and smallest eigenvalue",
        },
        {
            label: "Diameter",
            value: Number(descriptors?.diameter.toFixed(5)),
            tip: "Largest distance between any two surface points",
        },
    ];

    return (
        <div className={props.className}>
            <p className={clsx("border-b-2", "text-center", "font-bold")}>Descriptors</p>
            <table className={clsx("p-2", "ml-2", "mt-1")}>
                <tbody>
                    {rows.map((row) => (
                        <tr key={row.label} className="ml-2">
                            <td title={row.tip}>{row.label}</td>
                            <td className={clsx("text-green-500", "pl-4")}>
                                {row.value != undefined && !isNaN(row.value) ? row.value : ""}
                            </td>
                        </tr>
                    ))}
                </tbody>
            </table>
        </div>
    );
}
