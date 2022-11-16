// Next.js API route support: https://nextjs.org/docs/api-routes/introduction
import type { NextApiRequest, NextApiResponse } from "next";

export const config = {
    api: {
        bodyParser: {
            sizeLimit: "100mb",
        },
        responseLimit: false,
        externalResolver: true,
    },
};

export default async function handler(req: NextApiRequest, res: NextApiResponse) {
    const data = await fetch("http://127.0.0.1:5000/api/query", {
        method: "POST",
        body: req.body,
        mode: "cors",
    }).then((r) => r.json());
    // console.log(data);

    res.json(data);
}
