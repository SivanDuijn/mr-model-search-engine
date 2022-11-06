import { RenderMaterial } from "src/components/model-viewer/viewGL";

export interface ModelState {
    model: {
        name?: string;
        text?: string;
        secondModel?: string;
        isProcessed?: boolean;
        top_k?: { name: string; dist: number }[];
    };
    modelStats?: {
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
    modelDescriptors?: {
        area: number;
        AABBVolume: number;
        volume: number;
        compactness: number;
        eccentricity: number;
        diameter: number;
        sphericity: number;
        rectangularity: number;
        A3: number[];
        D1: number[];
        D2: number[];
        D3: number[];
        D4: number[];
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
    ChangeModelStats,
    ChangeModelDescriptors,
    ChangeRenderSettings,
}

export interface ChangeModelAction {
    type: ActionKind.ChangeModel;
    payload: ModelState["model"];
}
export interface ChangeModelStatsAction {
    type: ActionKind.ChangeModelStats;
    payload: ModelState["modelStats"];
}
export interface ChangeModelDescriptorsAction {
    type: ActionKind.ChangeModelDescriptors;
    payload: ModelState["modelDescriptors"];
}

export interface ChangeRenderSettingsAction {
    type: ActionKind.ChangeRenderSettings;
    payload: ModelState["renderSettings"];
}
//

export type Actions =
    | ChangeModelAction
    | ChangeModelStatsAction
    | ChangeModelDescriptorsAction
    | ChangeRenderSettingsAction;

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
                model: payload,
            };
        case ActionKind.ChangeModelStats:
            return {
                ...state,
                modelStats: payload,
            };
        case ActionKind.ChangeModelDescriptors:
            return {
                ...state,
                modelDescriptors: payload,
            };
        default:
            return state;
    }
}
