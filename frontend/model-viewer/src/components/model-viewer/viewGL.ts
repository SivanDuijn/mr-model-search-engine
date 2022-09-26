import * as THREE from "three";
import { OrbitControls } from "three/examples/jsm/controls/OrbitControls";
import { VertexNormalsHelper } from "three/examples/jsm/helpers/VertexNormalsHelper";
import { OBJLoader } from "three/examples/jsm/loaders/OBJLoader";
import LoadOBJModel from "../../lib/OBJLoader";

export interface ModelStats {
    nFaces: number;
    nVertices: number;
}

export enum Material {
    "Phong",
    "Hidden",
}

export default class ThreeJSViewGL {
    scene: THREE.Scene;
    renderer: THREE.WebGLRenderer;
    camera: THREE.PerspectiveCamera;
    controls: OrbitControls;
    loader = new OBJLoader();

    mesh?: THREE.Mesh;
    wireframe?: THREE.LineSegments;
    vertexNormalsHelper?: VertexNormalsHelper;

    material: THREE.Material = new THREE.Material();

    // OPTIONS
    renderMaterial: Material = Material.Phong;
    vertexNormalsEnabled = false;
    wireframeEnabled = true;

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
        if (window) this.renderer.setPixelRatio(window.devicePixelRatio);

        const pointLight = new THREE.PointLight(0xffffff, 3, 100);
        pointLight.position.set(50, 20, 40);
        this.scene.add(pointLight);

        const ambientLight = new THREE.AmbientLight(0x404040); // soft white light
        this.scene.add(ambientLight);

        this.controls = new OrbitControls(this.camera, canvas);

        this.camera.position.z = 1;
        this.camera.position.y = 0.3;

        this.updateMaterial();
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
            color: 0xdedede,
            polygonOffset: true,
            polygonOffsetFactor: 1,
            polygonOffsetUnits: 1,
        });

        if (this.mesh) this.mesh.material = this.material;
    }

    private getModelStats() {
        if (!this.mesh) return;
        const nVertices = this.mesh.geometry.getAttribute("position").count;
        const nFaces = this.mesh.geometry.getAttribute("normal").count;
        if (this.onModelStatsChanged) this.onModelStatsChanged({ nVertices, nFaces });
    }
    setOnModelStatsChanged(onModelStatsChanged: (stats: ModelStats) => void) {
        this.onModelStatsChanged = onModelStatsChanged;
    }

    loadModelByText(text: string) {
        if (this.mesh) this.scene.remove(this.mesh);

        const geometry = LoadOBJModel(text);
        this.mesh = new THREE.Mesh(geometry, this.material);
        this.scene.add(this.mesh);

        this.vertexNormalsHelper = new VertexNormalsHelper(this.mesh, 0.05, 0xff0000);
        if (this.vertexNormalsEnabled) this.scene.add(this.vertexNormalsHelper);

        this.wireframe = new THREE.LineSegments(
            new THREE.WireframeGeometry(this.mesh.geometry),
            new THREE.LineBasicMaterial({ color: 0x00fff0 }),
        );
        if (!this.wireframeEnabled) this.wireframe.visible = false;
        this.mesh.add(this.wireframe);

        this.getModelStats();
    }

    loadOBJModelByUrl(url: string) {
        fetch(url)
            .then((response) => response.text())
            .then((text) => this.loadModelByText(text));
    }

    // SET OPTIONS FUNCTIONS
    setRenderStyle(renderStyle: Material) {
        this.renderMaterial = renderStyle;

        this.updateMaterial();
    }
    showVertexNormals(show: boolean) {
        if (this.vertexNormalsEnabled == show) return;

        this.vertexNormalsEnabled = show;

        if (this.vertexNormalsHelper)
            if (this.vertexNormalsEnabled) this.scene.add(this.vertexNormalsHelper);
            else this.scene.remove(this.vertexNormalsHelper);
    }
    showWireframe(show: boolean) {
        if (this.wireframeEnabled == show) return;

        this.wireframeEnabled = show;
        if (this.wireframe)
            if (this.wireframeEnabled) this.wireframe.visible = true;
            else this.wireframe.visible = false;
    }
    // ---

    update() {
        this.renderer.render(this.scene, this.camera);
        this.controls.update();

        if (this.mesh) this.mesh.rotation.y += 0.007;
        if (this.vertexNormalsHelper) this.vertexNormalsHelper.update();

        requestAnimationFrame(this.update.bind(this));
    }
}
