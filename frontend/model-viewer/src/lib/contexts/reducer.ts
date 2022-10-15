import { RenderMaterial } from "src/components/model-viewer/viewGL";

export interface ModelState {
    modelInfo: {
        file?: string;
        name?: string;
        class?: string;
        isProcessed?: boolean;
    };
    renderSettings: {
        material: RenderMaterial;
        showWireframe: boolean;
        showVertexNormals: boolean;
        autoRotateEnabled: boolean;
    };
}

// Actions:
export enum ActionKind {
    ChangeModelInfo,
    ChangeRenderSettings,
}

export interface ChangeModelInfoAction {
    type: ActionKind.ChangeModelInfo;
    payload: ModelState["modelInfo"];
}

export interface ChangeRenderSettingsAction {
    type: ActionKind.ChangeRenderSettings;
    payload: ModelState["renderSettings"];
}
//

export type Actions = ChangeModelInfoAction | ChangeRenderSettingsAction;

export function modelReducer(state: ModelState, action: Actions) {
    const { type, payload } = action;
    switch (type) {
        case ActionKind.ChangeRenderSettings:
            return {
                ...state,
                ...{ renderSettings: { ...payload } },
            };
        case ActionKind.ChangeModelInfo:
            return {
                ...state,
                ...{ modelInfo: { ...payload } },
            };
        default:
            return state;
    }
}
