import React, { useEffect, useRef } from "react";
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

    return (
        <div className={props.className}>
            <canvas
                ref={canvasRef}
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
