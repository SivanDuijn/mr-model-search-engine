import * as THREE from "three";
import { OrbitControls } from "three/examples/jsm/controls/OrbitControls";
import { OBJLoader } from "three/examples/jsm/loaders/OBJLoader";

export default class ViewGL {
    scene: THREE.Scene;
    renderer: THREE.WebGLRenderer;
    camera: THREE.PerspectiveCamera;
    controls: OrbitControls;
    loader = new OBJLoader();

    model: THREE.Group | null = null;

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

        const pointLight = new THREE.PointLight(0xffffff, 10, 100);
        pointLight.position.set(50, 50, 50);
        this.scene.add(pointLight);

        const ambientLight = new THREE.AmbientLight(0x404040); // soft white light
        this.scene.add(ambientLight);

        this.controls = new OrbitControls(this.camera, canvas);

        this.camera.position.z = 1;
        this.camera.position.y = 0.3;

        this.loadModelByUrl("models/animal/m279.obj");

        this.update();
    }

    // updateValue(value) {
    //     // Whatever you need to do with React props
    // }

    // onMouseMove() {
    //     // Mouse moves
    // }

    // onWindowResize(vpW, vpH) {
    //     this.renderer.setSize(vpW, vpH);
    // }

    loadModel(content: string) {
        if (this.model) this.scene.remove(this.model);
        this.model = this.model = this.loader.parse(content);

        const mat = new THREE.MeshStandardMaterial({ color: 0x00ff00, wireframe: true });
        this.model.traverse((child) => {
            if (child instanceof THREE.Mesh) {
                child.geometry.center();
                child.material = mat;
            }
        });

        this.scene.add(this.model);
    }

    loadModelByUrl(url: string) {
        this.loader.load(
            url,
            (model) => {
                const mat = new THREE.MeshStandardMaterial({ color: 0x00ff00, wireframe: true });
                model.traverse((child) => {
                    if (child instanceof THREE.Mesh) {
                        child.geometry.center();
                        child.material = mat;
                    }
                });

                if (this.model) this.scene.remove(this.model);
                this.model = model;
                this.scene.add(model);
            },
            () => true,
            (e) => console.error("Something went wrong when loading model!", e),
        );
    }

    update() {
        this.renderer.render(this.scene, this.camera);
        this.controls.update();

        if (this.model) this.model.rotation.y += 0.007;

        requestAnimationFrame(this.update.bind(this));
    }
}
