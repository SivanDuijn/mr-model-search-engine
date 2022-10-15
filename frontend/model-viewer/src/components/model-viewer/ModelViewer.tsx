import React, { useContext, useEffect, useRef } from "react";
import { ModelContext } from "src/lib/contexts";
import ThreeJSViewGL from "./viewGL";

type Props = {
    onMounted: (viewGL: ThreeJSViewGL) => void;
    className?: string;
};

// eslint-disable-next-line react/display-name
export const MemoizedViewGLCanvas = React.memo((props: Props) => {
    const canvasRef = useRef<HTMLCanvasElement>(null);
    const mouseIsDown = useRef<boolean>(false);
    const viewGL = useRef<ThreeJSViewGL>();

    useEffect(() => {
        viewGL.current = new ThreeJSViewGL(canvasRef.current || undefined);
        props.onMounted(viewGL.current);
    }, []);

    const { state } = useContext(ModelContext);
    useEffect(() => {
        viewGL.current?.setMaterial(state.renderSettings.material);
        viewGL.current?.showWireframe(state.renderSettings.showWireframe);
        viewGL.current?.showVertexNormals(state.renderSettings.showVertexNormals);
        viewGL.current?.setAutoRotateEnabled(state.renderSettings.autoRotateEnabled);
    }, [state.renderSettings]);

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
