import clsx from "clsx";
import { useState, useMemo } from "react";
import filenames from "./filenames.json";

type ModelSelectorProps = {
    onModelSelected: (url: string) => void;
    onFileSelected: (textContent: string) => void;
    className?: string;
};

export default function ModelSelector(props: ModelSelectorProps) {
    const [subgroup, setSubgroup] = useState<string>();

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
                        accept=".obj"
                        onChange={(e) => {
                            if (e.currentTarget.files)
                                e.currentTarget.files[0]
                                    .text()
                                    .then((text) => props.onFileSelected(text));
                            setSubgroup(undefined);
                        }}
                    />
                </div>
                <p className="mt-5">Select file from database</p>
                <div>
                    <select
                        onChange={(e) => setSubgroup(e.currentTarget.value)}
                        value={subgroup ? undefined : ""}
                    >
                        {!subgroup && <option value=""></option>}
                        {Object.keys(filenames).map((subgroup) => (
                            <option key={subgroup} value={subgroup}>
                                {subgroup}
                            </option>
                        ))}
                    </select>
                    <select
                        className="ml-3 min-w-[7rem]"
                        onChange={(e) => {
                            const file = e.currentTarget.value;

                            if (subgroup !== undefined && file !== undefined)
                                props.onModelSelected("models/" + subgroup + "/" + file);
                        }}
                        value={subgroup ? undefined : ""}
                    >
                        <option value=""></option>
                        {subgroupfiles.map((file) => (
                            <option key={file} value={file}>
                                {file}
                            </option>
                        ))}
                    </select>
                </div>
            </div>
        </div>
    );
}
