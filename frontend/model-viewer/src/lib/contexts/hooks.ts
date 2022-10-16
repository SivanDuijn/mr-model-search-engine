import { useContext } from "react";
import { ActionKind, ChangeModelAction, ChangeRenderSettingsAction } from "./reducer";
import { ModelContext } from ".";

export function useRenderSettings() {
    const { state, dispatch } = useContext(ModelContext);

    const changeRenderSettings = (settings: ChangeRenderSettingsAction["payload"]) => {
        dispatch({ type: ActionKind.ChangeRenderSettings, payload: settings });
    };

    return { settings: state.renderSettings, changeRenderSettings };
}

export function useModel() {
    const { state, dispatch } = useContext(ModelContext);

    const changeModel = (model: ChangeModelAction["payload"]) => {
        dispatch({ type: ActionKind.ChangeModel, payload: model });
    };

    return { model: state.model, changeModel };
}
