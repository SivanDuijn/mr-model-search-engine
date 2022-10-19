const ApexChart = dynamic(() => import("react-apexcharts"), { ssr: false });
import clsx from "clsx";
import dynamic from "next/dynamic";
import { Props as ApexProps } from "react-apexcharts";

type Options = Omit<NonNullable<ApexProps["options"]>, "series">;

export type HistogramProps = {
    title?: string;
    data: number[];
    color?: string;
};

export default function Histogram(props: HistogramProps) {
    const options: Options = {
        plotOptions: {
            bar: {
                horizontal: false,
            },
        },
        dataLabels: { enabled: false },
        xaxis: {
            labels: { show: false },
            axisTicks: { show: false },
            title: { text: props.title, offsetY: -10 },
        },
        yaxis: {
            decimalsInFloat: 2,
        },
        tooltip: { enabled: false },
        grid: { borderColor: "grey" },
        chart: {
            animations: {
                enabled: true,
                easing: "easeout",
                speed: 400,
                animateGradually: {
                    enabled: true,
                    delay: 50,
                },
                dynamicAnimation: {
                    enabled: true,
                    speed: 200,
                },
            },
            toolbar: {
                show: false,
            },
            foreColor: "white",
            zoom: { enabled: false },
        },
    };

    return (
        <div className={clsx("max-w-lg")}>
            <ApexChart
                options={options}
                series={[{ name: "", data: props.data, color: props.color }]}
                type={"bar"}
            />
        </div>
    );
}
