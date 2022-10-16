import modelDescriptors from "public/PSBDatabase/featureDescriptors.json";
import { ModelState } from "./contexts/reducer";

export default function GetModelDescriptors(
    modelName?: string,
    isProcessed?: boolean,
): ModelState["modelDescriptors"] {
    if (!isProcessed || modelName == undefined) return;

    const ws = modelName.split(".");
    const name = ws[0] + "_processed." + ws[1];

    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    const descriptors = (modelDescriptors as any)[name];
    return descriptors;
}
