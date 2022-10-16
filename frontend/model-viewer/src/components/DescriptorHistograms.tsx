import clsx from "clsx";
import React, { useMemo } from "react";
import { useModelDescriptors } from "src/lib/contexts/hooks";
import Histogram, { HistogramProps } from "./charts/Histogram";

type Props = {
    className?: string;
};

export default function DescriptorHistograms(props: Props) {
    const { descriptors } = useModelDescriptors();

    const getRandomData = () =>
        Array(20)
            .fill(0)
            .map(() => Number(Math.random().toFixed(2)));

    const histograms = useMemo(
        () => [
            { title: "A3", color: "#f06d36", data: getRandomData() },
            { title: "D1", color: "#f9ad6e", data: getRandomData() },
            { title: "D2", color: "#f0daad", data: getRandomData() },
            { title: "D3", color: "#84aca3", data: getRandomData() },
            { title: "D4", color: "#288995", data: getRandomData() },
        ],
        [descriptors],
    );

    return (
        <div className={props.className}>
            <p className={clsx("border-b-2", "text-center", "font-bold")}>Descriptor Histograms</p>
            {descriptors && <MemoizedHistograms histograms={histograms} />}
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
