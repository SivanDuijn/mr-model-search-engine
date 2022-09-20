import clsx from "clsx";
import { RenderStyle } from "./model-viewer/viewGL";

type SettingsProps = {
    onRenderStyleChanged: (renderStyle: RenderStyle) => void;
    className?: string;
};

export default function Settings(props: SettingsProps) {
    return (
        <div className={props.className}>
            <p className={clsx("border-b-2", "text-center")}>Settings</p>
            <div className={clsx("p-2")}>
                <label htmlFor="renderstyle" className="mr-2">
                    Wireframe
                </label>
                <input
                    type="checkbox"
                    id="renderstyle"
                    onChange={(e) =>
                        props.onRenderStyleChanged(
                            e.currentTarget.checked ? RenderStyle.Wireframe : RenderStyle.Shaded,
                        )
                    }
                    defaultChecked
                />
            </div>
        </div>
    );
}
