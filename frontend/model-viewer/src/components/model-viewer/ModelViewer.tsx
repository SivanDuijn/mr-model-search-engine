import clsx from "clsx";
import React, { useContext, useEffect, useRef } from "react";
import { ModelContext } from "src/lib/contexts";
import { ActionKind, ModelState } from "src/lib/contexts/reducer";
import GetModelDescriptors from "src/lib/getModelDescriptors";
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
    const { state, dispatch } = useContext(ModelContext);

    const setModelState = (stats: ModelState["modelStats"]) => {
        dispatch({
            type: ActionKind.ChangeModelStats,
            payload: stats,
        });
    };
    useEffect(() => {
        viewGL.current = new ThreeJSViewGL(canvasRef.current || undefined);
        viewGL.current?.setOnModelStatsChanged((stats) => setModelState(stats));
        props.onMounted(viewGL.current);
    }, []);

    useEffect(() => {
        viewGL.current?.setMaterial(state.renderSettings.material);
        viewGL.current?.showWireframe(state.renderSettings.showWireframe);
        viewGL.current?.showVertexNormals(state.renderSettings.showVertexNormals);
        viewGL.current?.setAutoRotateEnabled(state.renderSettings.autoRotateEnabled);
    }, [state.renderSettings]);

    useEffect(() => {
        if (state.model.name) {
            dispatch({
                type: ActionKind.ChangeModelDescriptors,
                payload: GetModelDescriptors(state.model.name, state.model.isProcessed),
            });
            const ws = state.model.name.split(".");
            viewGL.current?.loadModelByUrl(
                database +
                    "/models/" +
                    ws[0] +
                    `${state.model.isProcessed ? "_processed" : ""}.` +
                    ws[1],
            );
        }
    }, [state.model.name, state.model.isProcessed]);

    const prevXY = useRef<{ x: number; y: number }>({ x: 0, y: 0 });

    return (
        <div className={clsx(props.className, "relative")}>
            {state.model.secondModel && (
                <MemoizedViewGLCanvasSmall
                    className={clsx(
                        "absolute",
                        "bottom-0",
                        "left-0",
                        "border-r-2",
                        "border-t-2",
                        "border-slate-200",
                    )}
                />
            )}
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

// eslint-disable-next-line react/display-name
export const MemoizedViewGLCanvasSmall = React.memo((props: { className?: string }) => {
    const canvasRef = useRef<HTMLCanvasElement>(null);
    const mouseIsDown = useRef<boolean>(false);
    const viewGL = useRef<ThreeJSViewGL>();
    const { state } = useContext(ModelContext);

    useEffect(() => {
        viewGL.current = new ThreeJSViewGL(canvasRef.current || undefined, 250, 250);
    }, []);

    useEffect(() => {
        viewGL.current?.setMaterial(state.renderSettings.material);
        viewGL.current?.showWireframe(state.renderSettings.showWireframe);
        viewGL.current?.showVertexNormals(state.renderSettings.showVertexNormals);
        viewGL.current?.setAutoRotateEnabled(state.renderSettings.autoRotateEnabled);
    }, [state.renderSettings]);

    useEffect(() => {
        if (state.model.secondModel) {
            const ws = state.model.secondModel.split(".");
            viewGL.current?.loadModelByUrl(
                database +
                    "/models/" +
                    ws[0] +
                    `${state.model.isProcessed ? "_processed" : ""}.` +
                    ws[1],
            );
        }
    }, [state.model.secondModel, state.model.isProcessed]);

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
