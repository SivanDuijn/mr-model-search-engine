import clsx from "clsx";
import { useCallback, useRef } from "react";
import DescriptorHistograms from "src/components/DescriptoHistograms";
import ModelDescriptors from "src/components/ModelDescriptors";
import ModelInformation from "src/components/ModelInformation";
import { ModelProvider } from "src/lib/contexts";
import { MemoizedViewGLCanvas } from "../components/model-viewer/ModelViewer";
import ThreeJSViewGL from "../components/model-viewer/viewGL";
import ModelSelector from "../components/ModelSelector/ModelSelector";
import Settings from "../components/Settings";

export default function HomePage() {
    const viewGL = useRef<ThreeJSViewGL>();
    const onCanvasMounted = useCallback((viewGLFromCanvas: ThreeJSViewGL) => {
        viewGL.current = viewGLFromCanvas;
    }, []);

    return (
        <ModelProvider>
            <div className={clsx("flex", "flex-col", "h-full")}>
                <div className={clsx("grid", "lg:grid-cols-[1fr_auto_1fr]")}>
                    <div className={clsx("grid", "lg:grid-rows-[auto_1fr]")}>
                        <ModelSelector
                            className={clsx("border-2", "border-slate-200", "mx-2", "mt-4")}
                            onFileSelected={(textContent, filetype) =>
                                viewGL.current?.loadModelByText(textContent, filetype)
                            }
                        />
                        <Settings
                            className={clsx("border-2", "border-slate-200", "mx-2", "mt-4")}
                        />
                    </div>
                    <MemoizedViewGLCanvas
                        className={clsx("border-2", "border-slate-200", "mx-2", "mt-4")}
                        onMounted={onCanvasMounted}
                    />
                    <div className={clsx("grid", "lg:grid-rows-[auto_1fr]")}>
                        <ModelInformation
                            className={clsx("border-2", "border-slate-200", "mx-2", "mt-4")}
                        />
                        <ModelDescriptors
                            className={clsx("border-2", "border-slate-200", "mx-2", "mt-4")}
                        />
                    </div>
                </div>
                <DescriptorHistograms
                    className={clsx(
                        "border-2",
                        "border-slate-200",
                        "mx-2",
                        "mt-4",
                        "mb-2",
                        "flex-grow",
                    )}
                />
            </div>
        </ModelProvider>
    );
}
