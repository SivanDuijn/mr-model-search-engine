import { useContext } from "react";
import { ActionKind, ChangeRenderSettingsAction } from "./reducer";
import { ModelContext } from ".";

export function useRenderSettings() {
    const { state, dispatch } = useContext(ModelContext);

    const changeRenderSettings = (settings: ChangeRenderSettingsAction["payload"]) => {
        dispatch({ type: ActionKind.ChangeRenderSettings, payload: settings });
    };

    return { settings: state.renderSettings, changeRenderSettings };
}
