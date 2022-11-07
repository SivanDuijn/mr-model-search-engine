import { AppProps } from "next/app";
import React from "react";
import { Toaster } from "react-hot-toast";
import { ModelProvider } from "src/lib/contexts";
import "../styles/globals.css";

function MyApp({ Component, pageProps }: AppProps) {
    return (
        <ModelProvider>
            <React.Fragment>
                <Component {...pageProps} />
                <Toaster />
            </React.Fragment>
        </ModelProvider>
    );
}

export default MyApp;
