import { useContext } from "react";
import {
    ActionKind,
    ChangeModelAction,
    ChangeModelDescriptorsAction,
    ChangeModelStatsAction,
    ChangeRenderSettingsAction,
} from "./reducer";
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

export function useModelStats() {
    const { state, dispatch } = useContext(ModelContext);
    const changeModelStats = (stats: ChangeModelStatsAction["payload"]) => {
        dispatch({ type: ActionKind.ChangeModelStats, payload: stats });
    };
    return { stats: state.modelStats, changeModelStats };
}

export function useModelDescriptors() {
    const { state, dispatch } = useContext(ModelContext);
    const changeModelDescriptors = (descriptors: ChangeModelDescriptorsAction["payload"]) => {
        dispatch({ type: ActionKind.ChangeModelDescriptors, payload: descriptors });
    };
    return { descriptors: state.modelDescriptors, changeModelDescriptors };
}
