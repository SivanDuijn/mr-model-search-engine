import clsx from "clsx";
import { useRouter } from "next/router";
import { useCallback, useEffect, useRef, useState } from "react";
import ModelInformation from "src/components/ModelInformation";
import { GetModelClass, ModelStats } from "src/lib/getModelStats";
import { getRenderMaterial } from "src/lib/utils";
import { MemoizedViewGLCanvas } from "../components/model-viewer/ModelViewer";
import ThreeJSViewGL from "../components/model-viewer/viewGL";
import ModelSelector from "../components/ModelSelector/ModelSelector";
import Settings from "../components/Settings";

export default function HomePage() {
    const router = useRouter();
    const model = router.query["m"];
    const defaults = {
        material: getRenderMaterial(router.query["mat"] as string),
        showWireframe: router.query["wireframe"] === "1" ? true : false,
        showVertextNormals: router.query["vertexnormals"] === "1" ? true : false,
    };
    const [modelStats, setModelStats] = useState<ModelStats>();

    const viewGL = useRef<ThreeJSViewGL>();
    const onCanvasMounted = useCallback((viewGLFromCanvas: ThreeJSViewGL) => {
        viewGL.current = viewGLFromCanvas;
        viewGL.current.setOnModelStatsChanged((stats) => setModelStats(stats));
    }, []);

    useEffect(() => {
        if (model && !Array.isArray(model)) {
            const classn = GetModelClass(model);
            if (classn) {
                viewGL.current?.loadModelByUrl("LabeledDB_new/" + classn + "/" + model);
            }
        }
    }, [model]);
    useEffect(() => {
        if (defaults.material != undefined) viewGL.current?.setMaterial(defaults.material);
        if (defaults.showVertextNormals != undefined)
            viewGL.current?.showVertexNormals(defaults.showVertextNormals);
        if (defaults.showWireframe != undefined)
            viewGL.current?.showWireframe(defaults.showWireframe);
    }, [defaults]);

    return (
        <div className={clsx("grid", "lg:grid-cols-[1fr_auto_1fr]")}>
            <div>
                <ModelSelector
                    className={clsx("border-2", "border-slate-200", "mx-2", "mt-4")}
                    onFileSelected={(textContent, filetype) =>
                        viewGL.current?.loadModelByText(textContent, filetype)
                    }
                    onModelSelected={(url) => viewGL.current?.loadModelByUrl(url)}
                />
                <Settings
                    className={clsx("border-2", "border-slate-200", "mx-2", "mt-8")}
                    onRenderMaterialChange={(material) => viewGL.current?.setMaterial(material)}
                    onWireframeEnable={(enabled) => viewGL.current?.showWireframe(enabled)}
                    onVertexNormalsEnable={(enabled) => viewGL.current?.showVertexNormals(enabled)}
                />
            </div>
            <MemoizedViewGLCanvas
                className={clsx("border-2", "border-slate-200", "mx-2", "mt-4")}
                onMounted={onCanvasMounted}
            />
            <ModelInformation stats={modelStats} />
        </div>
    );
}
