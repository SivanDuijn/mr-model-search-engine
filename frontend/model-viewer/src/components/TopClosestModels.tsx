import clsx from "clsx";
import { useMemo } from "react";
import topClosest from "public/PSBDatabase/closest_models.json";
import { useModel } from "src/lib/contexts/hooks";

type Props = {
    className?: string;
};

export default function TopClosestModels(props: Props) {
    const { model, changeModel } = useModel();

    const closestModels = useMemo(() => {
        if (model.name)
            return (topClosest as unknown as Record<string, (string | number)[][]>)[model.name].map(
                (cm) => ({ name: cm[0] as string, dist: cm[1] as number }),
            );
        return [];
    }, [model]);

    console.log(closestModels);

    return (
        <div className={props.className}>
            <p className={clsx("border-b-2", "text-center", "font-bold")}>Top 10 closest models</p>
            {/* <table className={clsx("p-2", "ml-2", "mt-1")}>
                <tbody>
                    {closestModels.map((cmodel) => (
                        <tr key={cmodel.name} className="ml-2">
                            <td>
                                <div
                                    className={clsx(
                                        "min-w-[2.5rem]",
                                        "pt-[3px]",
                                        "pb-[2px]",
                                        "m-1",
                                        "text-white",
                                        "text-center",
                                        "text-sm",
                                        model.name === (cmodel.name as unknown as string)
                                            ? "bg-slate-500"
                                            : "bg-slate-700 hover:bg-slate-500",
                                        "hover:cursor-pointer",
                                    )}
                                    onClick={() => {
                                        if (cmodel.name !== undefined)
                                            changeModel({
                                                ...model,
                                                name: cmodel.name as unknown as string,
                                                file: undefined,
                                            });
                                    }}
                                >
                                    {(cmodel.name as unknown as string).split(".")[0]}
                                </div>
                            </td>
                            <td className={clsx("text-green-500", "pl-4")}>
                                {Number(cmodel.dist.toFixed(3))}
                            </td>
                        </tr>
                    ))}
                </tbody>
            </table> */}
            <div className={clsx("flex", "flex-col", "flex-wrap", "mt-2", "ml-2", "h-[9rem]")}>
                {closestModels.map((cmodel) => (
                    <div className={clsx("flex")} key={cmodel.name}>
                        <div
                            className={clsx(
                                "min-w-[2.5rem]",
                                "pt-[3px]",
                                "pb-[2px]",
                                "m-1",
                                "text-white",
                                "text-center",
                                "text-sm",
                                model.name === (cmodel.name as unknown as string)
                                    ? "bg-slate-500"
                                    : "bg-slate-700 hover:bg-slate-500",
                                "hover:cursor-pointer",
                            )}
                            onClick={() => {
                                if (cmodel.name !== undefined)
                                    changeModel({
                                        ...model,
                                        name: cmodel.name as unknown as string,
                                        file: undefined,
                                    });
                            }}
                        >
                            {(cmodel.name as unknown as string).split(".")[0]}
                        </div>
                        <p className={clsx("text-green-500", "pl-1", "text-sm", "mt-2")}>
                            {Number(cmodel.dist.toFixed(3))}
                        </p>
                    </div>
                ))}
            </div>
        </div>
    );
}
