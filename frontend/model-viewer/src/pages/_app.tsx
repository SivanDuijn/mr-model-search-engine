import "../styles/globals.css";
import clsx from "clsx";
import { useCallback, useRef, useState } from "react";
import { MemoizedViewGLCanvas } from "../components/model-viewer/ModelViewer";
import ThreeJSViewGL, { ModelStats } from "../components/model-viewer/viewGL";
import ModelSelector from "../components/ModelSelector/ModelSelector";
import Settings from "../components/Settings";

function MyApp() {
    const [modelStats, setModelStats] = useState<ModelStats>();

    const viewGL = useRef<ThreeJSViewGL>();
    const onCanvasMounted = useCallback((viewGLFromCanvas: ThreeJSViewGL) => {
        viewGL.current = viewGLFromCanvas;
        viewGL.current.setOnModelStatsChanged((stats) => setModelStats(stats));
    }, []);

    return (
        <div className={clsx("grid", "lg:grid-cols-[1fr_auto_1fr]")}>
            <div>
                <ModelSelector
                    className={clsx("border-2", "border-slate-200", "mx-2", "mt-4")}
                    onFileSelected={(textContent) => viewGL.current?.loadModelByText(textContent)}
                    onModelSelected={(url) => viewGL.current?.loadOBJModelByUrl(url)}
                />
                <Settings
                    className={clsx("border-2", "border-slate-200", "mx-2", "mt-8")}
                    onRenderStyleChanged={(renderStyle) =>
                        viewGL.current?.setRenderStyle(renderStyle)
                    }
                    onVertexNormalsEnable={(enabled) =>
                        viewGL.current?.setVertexNormalHelper(enabled)
                    }
                />
            </div>
            <MemoizedViewGLCanvas
                className={clsx("border-2", "border-slate-200", "mx-2", "mt-4")}
                onMounted={onCanvasMounted}
            />
            <div className={clsx("border-2", "border-slate-200", "mx-2", "mt-4")}>
                <p className={clsx("border-b-2", "text-center", "font-bold")}>Model Information</p>
                <div className={clsx("p-2")}>
                    <p>
                        # vertices: <span className="text-green-500">{modelStats?.nVertices}</span>
                    </p>
                    <p>
                        # faces: &nbsp;&nbsp;&nbsp;
                        <span className="text-green-500">{modelStats?.nFaces}</span>
                    </p>
                </div>
            </div>
        </div>
    );
}

export default MyApp;
