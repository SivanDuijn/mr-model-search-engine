import { useEffect, useMemo, useRef, useState } from "react";
import filenames from "./filename.json";
import ViewGL, { RenderStyle } from "./viewGL";

export default function ModelViewer() {
    const canvasRef = useRef<HTMLCanvasElement>(null);
    const [subgroup, setSubgroup] = useState<string>();

    const viewGL = useRef<ViewGL>();

    useEffect(() => {
        viewGL.current = new ViewGL(canvasRef.current || undefined);
    }, []);

    const subgroupfiles = useMemo(() => {
        if (subgroup) {
            const files = (filenames as unknown as Record<string, string[]>)[subgroup];
            if (files) return files;
        }
        return [];
    }, [subgroup]);

    return (
        <div className="flex flex-col items-center min-w-[7rem]">
            <canvas ref={canvasRef} />
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
                                .then((text) => viewGL.current?.loadModel(text));
                        setSubgroup(undefined);
                    }}
                />
            </div>
            <p className="mt-5">Or select file from database</p>
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

                        if (subgroup !== undefined && file !== undefined) {
                            viewGL.current?.loadModelByUrl("models/" + subgroup + "/" + file);
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
            </div>
            <div className="mt-3">
                <label htmlFor="renderstyle" className="mr-2">
                    Wireframe
                </label>
                <input
                    type="checkbox"
                    id="renderstyle"
                    onChange={(e) =>
                        viewGL.current?.setRenderStyle(
                            e.currentTarget.checked ? RenderStyle.wireframe : RenderStyle.normal,
                        )
                    }
                    defaultChecked
                />
            </div>
        </div>
    );
}
