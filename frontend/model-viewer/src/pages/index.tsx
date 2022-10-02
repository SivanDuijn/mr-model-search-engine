import clsx from "clsx";
import { useRouter } from "next/router";
import { useCallback, useEffect, useMemo, useRef, useState } from "react";
import ModelInformation from "src/components/ModelInformation";
import { ModelStats } from "src/lib/getModelStats";
import { getRenderMaterial, getURLVariableBool } from "src/lib/utils";
import { MemoizedViewGLCanvas } from "../components/model-viewer/ModelViewer";
import ThreeJSViewGL, { RenderMaterial } from "../components/model-viewer/viewGL";
import ModelSelector from "../components/ModelSelector/ModelSelector";
import Settings from "../components/Settings";

export const defaultSettings = {
    material: RenderMaterial.Flat,
    showWireframe: true,
    showVertexNormals: false,
};

export default function HomePage() {
    const router = useRouter();
    const model = router.query["m"];
    const settingsFromURL = useMemo(
        () => ({
            material: getRenderMaterial(router.query["mat"] as string),
            showWireframe: getURLVariableBool(router.query["wireframe"]),
            showVertextNormals: getURLVariableBool(router.query["vnormals"]),
        }),
        [router],
    );
    const [modelStats, setModelStats] = useState<ModelStats>();

    const viewGL = useRef<ThreeJSViewGL>();
    const onCanvasMounted = useCallback((viewGLFromCanvas: ThreeJSViewGL) => {
        viewGL.current = viewGLFromCanvas;
        viewGL.current.setOnModelStatsChanged((stats) => setModelStats(stats));
    }, []);

    useEffect(() => {
        if (model && !Array.isArray(model)) {
            viewGL.current?.loadModelByUrl("PSBModels/" + model);
        }
    }, [model]);

    useEffect(() => {
        if (settingsFromURL.material != undefined)
            viewGL.current?.setMaterial(settingsFromURL.material);
        if (settingsFromURL.showVertextNormals != undefined)
            viewGL.current?.showVertexNormals(settingsFromURL.showVertextNormals);
        if (settingsFromURL.showWireframe != undefined)
            viewGL.current?.showWireframe(settingsFromURL.showWireframe);
    }, [settingsFromURL]);

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
                    defaultMaterial={settingsFromURL.material ?? defaultSettings.material}
                    defaultWireframeEnabled={
                        settingsFromURL.showWireframe ?? defaultSettings.showWireframe
                    }
                    defaultVertexNormalsEnabled={
                        settingsFromURL.showVertextNormals ?? defaultSettings.showVertexNormals
                    }
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
