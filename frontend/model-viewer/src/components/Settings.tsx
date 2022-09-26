import clsx from "clsx";
import { RenderMaterial } from "./model-viewer/viewGL";

type SettingsProps = {
    onRenderMaterialChange: (material: RenderMaterial) => void;
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
                    <label htmlFor="renderMaterial" className="mr-2">
                        Render material
                    </label>
                    <select
                        id="renderMaterial"
                        defaultValue={RenderMaterial.Flat}
                        onChange={(e) =>
                            props.onRenderMaterialChange(
                                parseInt(e.currentTarget.value) as unknown as RenderMaterial,
                            )
                        }
                    >
                        <option value={RenderMaterial.Flat} label={"Flat"} />
                        <option value={RenderMaterial.Phong} label={"Phong"} />
                        <option value={RenderMaterial.Normals} label={"Normals"} />
                        <option value={RenderMaterial.Cartoon} label={"Cartoon"} />
                        <option value={RenderMaterial.Hidden} label={"Hidden"} />
                    </select>
                </div>
                <div>
                    <label htmlFor="wireframe" className="mr-2">
                        Wireframe
                    </label>
                    <input
                        type="checkbox"
                        id="wireframe"
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
