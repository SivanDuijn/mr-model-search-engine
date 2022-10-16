import { createContext, ReactElement, useReducer, Dispatch } from "react";
import { RenderMaterial } from "src/components/model-viewer/viewGL";
import { Actions, modelReducer, ModelState } from "./reducer";

export const initialState: ModelState = {
    model: {
        name: "125.off",
        isProcessed: true,
    },
    renderSettings: {
        material: RenderMaterial.Flat,
        showWireframe: true,
        showVertexNormals: false,
        autoRotateEnabled: true,
    },
};

export const ModelContext = createContext<{
    state: ModelState;
    dispatch: Dispatch<Actions>;
}>({
    state: initialState,
    dispatch: () => null,
});

export const ModelProvider = ({ children }: { children: ReactElement }) => {
    const [state, dispatch] = useReducer(modelReducer, initialState);

    return <ModelContext.Provider value={{ state, dispatch }}>{children}</ModelContext.Provider>;
};
