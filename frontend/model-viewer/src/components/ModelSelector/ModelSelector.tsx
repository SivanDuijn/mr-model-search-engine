import clsx from "clsx";
import { useState, useMemo, useRef } from "react";
import filenames from "public/PSBDatabase/files.json";

const database = "PSBDatabase";

type ModelSelectorProps = {
    onModelSelected: (url: string) => void;
    onFileSelected: (textContent: string, fileName: string) => void;
    className?: string;
};

export default function ModelSelector(props: ModelSelectorProps) {
    const [subgroup, setSubgroup] = useState<string>();
    const usePreprocessed = useRef(true);
    const currentModel = useRef("");

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
                                });
                            setSubgroup(undefined);
                        }}
                    />
                </div>
                <p className="mt-5">Select file from database</p>
                <div className="flex flex-row">
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

                            if (subgroup !== undefined && file !== undefined) {
                                currentModel.current = file;
                                const ws = file.split(".");
                                props.onModelSelected(
                                    database +
                                        "/models/" +
                                        ws[0] +
                                        `${usePreprocessed.current ? "_processed" : ""}.` +
                                        ws[1],
                                );
                            }
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
                    <div className="flex flex-row">
                        <input
                            id="preprocessed"
                            title="preprocessed"
                            className="ml-2"
                            type="checkbox"
                            defaultChecked={true}
                            onChange={(e) => {
                                if (e.currentTarget.checked != usePreprocessed.current) {
                                    usePreprocessed.current = e.currentTarget.checked;
                                    if (currentModel.current != "") {
                                        const ws = currentModel.current.split(".");
                                        props.onModelSelected(
                                            database +
                                                "/models/" +
                                                ws[0] +
                                                `${usePreprocessed.current ? "_processed" : ""}.` +
                                                ws[1],
                                        );
                                    }
                                }
                            }}
                        />
                    </div>
                </div>
            </div>
        </div>
    );
}
