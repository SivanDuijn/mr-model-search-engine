import "../styles/globals.css";
import clsx from "clsx";
import { useCallback, useRef } from "react";
import { MemoizedViewGLCanvas } from "../components/model-viewer/ModelViewer";
import ThreeJSViewGL from "../components/model-viewer/viewGL";
import ModelSelector from "../components/ModelSelector/ModelSelector";
import Settings from "../components/Settings";

function MyApp() {
    const viewGL = useRef<ThreeJSViewGL>();
    const onCanvasMounted = useCallback((viewGLFromCanvas: ThreeJSViewGL) => {
        viewGL.current = viewGLFromCanvas;
    }, []);

    return (
        <div className={clsx("grid", "lg:grid-cols-[1fr_auto_1fr]")}>
            <ModelSelector
                className={clsx("border-2", "border-slate-200", "mx-2", "mt-4")}
                onFileSelected={(textContent) => viewGL.current?.loadModelByText(textContent)}
                onModelSelected={(url) => viewGL.current?.loadModelByUrl(url)}
            />
            <MemoizedViewGLCanvas
                className={clsx("border-2", "border-slate-200", "mx-2", "mt-4")}
                onMounted={onCanvasMounted}
            />
            <Settings
                className={clsx("border-2", "border-slate-200", "mx-2", "mt-4")}
                onRenderStyleChanged={(renderStyle) => viewGL.current?.setRenderStyle(renderStyle)}
            />
        </div>
    );
}

export default MyApp;
