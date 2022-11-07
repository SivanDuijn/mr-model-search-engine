import clsx from "clsx";
import Image from "next/image";
import { useModel } from "src/lib/contexts/hooks";
import { database } from "./model-viewer/ModelViewer";

type Props = {
    className?: string;
};

export default function TopClosestModels(props: Props) {
    const { model, changeModel } = useModel();
    return (
        <div className={props.className}>
            <p className={clsx("border-b-2", "text-center", "font-bold")}>Top 10 closest models</p>
            <div className={clsx("flex", "justify-around", "my-2")}>
                {model.top_k &&
                    model.top_k.map((cm) => (
                        <div
                            key={cm.name}
                            className={clsx("flex", "flex-col", "items-center", "w-32")}
                        >
                            <Image
                                src={
                                    "/" +
                                    database +
                                    "/model_images/" +
                                    cm.name.split(".")[0] +
                                    "_processed.png"
                                }
                                alt={cm.name}
                                width={128}
                                height={128}
                            />
                            <div
                                className={clsx(
                                    "w-max",
                                    "pt-[3px]",
                                    "pb-[2px]",
                                    "px-2",
                                    "m-1",
                                    "text-white",
                                    "text-center",
                                    "text-sm",
                                    model.secondModel === cm.name
                                        ? "bg-slate-500"
                                        : "bg-slate-700 hover:bg-slate-500",
                                    "hover:cursor-pointer",
                                )}
                                onClick={() => {
                                    if (cm.name !== undefined)
                                        if (cm.name != model.secondModel)
                                            changeModel({
                                                ...model,
                                                secondModel: cm.name as unknown as string,
                                            });
                                        else
                                            changeModel({
                                                ...model,
                                                secondModel: undefined,
                                            });
                                }}
                            >
                                {cm.name.split(".")[0]}
                            </div>
                            <p className="text-center">
                                d:{" "}
                                <span className={clsx("text-green-500")}>{cm.dist.toFixed(4)}</span>
                            </p>
                        </div>
                    ))}
            </div>
        </div>
    );
}
