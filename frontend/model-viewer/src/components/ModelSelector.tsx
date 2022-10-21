import clsx from "clsx";
import { useRouter } from "next/router";
import { useState, useMemo, useEffect } from "react";
import filenames from "public/PSBDatabase/files.json";
import { useModel, useModelStats } from "src/lib/contexts/hooks";

type ModelSelectorProps = {
    onFileSelected: (textContent: string, fileName: string) => void;
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
                file: undefined,
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
            <p className={clsx("border-b-2", "text-center", "font-bold")}>Load Model</p>
            <div className={clsx("p-2")}>
                <div className="flex flex-col">
                    <label htmlFor="model">Load model from file</label>
                    <input
                        id="model"
                        type="file"
                        accept=".obj,.off"
                        onChange={(e) => {
                            if (!e.currentTarget.files) return;
                            const file = e.currentTarget.files[0];
                            if (file)
                                file.text().then((text) => {
                                    const m = file.name.split("/");
                                    props.onFileSelected(text, m[m.length - 1]);
                                    setSubgroup(undefined);
                                    changeModel({
                                        ...model,
                                        file: m[m.length - 1],
                                        name: undefined,
                                    });
                                });
                        }}
                    />
                </div>
                <p className="mt-5">Select file from database</p>
                <div className="flex flex-row">
                    <select
                        onChange={(e) => {
                            setSubgroup(e.currentTarget.value);
                            const files = (filenames as unknown as Record<string, string[]>)[
                                e.currentTarget.value
                            ];
                            if (files) changeModel({ ...model, name: files[0], file: undefined });
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
                                if (subgroup !== undefined && file !== undefined)
                                    changeModel({ ...model, name: file, file: undefined });
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
