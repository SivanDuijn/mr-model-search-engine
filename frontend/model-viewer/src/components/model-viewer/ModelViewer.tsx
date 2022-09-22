import React, { useEffect, useRef } from "react";
import ThreeJSViewGL from "./viewGL";

type Props = {
    onMounted: (viewGL: ThreeJSViewGL) => void;
    className?: string;
};

// eslint-disable-next-line react/display-name
export const MemoizedViewGLCanvas = React.memo((props: Props) => {
    const canvasRef = useRef<HTMLCanvasElement>(null);

    useEffect(() => {
        props.onMounted(new ThreeJSViewGL(canvasRef.current || undefined));
    }, []);

    return (
        <div className={props.className}>
            <canvas ref={canvasRef} />
        </div>
    );
});
