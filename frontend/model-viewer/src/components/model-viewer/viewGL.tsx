import * as THREE from "three";
import { OrbitControls } from "three/examples/jsm/controls/OrbitControls";
import { VertexNormalsHelper } from "three/examples/jsm/helpers/VertexNormalsHelper";
import { OBJLoader } from "three/examples/jsm/loaders/OBJLoader";

export interface ModelStats {
    nFaces: number;
    nVertices: number;
}

export enum RenderStyle {
    "Shaded",
    "Wireframe",
}

export default class ThreeJSViewGL {
    scene: THREE.Scene;
    renderer: THREE.WebGLRenderer;
    camera: THREE.PerspectiveCamera;
    controls: OrbitControls;
    loader = new OBJLoader();

    model?: THREE.Mesh;
    vertexNormalsHelper?: VertexNormalsHelper;

    // OPTIONS
    renderStyle: RenderStyle = RenderStyle.Wireframe;
    vertexNormalHelperEnabled = false;

    material: THREE.Material = new THREE.Material();

    onModelStatsChanged?: (stats: ModelStats) => void;

    constructor(canvas: HTMLCanvasElement | undefined) {
        this.scene = new THREE.Scene();
        this.renderer = new THREE.WebGLRenderer({
            canvas: canvas,
            antialias: true,
        });
        // objects which are near or far away won't be rendered
        const clippingPlane = [0.1, 1000];
        this.camera = new THREE.PerspectiveCamera(75, 1, ...clippingPlane);

        this.renderer.setSize(600, 600);

        this.updateMaterial();

        const pointLight = new THREE.PointLight(0xdedede, 6, 100);
        pointLight.position.set(40, 10, 50);
        this.scene.add(pointLight);

        const ambientLight = new THREE.AmbientLight(0x404040); // soft white light
        this.scene.add(ambientLight);

        this.controls = new OrbitControls(this.camera, canvas);

        this.camera.position.z = 1;
        this.camera.position.y = 0.3;

        this.loadOBJModelByUrl("models/animal/m279.obj");

        // const l = new PLYLoader();
        // l.load("models/61.ply", (geometry) => {
        //     geometry.computeVertexNormals();
        //     const mat = new THREE.MeshPhongMaterial({
        //         color: this.renderStyle == RenderStyle.Wireframe ? 0x00ff00 : 0xdedede,
        //         wireframe: this.renderStyle == RenderStyle.Wireframe,
        //     });

        //     this.scene.add(new THREE.Mesh(geometry, mat));
        //     console.log(geometry);
        // });

        this.update();
    }

    private updateMaterial() {
        this.material = new THREE.MeshPhongMaterial({
            color: this.renderStyle == RenderStyle.Wireframe ? 0x00ff00 : 0xdedede,
            wireframe: this.renderStyle == RenderStyle.Wireframe,
        });

        if (this.model) this.model.material = this.material;
    }

    private getModelStats() {
        if (!this.model) return;
        const nVertices = this.model.geometry.getAttribute("position").count;
        const nFaces = this.model.geometry.getAttribute("normal").count;
        if (this.onModelStatsChanged) this.onModelStatsChanged({ nVertices, nFaces });
    }
    setOnModelStatsChanged(onModelStatsChanged: (stats: ModelStats) => void) {
        this.onModelStatsChanged = onModelStatsChanged;
    }

    loadModelByText(text: string) {
        const lines = text.split("\n");
        const facesStartLineI = lines.findIndex((line) => line.startsWith("f "));
        const vertices: number[][] = [];
        for (let i = 0; i < facesStartLineI; i++) {
            const coords = lines[i].split(" ");
            vertices.push([parseFloat(coords[1]), parseFloat(coords[2]), parseFloat(coords[3])]);
        }

        const normals: number[] = [];
        const faceIndices: number[] = [];
        for (let i = facesStartLineI; i < lines.length; i++) {
            const indices = lines[i].split(" ");
            if (indices.length === 4) {
                const v1 = parseInt(indices[1]) - 1;
                const v2 = parseInt(indices[2]) - 1;
                const v3 = parseInt(indices[3]) - 1;

                const p1 = vertices[v1];
                const p2 = vertices[v2];
                const p3 = vertices[v3];
                const Ax = p2[0] - p1[0];
                const Ay = p2[1] - p1[1];
                const Az = p2[2] - p1[2];
                const Bx = p3[0] - p1[0];
                const By = p3[1] - p1[1];
                const Bz = p3[2] - p1[2];
                normals.push(Ay * Bz - Az * By, Az * Bx - Ax * Bz, Ax * By - Ay * Bx);
                faceIndices.push(v1, v2, v3);
            }
        }

        const geometry = new THREE.BufferGeometry();
        geometry.setIndex(faceIndices);
        geometry.setAttribute("position", new THREE.Float32BufferAttribute(vertices.flat(), 3));
        geometry.setAttribute("normal", new THREE.Float32BufferAttribute(normals, 3));

        geometry.computeVertexNormals();
        geometry.center();

        if (this.model) this.scene.remove(this.model);
        if (this.vertexNormalsHelper) this.scene.remove(this.vertexNormalsHelper);

        this.model = new THREE.Mesh(geometry, this.material);

        this.vertexNormalsHelper = new VertexNormalsHelper(this.model, 0.05, 0xff0000);
        if (this.vertexNormalHelperEnabled) this.scene.add(this.vertexNormalsHelper);

        this.scene.add(this.model);
        this.getModelStats();
    }

    loadOBJModelByUrl(url: string) {
        fetch(url)
            .then((response) => response.text())
            .then((text) => this.loadModelByText(text));
    }

    setRenderStyle(renderStyle: RenderStyle) {
        this.renderStyle = renderStyle;

        this.updateMaterial();
    }
    setVertexNormalHelper(enable: boolean) {
        if (this.vertexNormalHelperEnabled === enable) return;

        this.vertexNormalHelperEnabled = enable;

        if (this.vertexNormalsHelper)
            if (this.vertexNormalHelperEnabled) this.scene.add(this.vertexNormalsHelper);
            else this.scene.remove(this.vertexNormalsHelper);
    }

    update() {
        this.renderer.render(this.scene, this.camera);
        this.controls.update();

        if (this.model) this.model.rotation.y += 0.007;
        if (this.vertexNormalsHelper) this.vertexNormalsHelper.update();

        requestAnimationFrame(this.update.bind(this));
    }
}
