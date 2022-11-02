import clsx from "clsx";
import { useRef, useCallback, useEffect } from "react";
import filenames from "public/PSBDatabase/files.json";
import { MemoizedViewGLCanvasSmall } from "src/components/model-viewer/ModelViewer";
import ThreeJSViewGL, { RenderMaterial } from "src/components/model-viewer/viewGL";
import { ModelProvider } from "src/lib/contexts";

const files = Object.keys(filenames).reduce<string[]>(
    (prev, key) => [...prev, ...(filenames as { [key: string]: string[] })[key]],
    [],
);

export default function GenerateImages() {
    const viewGL = useRef<ThreeJSViewGL>();
    const intervalId = useRef<NodeJS.Timer>();
    const i = useRef(0);
    const onCanvasMounted = useCallback((viewGLFromCanvas: ThreeJSViewGL) => {
        viewGL.current = viewGLFromCanvas;
    }, []);

    useEffect(() => {
        viewGL.current?.setMaterial(RenderMaterial.Phong);
        viewGL.current?.setAutoRotateEnabled(false);
        viewGL.current?.showWireframe(false);
        viewGL.current?.setCameraZ(0.9);
        viewGL.current?.showBoundingBox(false);
        viewGL.current?.showUnitBox(false);
    }, []);

    const onStart = () => {
        intervalId.current = setInterval(() => {
            const ws = files[i.current].split(".");
            viewGL.current?.loadModelByUrl("PSBDatabase/models/" + ws[0] + "_processed." + ws[1]);
            setTimeout(() => {
                viewGL.current?.capture();
            }, 200);

            i.current++;
            if (i.current == files.length) clearInterval(intervalId.current);
        }, 500);
    };

    return (
        <ModelProvider>
            <div className={clsx("flex", "flex-col", "items-center", "m-4")}>
                <p
                    className={clsx(
                        "cursor-pointer",
                        "border-2",
                        "w-max",
                        "px-1",
                        "m-2",
                        "border-slate-200",
                        "hover:text-slate-300",
                        "hover:border-slate-300",
                    )}
                    onClick={onStart}
                >
                    START
                </p>
                <div className={clsx("w-max")}>
                    <MemoizedViewGLCanvasSmall
                        className={clsx("border-2", "border-slate-200", "mx-2", "mt-4")}
                        onMounted={onCanvasMounted}
                    />
                </div>
            </div>
        </ModelProvider>
    );
}
