#include "Renderer.hpp"
#include "Scene.hpp"
#include "Triangle.hpp"
#include "Sphere.hpp"
#include "Vector.hpp"
#include "global.hpp"
#include <chrono>

// In the main function of the program, we create the scene (create objects and
// lights) as well as set the options for the render (image width and height,
// maximum recursion depth, field-of-view, etc.). We then call the render
// function().
int main(int argc, char** argv)
{

    // Change the definition here to change resolution
    // Scene scene(784, 784);
    Scene scene(748, 748);

    Material* red = new Material(DIFFUSE, Vector3f(0.0f));
    red->Kd = Vector3f(0.63f, 0.065f, 0.05f);
    Material* green = new Material(DIFFUSE, Vector3f(0.0f));
    green->Kd = Vector3f(0.14f, 0.45f, 0.091f);
    Material* white = new Material(DIFFUSE, Vector3f(0.0f));
    white->Kd = Vector3f(0.725f, 0.71f, 0.68f);
    Material* light = new Material(DIFFUSE, (8.0f * Vector3f(0.747f+0.058f, 0.747f+0.258f, 0.747f) + 15.6f * Vector3f(0.740f+0.287f,0.740f+0.160f,0.740f) + 18.4f *Vector3f(0.737f+0.642f,0.737f+0.159f,0.737f)));
    light->Kd = Vector3f(0.65f);

    Material* blue = new Material(DIFFUSE, Vector3f(0.0f));
    blue->Kd = Vector3f(0.25f, 0.31f, 0.98f);
    Material* blue1 = new Material(DIFFUSE, Vector3f(0.0f));
    blue1->Kd = Vector3f(0.10f, 0.10f, 0.75f);
    Material* blue2 = new Material(DIFFUSE, Vector3f(0.0f));
    blue2->Kd = Vector3f(0.05f, 0.15f, 0.75f);

    Material* mirror = new Material(MIRROR, Vector3f(0.0f));
    mirror->Kd = Vector3f(0.3f, 0.3f, 0.25f);
    mirror->Ks = Vector3f(0.45f, 0.45f, 0.45f);
    mirror->roughness = 0.3;

    Material* glass1 = new Material(GLASS, Vector3f(0.0f));
    glass1->Kd = Vector3f(1.0f, 0.8f, 0.7f);
    glass1->Ks = Vector3f(0.45f, 0.45f, 0.45f);
    glass1->ior = 1.5f;
    glass1->roughness = 0.2;

    Material* glass2 = new Material(GLASS, Vector3f(0.0f));
    glass2->Kd = Vector3f(0.8f, 1.0f, 0.7f);
    glass2->Ks = Vector3f(0.45f, 0.45f, 0.45f);
    glass2->ior = 1.5f;
    glass1->roughness = 0.5;

    Material* glass3 = new Material(GLASS, Vector3f(0.0f));
    glass3->Kd = Vector3f(0.7f, 0.8f, 1.0f);
    glass3->Ks = Vector3f(0.45f, 0.45f, 0.45f);
    glass3->ior = 1.5f;
    glass1->roughness = 0.8;

    int spp = 400;

    MeshTriangle floor("../models/cornellbox/floor.obj", white);
    MeshTriangle shortbox("../models/cornellbox/shortbox.obj", glass1);
    MeshTriangle tallbox("../models/cornellbox/tallbox.obj", glass2);
    MeshTriangle left("../models/cornellbox/left.obj", red);
    MeshTriangle right("../models/cornellbox/right.obj", green);
    MeshTriangle light_("../models/cornellbox/light.obj", light);
    MeshTriangle bunny("../models/bunny/bunny.obj", Vector3f(300, 0, 300), Vector3f(2000, 2000, 2000), glass1);

    MeshTriangle shortBoxGlass("../models/cornellbox/shortbox.obj", glass1);

    Sphere sphere_1(Vector3f(150, 100, 350), 70, glass3);
    Sphere sphere_2(Vector3f(360, 160, 350), 110, glass2);
    Sphere sphere_3(Vector3f(200, 180, 150), 90, glass1);

    scene.Add(&floor);
    // scene.Add(&shortbox); 
    // scene.Add(&tallbox);
    // scene.Add(&shortBoxGlass);
    // scene.Add(&sphere_1);
    // scene.Add(&sphere_2);
    // scene.Add(&sphere_3);
    scene.Add(&bunny);
    scene.Add(&left);
    scene.Add(&right);
    scene.Add(&light_);

    scene.buildBVH();

    Renderer r;

    auto start = std::chrono::system_clock::now();
    r.Render(scene, spp);
    auto stop = std::chrono::system_clock::now();

    std::cout << "Render complete: \n";
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::hours>(stop - start).count() << " hours\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::minutes>(stop - start).count() << " minutes\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " seconds\n";

    return 0;
}