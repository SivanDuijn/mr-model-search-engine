import "../styles/globals.css";
import type { AppProps } from "next/app";
import ModelViewer from "../components/model-viewer/ModelViewer";

function MyApp({ Component, pageProps }: AppProps) {
    return <ModelViewer />;
}

export default MyApp;
