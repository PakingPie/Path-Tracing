//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"
using namespace std;


static int ray_count = 0;

void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    } 
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}



// Vector3f Scene::castRay(const Ray &ray, int depth) const
// {
//     Vector3f scattered_origin = ray.origin;
//     Vector3f scattered_direction = ray.direction;
//     Vector3f brightness = Vector3f(1.0, 1.0, 1.0);
//     Vector3f color_buffer = Vector3f(0.0, 0.0, 0.0);

//     bool sample_on_unit_sphere_surface = true;
//     ray_count ++;
//     for(int i = 0; i < 20; i++)
//     {
//         if(get_random_float() > RussianRoulette)
//             break;
        
//         Ray new_ray(scattered_origin, scattered_direction);
//         Intersection hit = intersect(new_ray);

//         bool is_hit = hit.happened;

//         if(is_hit)
//         {
//             // if(ray_count<1000)
//             // cout<<ray_count<<"'s light: "<<scattered_origin<<endl;
//             bool front_face = false;
//             Vector3f hit_point = hit.coords;
//             MaterialType material = hit.m->m_type; 
//             Vector3f color = hit.m->Kd;
//             Vector3f hit_point_normal = hit.normal.normalized();

//             if(dotProduct(scattered_direction,  hit_point_normal) < 0)
//                 front_face = true;
//             else
//                 hit_point_normal = -hit_point_normal;
//             float refraction_ratio = hit.m->ior;
//             if(hit.m->hasEmission())
//             {
//                 color_buffer = hit.m->getColor() * brightness;
//                 break;
//             }
//             else
//             {
//                 if(material == DIFFUSE)
//                 {
//                     Vector3f target = hit_point + hit_point_normal;

//                     if(sample_on_unit_sphere_surface)
//                         target += random_unit_vector();
//                     else
//                         target += random_in_unit_sphere();

//                     scattered_direction = target - hit_point;
//                     scattered_origin = hit_point;
//                     brightness = color * brightness;
//                 }
//                 else if(material == MIRROR)
//                 {
//                     scattered_direction = reflect(scattered_direction.normalized(), hit_point_normal);
//                     if(sample_on_unit_sphere_surface)
//                         scattered_direction += hit.m->roughness * random_unit_vector();
//                     else
//                         scattered_direction += hit.m->roughness * random_in_unit_sphere();
//                     scattered_origin = hit_point;
//                     if(dotProduct(scattered_direction, hit_point_normal) < 0)
//                     {
//                         break;
//                     }
//                     else
//                         brightness = color * brightness;
//                 }
//                 else if(material == GLASS)
//                 {
//                     if(front_face)
//                         refraction_ratio = 1 / refraction_ratio;
//                     float cos_theta = min(-dotProduct(ray.direction, hit_point_normal), 1.0f);
//                     float sin_theta = sqrt(1 - cos_theta * cos_theta);
                
//                     if(refraction_ratio * sin_theta > 1.0f || reflectance(cos_theta, refraction_ratio) > get_random_float()) // Reflection
//                     {
//                         scattered_direction = reflect(scattered_direction.normalized(), hit_point_normal);
//                     }
//                     else
//                     {
//                         scattered_direction = my_refract(scattered_direction.normalized(), hit_point_normal, refraction_ratio);
//                     }

//                     scattered_origin = hit_point;
//                     brightness = color * brightness; 
//                 }

//                 brightness = brightness / RussianRoulette;
//             }
//         }
//     }
//     return color_buffer;
// }


// // Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    if(depth > 8)
        return Vector3f();
    // TO DO Implement Path Tracing Algorithm here
    Intersection p_inter = intersect(ray);

    if(!p_inter.happened)
        return Vector3f();
    if(p_inter.m->hasEmission())
        return p_inter.m->getEmission();

    Vector3f l_dir = 0;
    Vector3f l_indir = 0;

    Vector3f p = p_inter.coords;
    Vector3f N = p_inter.normal.normalized();

    switch (p_inter.m->m_type)
    {
        case DIFFUSE:
        {
            MaterialType temp_m = DIFFUSE;
            Intersection x_inter;
            float pdf_light = 0.0f;

            // Get a ray 
            sampleLight(x_inter, pdf_light);

            Vector3f x = x_inter.coords;
            Vector3f NN = x_inter.normal.normalized();
            Vector3f emit = x_inter.emit;

            float ws_distance = (x - p).norm();
            Vector3f ws = (x - p).normalized();
            // Shoot a ray from p to x
            Intersection ws_p_to_x_intersection = intersect(Ray(p, ws));

            if(ws_p_to_x_intersection.distance - ws_distance > -0.001)
            {
                l_dir = emit * p_inter.m->eval(ray.direction, ws, N, temp_m) * dotProduct(ws, N) * dotProduct(-ws, NN) / std::pow(ws_distance, 2) / pdf_light;
            }

            if(get_random_float() > RussianRoulette)
                return l_dir;
            
            Vector3f wi = p_inter.m->sample(ray.direction, N, temp_m).normalized();
            Intersection wi_p_inter = intersect(Ray(p, wi));

            if(wi_p_inter.happened && p_inter.m->pdf(ray.direction, wi, N, temp_m) > EPSILON)
            {
                l_indir = castRay(Ray(p, wi), depth + 1) * p_inter.m->eval(ray.direction, wi, N, temp_m) * dotProduct(wi, N) / p_inter.m->pdf(ray.direction, wi, N, temp_m) / RussianRoulette;
            }
            break;
        }
        case MIRROR: // MIRROR
        {
            MaterialType temp_m = MIRROR;
            if(get_random_float() > RussianRoulette)
                return l_dir;

            Vector3f wi = p_inter.m->sample(ray.direction, N, temp_m).normalized();
            Intersection wi_p_inter = intersect(Ray(p, wi));

            if(wi_p_inter.happened && p_inter.m->pdf(ray.direction, wi, N, temp_m) > EPSILON)
            {
                l_indir = castRay(Ray(p, wi), depth + 1) * p_inter.m->eval(ray.direction, wi, N, temp_m) * dotProduct(wi, N) / p_inter.m->pdf(ray.direction, wi, N, temp_m) / RussianRoulette;
            }
            break;
        }
        case GLASS:
        {
            MaterialType temp_m = DIFFUSE;
            
            bool isFrontFace = false;

            if(dotProduct(ray.direction,  N) < 0)
                isFrontFace = true;
            else
                N = -N;

            
            if(get_random_float() > RussianRoulette)
                return l_dir;


            float refraction_ratio = p_inter.m->ior;
            if(isFrontFace)
                refraction_ratio = 1 / refraction_ratio;

            float random_number = get_random_float();

            float cos_theta = min(-dotProduct(ray.direction, N), 1.0f);
            float sin_theta = sqrt(1 - cos_theta * cos_theta);
            Vector3f wi;
            if(refraction_ratio * sin_theta > 1.0 || reflectance(cos_theta, refraction_ratio) > get_random_float())
            {
                wi = reflect(ray.direction, N).normalized();
                temp_m = MIRROR;
            }
            else
            {
                wi = my_refract(ray.direction, N, refraction_ratio).normalized();
                temp_m = GLASS;
                N = -N;
            }

            Ray scattered_ray(p, wi);
            Intersection scattered_inter = intersect(scattered_ray);

            if(scattered_inter.happened && p_inter.m->pdf(ray.direction, wi, N, temp_m) > EPSILON)
            {
                l_indir = castRay(scattered_ray, depth + 1) * p_inter.m->eval(ray.direction, wi, N, temp_m) * dotProduct(wi, N) / p_inter.m->pdf(ray.direction, wi, N, temp_m) / RussianRoulette;
            }

            // BxDF: eval / pdf
            // if(temp_m == DIFFUSE)
            // {
            //     l_dir = Vector3f(0.f, 0.f, l_dir.z);
            //     l_indir = Vector3f(0.f, 0.f, l_indir.z);
            // }
            // else if(temp_m == MIRROR)
            // {
            //     l_dir = Vector3f(l_dir.x, 0.f, 0.f);
            //     l_indir = Vector3f(l_indir.x, 0.f, 0.f);
            // }
            // else
            // {
            //     l_dir = Vector3f(0.f, l_dir.y, 0.f);
            //     l_indir = Vector3f(0.f, l_indir.y,0.f);
            // }
            break;
        }                                                    

    }
    
    if(p_inter.m->m_type == GLASS)
    {
        return l_dir * p_inter.m->Ks + l_indir * p_inter.m->Kd;
    }
    else
    return l_dir + l_indir;
}


Vector3f Scene::lightTrace(const Ray &ray, int depth) const
{
    Intersection l_inter = intersect(ray);
    if(!l_inter.happened)
        return Vector3f();
    if(l_inter.m->hasEmission())
        return l_inter.m->m_emission;

    Vector3f l = l_inter.coords;
    Vector3f N = l_inter.normal.normalized();

}