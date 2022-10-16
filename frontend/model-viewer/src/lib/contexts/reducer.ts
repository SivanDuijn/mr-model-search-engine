import { RenderMaterial } from "src/components/model-viewer/viewGL";

export interface ModelState {
    model: {
        file?: string;
        name?: string;
        // class?: string;
        isProcessed?: boolean;
        stats?: {
            className?: string;
            nFaces?: number;
            nVertices?: number;
            boundingBoxSize?: number;
            distBarycenterToOrigin?: number;
            angleX?: number;
            angleY?: number;
            angleZ?: number;
            totalAngle?: number;
            totalFlip?: number;
        };
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
    ChangeModel,
    ChangeRenderSettings,
}

export interface ChangeModelAction {
    type: ActionKind.ChangeModel;
    payload: ModelState["model"];
}

export interface ChangeRenderSettingsAction {
    type: ActionKind.ChangeRenderSettings;
    payload: ModelState["renderSettings"];
}
//

export type Actions = ChangeModelAction | ChangeRenderSettingsAction;

export function modelReducer(state: ModelState, action: Actions) {
    const { type, payload } = action;
    switch (type) {
        case ActionKind.ChangeRenderSettings:
            return {
                ...state,
                ...{ renderSettings: { ...payload } },
            };
        case ActionKind.ChangeModel:
            return {
                ...state,
                ...{ model: { ...payload, stats: { ...payload.stats } } },
            };
        default:
            return state;
    }
}
