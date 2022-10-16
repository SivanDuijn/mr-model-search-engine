import clsx from "clsx";
import React, { useMemo } from "react";
import { useModelDescriptors } from "src/lib/contexts/hooks";
import Histogram, { HistogramProps } from "./charts/Histogram";

type Props = {
    className?: string;
};

export default function DescriptorHistograms(props: Props) {
    const { descriptors } = useModelDescriptors();

    const histograms = useMemo(
        () =>
            descriptors
                ? [
                      { title: "A3", color: "#f06d36", data: descriptors.A3 },
                      { title: "D1", color: "#f9ad6e", data: descriptors.D1 },
                      { title: "D2", color: "#f0daad", data: descriptors.D2 },
                      { title: "D3", color: "#84aca3", data: descriptors.D3 },
                      { title: "D4", color: "#288995", data: descriptors.D4 },
                  ]
                : undefined,
        [descriptors],
    );

    return (
        <div className={props.className}>
            <p className={clsx("border-b-2", "text-center", "font-bold")}>Descriptor Histograms</p>
            {histograms && <MemoizedHistograms histograms={histograms} />}
        </div>
    );
}

// eslint-disable-next-line react/display-name
const MemoizedHistograms = React.memo(({ histograms }: { histograms: HistogramProps[] }) => {
    return (
        <div className={clsx("grid", "lg:grid-cols-5", "mr-6", "justify-center")}>
            {histograms.map((histogram) => (
                <Histogram key={histogram.title} {...histogram} />
            ))}
        </div>
    );
});
