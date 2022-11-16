## Frontend for the Multimedia Retrieval assignment

Build with NextJS, more info about running and deploying here 
[https://nextjs.org/docs](https://nextjs.org/docs)

# Getting Started

First, run the development server:

```bash
npm run dev
# or
yarn dev
```

Open [http://localhost:3000](http://localhost:3000) with your browser to see the result.

# URL parameters

- `m` is used to select a model, default 125.off
- `mat` to select a material, can be one of `flat, phong, normals, pointcloud, wireframe, cartoon`, default `flat`
- `wireframe` to enable wireframe, 0 or 1, default 1
- `vnormals` to enable vertex normals visualization, 0 or 1, default 0
- `rotate` to enable auto rotation, 0 or 1, default 1

for example: `http://localhost/?m=125_processed.off&mat=pointcloud&wireframe=0&vnormals=1&rotate=1`