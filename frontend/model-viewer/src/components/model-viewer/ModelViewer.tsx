import React, { useEffect, useRef } from "react";
import { useModel, useRenderSettings } from "src/lib/contexts/hooks";
import ThreeJSViewGL from "./viewGL";

const database = "PSBDatabase";

type Props = {
    onMounted: (viewGL: ThreeJSViewGL) => void;
    className?: string;
};

// eslint-disable-next-line react/display-name
export const MemoizedViewGLCanvas = React.memo((props: Props) => {
    const canvasRef = useRef<HTMLCanvasElement>(null);
    const mouseIsDown = useRef<boolean>(false);
    const viewGL = useRef<ThreeJSViewGL>();
    const { settings } = useRenderSettings();
    const { model, changeModel } = useModel();

    useEffect(() => {
        viewGL.current = new ThreeJSViewGL(canvasRef.current || undefined);
        props.onMounted(viewGL.current);
    }, []);

    useEffect(() => {
        viewGL.current?.setMaterial(settings.material);
        viewGL.current?.showWireframe(settings.showWireframe);
        viewGL.current?.showVertexNormals(settings.showVertexNormals);
        viewGL.current?.setAutoRotateEnabled(settings.autoRotateEnabled);
    }, [settings]);

    useEffect(() => {
        if (model.name) {
            viewGL.current?.setOnModelStatsChanged((stats) => changeModel({ ...model, stats }));
            const ws = model.name.split(".");
            viewGL.current?.loadModelByUrl(
                database + "/models/" + ws[0] + `${model.isProcessed ? "_processed" : ""}.` + ws[1],
            );
        }
    }, [model.name, model.isProcessed]);

    const prevXY = useRef<{ x: number; y: number }>({ x: 0, y: 0 });

    return (
        <div className={props.className}>
            <canvas
                ref={canvasRef}
                onTouchStart={(e) => {
                    if (e.touches.length != 1) return;
                    viewGL.current?.onMouseDown();
                    prevXY.current = { x: e.touches[0].clientX, y: e.touches[0].clientY };
                }}
                onTouchMove={(e) => {
                    if (e.touches.length != 1) return;
                    const x = e.touches[0].clientX;
                    const y = e.touches[0].clientY;
                    viewGL.current?.onMouseDrag(x - prevXY.current.x, y - prevXY.current.y);
                    prevXY.current = { x, y };
                }}
                onTouchEnd={(e) => {
                    if (e.touches.length != 0) return;
                    viewGL.current?.onMouseUp();
                }}
                onMouseDown={(e) => {
                    if (e.button === 0) {
                        mouseIsDown.current = true;
                        viewGL.current?.onMouseDown();
                    }
                }}
                onMouseUp={(e) => {
                    if (e.button === 0) {
                        mouseIsDown.current = false;
                        viewGL.current?.onMouseUp();
                    }
                }}
                onMouseMove={(e) => {
                    if (mouseIsDown.current) viewGL.current?.onMouseDrag(e.movementX, e.movementY);
                }}
            />
        </div>
    );
});
