//
// Created by goksu on 2/25/20.
//

#include <fstream>
#include "Scene.hpp"
#include "Renderer.hpp"

#include <thread>
#include <mutex>

inline float deg2rad(const float& deg) { return deg * M_PI / 180.0; }

const float EPSILON = 0.00001;

std::mutex _mutex;
int progress = 0;
// The main render function. This where we iterate over all pixels in the image,
// generate primary rays and cast these rays into the scene. The content of the
// framebuffer is saved to a file.
void Renderer::Render(const Scene& scene, int spp)
{
    std::vector<Vector3f> framebuffer(scene.width * scene.height);

    float scale = tan(deg2rad(scene.fov * 0.5));
    float imageAspectRatio = scene.width / (float)scene.height;
    Vector3f eye_pos(278, 273, -800);
    // change the spp value to change sample ammount
    std::cout << "SPP: " << spp << "\n";

    int thread_size = 32;
    std::thread th[thread_size];
    int thread_height = scene.height / thread_size;
    float step = 1.0 / std::sqrt(spp);
    auto renderThread = [&](uint32_t start, uint32_t end) 
    {
        for (uint32_t j = start; j < end; ++j) {
            for (uint32_t i = 0; i < scene.width; ++i) {
                // generate primary ray direction
                for (int k = 0; k < spp; k++) {
                    // float x = (2 * (i + step / 2 + step * (k % (int)std::sqrt(spp))) / (float)scene.width - 1) * imageAspectRatio * scale;
                    // float y = (1 - 2 * (j + step / 2 + step * (k / (int)std::sqrt(spp))) / (float)scene.height) * scale;

                    float x = (2 * (i + get_random_float() + step / 2 + step * (k % (int)std::sqrt(spp))) / (float)scene.width - 1) * imageAspectRatio * scale;
                    float y =  (1 - 2 * (j + get_random_float() + step / 2 + step * (k / (int)std::sqrt(spp))) / (float)scene.height) * scale;
                    Vector3f dir = normalize(Vector3f(-x, y, 1));
                    
                    framebuffer[j * scene.width + i] += scene.castRay(Ray(eye_pos, dir), 0) / spp;  
                }
                // framebuffer[j * scene.width + i] = framebuffer[j * scene.width + i] *40;
            }
            _mutex.lock();
            progress++;
            UpdateProgress(progress / (float)scene.height);
            _mutex.unlock();
        }
    };

    for(int i = 0; i<thread_size; ++i)
    {
        th[i] = std::thread(renderThread, i * thread_height, (i + 1) * thread_height);
    }

    for(int i = 0; i<thread_size; ++i)
    {
        th[i].join();
    }


    UpdateProgress(1.f);

    // save framebuffer to file
    FILE* fp = fopen("binary.ppm", "wb");
    (void)fprintf(fp, "P6\n%d %d\n255\n", scene.width, scene.height);
    for (auto i = 0; i < scene.height * scene.width; ++i) {
        static unsigned char color[3];
        color[0] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].x), 0.6f));
        color[1] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].y), 0.6f));
        color[2] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].z), 0.6f));
        fwrite(color, 1, 3, fp);
    }
    fclose(fp);    
}


void Renderer::RenderLight(const Scene& scene, int spp)
{
    std::vector<Vector3f> framebuffer(scene.width * scene.height);

    float scale = tan(deg2rad(scene.fov * 0.5));
    float imageAspectRatio = scene.width / (float)scene.height;

    Vector3f light_pos;
    
    float x_min = 213.f;
    float x_max = 343.f;
    float z_min = 227.f;
    float z_max = 332.f;

    int thread_size = 32;
    std::thread th[thread_size];
    int thread_height = (z_max - z_min) / thread_size;
    float step = 1.0 / std::sqrt(spp);

    
    for (uint32_t j = z_min; j < z_max; ++j) {
            for (uint32_t i = x_min; i < x_max; ++i) {
                for (int k = 0; k < spp; k++) {
                    float x = (float)i + get_random_float();
                    float z = (float)j + get_random_float();
                    Vector3f dir = normalize(Vector3f(x, -1, z));
                    
                }
            }
        }
    // auto renderThread = [&](uint32_t start, uint32_t end) 
    // {
    //     for (uint32_t j = start; j < end; ++j) {
    //         for (uint32_t i = x_min; i < x_max; ++i) {
    //             for (int k = 0; k < spp; k++) {
    //                 float x = (float)i + get_random_float();
    //                 float z = (float)j + get_random_float();
    //                 Vector3f dir = normalize(Vector3f(x, -1, z));
    //             }
    //         }
    //     }
    // };

    // for(int i = 0; i<thread_size; ++i)
    // {
    //     th[i] = std::thread(renderThread, i * thread_height, (i + 1) * thread_height);
    // }

    // for(int i = 0; i<thread_size; ++i)
    // {
    //     th[i].join();
    // }


    // UpdateProgress(1.f);

    // // save framebuffer to file
    // FILE* fp = fopen("binary.ppm", "wb");
    // (void)fprintf(fp, "P6\n%d %d\n255\n", scene.width, scene.height);
    // for (auto i = 0; i < scene.height * scene.width; ++i) {
    //     static unsigned char color[3];
    //     color[0] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].x), 0.6f));
    //     color[1] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].y), 0.6f));
    //     color[2] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].z), 0.6f));
    //     fwrite(color, 1, 3, fp);
    // }
    // fclose(fp);  
}