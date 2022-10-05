import clsx from "clsx";
import { RenderMaterial } from "./model-viewer/viewGL";

type SettingsProps = {
    defaultMaterial: RenderMaterial;
    defaultWireframeEnabled: boolean;
    defaultVertexNormalsEnabled: boolean;
    defaultAutoRotateEnabled: boolean;
    onRenderMaterialChange: (material: RenderMaterial) => void;
    onWireframeEnable: (enabled: boolean) => void;
    onVertexNormalsEnable: (enabled: boolean) => void;
    onAutoRotateEnable: (enabled: boolean) => void;
    className?: string;
};

export default function Settings(props: SettingsProps) {
    return (
        <div className={props.className}>
            <p className={clsx("border-b-2", "text-center", "font-bold")}>Settings</p>
            <div className={clsx("p-2")}>
                <div>
                    <label htmlFor="renderMaterial" className="mr-2">
                        Render material
                    </label>
                    <select
                        id="renderMaterial"
                        defaultValue={props.defaultMaterial}
                        onChange={(e) =>
                            props.onRenderMaterialChange(
                                parseInt(e.currentTarget.value) as unknown as RenderMaterial,
                            )
                        }
                    >
                        <option value={RenderMaterial.Flat} label={"Flat"} />
                        <option value={RenderMaterial.Phong} label={"Phong"} />
                        <option value={RenderMaterial.Normals} label={"Normals"} />
                        <option value={RenderMaterial.PointCloud} label={"Point cloud"} />
                        <option value={RenderMaterial.WireframeOnly} label={"Wireframe only"} />
                        <option value={RenderMaterial.Cartoon} label={"Cartoon"} />
                    </select>
                </div>
                <div>
                    <label htmlFor="wireframe" className="mr-2">
                        Wireframe
                    </label>
                    <input
                        type="checkbox"
                        id="wireframe"
                        onClick={(e) => props.onWireframeEnable(e.currentTarget.checked)}
                        defaultChecked={props.defaultWireframeEnabled}
                    />
                </div>
                <div>
                    <label htmlFor="vertexNormals" className="mr-2">
                        Vertex normals
                    </label>
                    <input
                        type="checkbox"
                        id="vertexNormals"
                        onClick={(e) => props.onVertexNormalsEnable(e.currentTarget.checked)}
                        defaultChecked={props.defaultVertexNormalsEnabled}
                    />
                </div>
                <div>
                    <label htmlFor="rotate" className="mr-2">
                        Rotate
                    </label>
                    <input
                        type="checkbox"
                        id="rotate"
                        onClick={(e) => props.onAutoRotateEnable(e.currentTarget.checked)}
                        defaultChecked={props.defaultAutoRotateEnabled}
                    />
                </div>
            </div>
        </div>
    );
}
