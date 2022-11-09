import clsx from "clsx";
import { useRouter } from "next/router";
import { useEffect } from "react";
import { useRenderSettings } from "src/lib/contexts/hooks";
import { ModelState } from "src/lib/contexts/reducer";
import { getRenderMaterial, getURLVariableBool } from "src/lib/utils";
import { RenderMaterial } from "./model-viewer/viewGL";

type SettingsProps = {
    className?: string;
};

export default function Settings(props: SettingsProps) {
    const { settings, changeRenderSettings } = useRenderSettings();

    // Get settings from url variables, if they are defined
    const router = useRouter();
    useEffect(() => {
        const material = getRenderMaterial(router.query["mat"] as string);
        const showWireframe = getURLVariableBool(router.query["wireframe"]);
        const showVertextNormals = getURLVariableBool(router.query["vnormals"]);
        const autoRotateEnabled = getURLVariableBool(router.query["rotate"]);

        const renderSettingsFromUrl: ModelState["renderSettings"] = { ...settings };
        if (material != undefined) renderSettingsFromUrl.material = material;
        if (showWireframe != undefined) renderSettingsFromUrl.showWireframe = showWireframe;
        if (showVertextNormals != undefined)
            renderSettingsFromUrl.showVertexNormals = showVertextNormals;
        if (autoRotateEnabled != undefined)
            renderSettingsFromUrl.autoRotateEnabled = autoRotateEnabled;

        changeRenderSettings(renderSettingsFromUrl);
    }, [router]);

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
                        value={settings.material}
                        onChange={(e) =>
                            changeRenderSettings({
                                ...settings,
                                material: parseInt(
                                    e.currentTarget.value,
                                ) as unknown as RenderMaterial,
                            })
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
                        onChange={(e) =>
                            changeRenderSettings({
                                ...settings,
                                showWireframe: e.currentTarget.checked,
                            })
                        }
                        checked={settings.showWireframe}
                    />
                </div>
                <div>
                    <label htmlFor="vertexNormals" className="mr-2">
                        Vertex normals
                    </label>
                    <input
                        type="checkbox"
                        id="vertexNormals"
                        onChange={(e) =>
                            changeRenderSettings({
                                ...settings,
                                showVertexNormals: e.currentTarget.checked,
                            })
                        }
                        checked={settings.showVertexNormals}
                    />
                </div>
                <div>
                    <label htmlFor="rotate" className="mr-2">
                        Rotate
                    </label>
                    <input
                        type="checkbox"
                        id="rotate"
                        onChange={(e) =>
                            changeRenderSettings({
                                ...settings,
                                autoRotateEnabled: e.currentTarget.checked,
                            })
                        }
                        checked={settings.autoRotateEnabled}
                    />
                </div>
                <div>
                    <label htmlFor="unitcube" className="mr-2">
                        Unit Cube
                    </label>
                    <input
                        type="checkbox"
                        id="unitcube"
                        onChange={(e) =>
                            changeRenderSettings({
                                ...settings,
                                showUnitBox: e.currentTarget.checked,
                            })
                        }
                        checked={settings.showUnitBox}
                    />
                </div>
                <div>
                    <label htmlFor="boundingbox" className="mr-2">
                        Bounding Box
                    </label>
                    <input
                        type="checkbox"
                        id="boundingbox"
                        onChange={(e) =>
                            changeRenderSettings({
                                ...settings,
                                showBoundingBox: e.currentTarget.checked,
                            })
                        }
                        checked={settings.showBoundingBox}
                    />
                </div>
            </div>
        </div>
    );
}
