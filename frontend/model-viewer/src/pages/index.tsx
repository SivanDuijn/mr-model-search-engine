import clsx from "clsx";
import Head from "next/head";
import { useCallback, useRef } from "react";
import DescriptorHistograms from "src/components/DescriptorHistograms";
import ModelDescriptors from "src/components/ModelDescriptors";
import ModelInformation from "src/components/ModelInformation";
import TopClosestModels from "src/components/TopClosestModels";
import { MemoizedViewGLCanvas } from "../components/model-viewer/ModelViewer";
import ThreeJSViewGL from "../components/model-viewer/viewGL";
import ModelSelector from "../components/ModelSelector";
import Settings from "../components/Settings";

export default function HomePage() {
    const viewGL = useRef<ThreeJSViewGL>();
    const onCanvasMounted = useCallback((viewGLFromCanvas: ThreeJSViewGL) => {
        viewGL.current = viewGLFromCanvas;
    }, []);

    return (
        <div className={clsx("flex", "flex-col", "h-full")}>
            <Head>
                <title>Model Go BRRR</title>
            </Head>
            <div className={clsx("grid", "lg:grid-cols-[1fr_auto_1fr]")}>
                <div className={clsx("grid", "lg:grid-rows-[auto_1fr]")}>
                    <ModelSelector
                        className={clsx("border-2", "border-slate-200", "mx-2", "mt-4")}
                    />
                    <Settings className={clsx("border-2", "border-slate-200", "mx-2", "mt-4")} />
                </div>
                <div>
                    <MemoizedViewGLCanvas
                        className={clsx("border-2", "border-slate-200", "mx-2", "mt-4")}
                        onMounted={onCanvasMounted}
                    />
                </div>
                <div className={clsx("grid", "lg:grid-rows-[auto_1fr]")}>
                    <ModelInformation
                        className={clsx("border-2", "border-slate-200", "mx-2", "mt-4")}
                    />
                    <ModelDescriptors
                        className={clsx("border-2", "border-slate-200", "mx-2", "mt-4")}
                    />
                </div>
            </div>
            <TopClosestModels className={clsx("border-2", "border-slate-200", "mx-2", "mt-4")} />
            <DescriptorHistograms
                className={clsx(
                    "border-2",
                    "border-slate-200",
                    "mx-2",
                    "mt-4",
                    "mb-2",
                    "flex-grow",
                    "min-w-[600px]",
                )}
            />
        </div>
    );
}
