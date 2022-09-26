import clsx from "clsx";

type SettingsProps = {
    onWireframeEnable: (enabled: boolean) => void;
    onVertexNormalsEnable: (enabled: boolean) => void;
    className?: string;
};

export default function Settings(props: SettingsProps) {
    return (
        <div className={props.className}>
            <p className={clsx("border-b-2", "text-center", "font-bold")}>Settings</p>
            <div className={clsx("p-2")}>
                <div>
                    <label htmlFor="renderstyle" className="mr-2">
                        Wireframe
                    </label>
                    <input
                        type="checkbox"
                        id="renderstyle"
                        onChange={(e) => props.onWireframeEnable(e.currentTarget.checked)}
                        defaultChecked={true}
                    />
                </div>
                <div>
                    <label htmlFor="vertexNormals" className="mr-2">
                        Vertex normals
                    </label>
                    <input
                        type="checkbox"
                        id="vertexNormals"
                        onChange={(e) => props.onVertexNormalsEnable(e.currentTarget.checked)}
                        defaultChecked={false}
                    />
                </div>
            </div>
        </div>
    );
}
