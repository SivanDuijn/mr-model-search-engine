import clsx from "clsx";
import { useRouter } from "next/router";
import { useState, useMemo, useEffect } from "react";
import filenames from "public/PSBDatabase/files.json";
import { useModel, useModelStats } from "src/lib/contexts/hooks";
import GetTopK from "src/lib/getTopK";

type ModelSelectorProps = {
    className?: string;
};

export default function ModelSelector(props: ModelSelectorProps) {
    const { model, changeModel } = useModel();
    const { stats } = useModelStats();

    const router = useRouter();
    const modelNameFromUrl = router.query["m"];
    useEffect(() => {
        if (modelNameFromUrl && !Array.isArray(modelNameFromUrl))
            changeModel({
                ...model,
                name: modelNameFromUrl.replace("_processed", ""),
                isProcessed: modelNameFromUrl.includes("_processed"),
                text: undefined,
            });
    }, [modelNameFromUrl]);

    const [subgroup, setSubgroup] = useState<string | undefined>(stats?.className);

    useEffect(() => setSubgroup(stats?.className), [stats?.className]);

    const subgroupfiles = useMemo(() => {
        if (subgroup) {
            const files = (filenames as unknown as Record<string, string[]>)[subgroup];
            if (files) return files;
        }
        return [];
    }, [subgroup]);

    return (
        <div className={props.className}>
            <p className={clsx("border-b-2", "text-center", "font-bold")}>Query Model</p>
            <div className={clsx("p-2")}>
                <div className="flex flex-col">
                    <label htmlFor="model">Query model from file</label>
                    <input
                        id="model"
                        type="file"
                        accept=".obj,.off"
                        onChange={(e) => {
                            if (!e.currentTarget.files) return;
                            const file = e.currentTarget.files[0];
                            const data = new FormData();
                            data.append("file", file, file.name);
                            fetch("http://localhost:5000/api/query", {
                                method: "POST",
                                body: data,
                                mode: "cors",
                            }).then((r) =>
                                r
                                    .json()
                                    .then(
                                        ({
                                            top_k,
                                            processed_model,
                                            stats,
                                            descriptors,
                                        }: {
                                            processed_model: string;
                                            top_k: { name: string; dist: number }[];
                                            stats: any;
                                            descriptors: any;
                                        }) => {
                                            setSubgroup(undefined);
                                            console.log(top_k, stats, descriptors);
                                            changeModel({
                                                ...model,
                                                text: processed_model,
                                                top_k,
                                                secondModel: model.secondModel
                                                    ? top_k[0].name
                                                    : undefined,
                                                name: undefined,
                                            });
                                        },
                                    ),
                            );
                            // file.text().then((text) => {
                            //     const m = file.name.split("/");
                            //     props.onFileSelected(text, m[m.length - 1]);
                            //     setSubgroup(undefined);
                            //     changeModel({
                            //         ...model,
                            //         file: m[m.length - 1],
                            //         text,
                            //         name: undefined,
                            //     });
                            // });
                        }}
                    />
                </div>
                <p className="mt-5">Query model from database</p>
                <div className="flex flex-row">
                    <select
                        onChange={(e) => {
                            setSubgroup(e.currentTarget.value);
                            const files = (filenames as unknown as Record<string, string[]>)[
                                e.currentTarget.value
                            ];
                            if (files) {
                                const file = files[0];
                                const top_k = GetTopK(file);
                                changeModel({
                                    ...model,
                                    name: files[0],
                                    text: undefined,
                                    top_k,
                                    secondModel: model.secondModel ? top_k[0].name : undefined,
                                });
                            }
                        }}
                        value={subgroup ?? ""}
                    >
                        {!subgroup && <option value=""></option>}
                        {Object.keys(filenames).map((subgroup) => (
                            <option key={subgroup} value={subgroup}>
                                {subgroup}
                            </option>
                        ))}
                    </select>
                    <div className="flex flex-row ml-4">
                        <p>Processed:</p>
                        <input
                            id="preprocessed"
                            title="preprocessed"
                            className="ml-2"
                            type="checkbox"
                            checked={model.isProcessed ?? true}
                            onChange={(e) => {
                                if (e.currentTarget.checked != model.isProcessed)
                                    changeModel({
                                        ...model,
                                        isProcessed: e.currentTarget.checked,
                                    });
                            }}
                        />
                    </div>
                </div>
                <div className={clsx("flex", "flex-wrap", "mt-2")}>
                    {subgroupfiles.map((file) => (
                        <div
                            key={file}
                            className={clsx(
                                "min-w-[2.5rem]",
                                "pt-[3px]",
                                "pb-[2px]",
                                "m-1",
                                "text-white",
                                "text-center",
                                "text-sm",
                                model.name === file
                                    ? "bg-slate-500"
                                    : "bg-slate-700 hover:bg-slate-500",
                                "hover:cursor-pointer",
                            )}
                            onClick={() => {
                                if (subgroup !== undefined && file !== undefined) {
                                    const top_k = GetTopK(file);
                                    changeModel({
                                        ...model,
                                        name: file,
                                        text: undefined,
                                        top_k,
                                        secondModel: model.secondModel ? top_k[0].name : undefined,
                                    });
                                }
                            }}
                        >
                            {file.split(".")[0]}
                        </div>
                    ))}
                </div>
            </div>
        </div>
    );
}
