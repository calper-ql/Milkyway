//
// Created by calper on 4/23/19.
//

#ifndef MILKYWAYVR_UTIL_H
#define MILKYWAYVR_UTIL_H

#include <optix.h>
#include <optixu/optixu_math_namespace.h>
#include <optixu/optixu_matrix_namespace.h>
#include "medium.h"

#define PI_HALF 1.57079632679
#define PI 3.14159265359
#define PI4 12.5663706144

__forceinline__ __device__ float calculate_sphere_shadow(optix::float3 origin, optix::float3 point,
                                                         optix::float3 light, float blend, float offset){
    optix::float3 li = light - origin;
    optix::float3 po = point - origin;
    float costheta = -optix::dot(optix::normalize(li), optix::normalize(po));
    float shadow = (costheta+blend)/(2.0*blend);
    if(shadow < 0.0){
        shadow = 0.0;
    } else if(shadow > 1.0) {
        shadow = 1.0;
    }

    return shadow;
}

__forceinline__ __device__ optix::float3 calculate_p(optix::float3 pa, optix::float3 pb, float s){
    return pa + (pb-pa) * (s*0.999+0.0001);
}

__forceinline__ __device__ float solve_for_pc_S(float cx, float cy, float cz, float px, float py, float pz, float H){
    float dsqr = ((cx*cx)+(cy*cy)+(cz*cz)) + 0.000001;
    float t_sqrt = (abs(pow((2.0*px*cx)+(2.0*py*cy)+(2.0*pz*cz),2.0) - (4.0*dsqr*((px*px)+(py*py)+(pz*pz)-(H*H)))));
    return ((0.5*t_sqrt)-(px*cx)-(py*cy)-(pz*cz))/dsqr;
}

__forceinline__ __device__ float density_altitude(float alt, float h0){
    if(alt < 0.0) alt = 0.0;
    float c = exp(-alt/(h0));
    return c;
}

__forceinline__ __device__ float integrate_density_altitude(float h0, optix::float3 origin, optix::float3 start, optix::float3 end, float ground_radius, int count){
    float sum = 0.0;
    float fdist = optix::length(start-end);
    for(int i = 0; i < count; i++){
        optix::float3 p = calculate_p(start, end, i/(float)count);
        float alt = optix::length(p-origin) - ground_radius;
        sum += density_altitude(alt, h0);
    }
    return (sum*fdist)/((float)count);
}

__forceinline__ __device__ optix::float3 integrate_scatter(optix::float3 light_source, float light_intensity, optix::float3 origin,
                                                           optix::float3 pa, optix::float3 pb, float h0, float rayleigh, float mia, optix::float3 inv_wave,
                                                           float ground_radius, float atmosphere_radius,  int count){
    optix::float3 sum = optix::make_float3(0.0);
    float pc_altitude = atmosphere_radius - ground_radius;
    for(int i = 0; i < count; i++){
        optix::float3 p = calculate_p(pa, pb, i/(float)count);
        float ddot = optix::dot(optix::normalize(pa-p), optix::normalize(p-light_source));
        float phase = 0.75 * (1.0+ddot*ddot);
        optix::float3 pc_dir = optix::normalize(light_source-p);
        float S = abs(solve_for_pc_S(pc_dir.x, pc_dir.y, pc_dir.z, p.x, p.y, p.z, (ground_radius+pc_altitude)));
        optix::float3 pc = p + S * pc_dir;
        float rK4pi = rayleigh * PI4 * light_intensity;
        float mK4pi = mia * PI4;
        optix::float3 iwK4pi = inv_wave * rK4pi + mK4pi;
        float tpa = integrate_density_altitude(h0, origin, p, pa, ground_radius, count);
        float tpc = integrate_density_altitude(h0, origin, pc, p, ground_radius, count);
        float shadow = calculate_sphere_shadow(origin, p, light_source, 0.04, 0.0);
        sum += h0 * phase * density_altitude((optix::length(origin - p) - ground_radius), h0)
                * optix::expf(-(tpa * iwK4pi)-(tpc * iwK4pi)) * shadow;
    }
    return (sum*optix::length(pa-pb))/((float)count);
}

__forceinline__ __device__ optix::float3 atmospheric_scatter(optix::float3 light_source, float light_intensity, float h0, float rayleigh, float mia,
        optix::float3 origin, optix::float3 pa, optix::float3 pb,  float ground_radius, float atmosphere_radius){
    optix::float3 inv_wave = 1.0/optix::make_float3(pow(667.5,4), pow(540,4), pow(470,4));
    optix::float3 integrated = integrate_scatter(light_source, light_intensity, origin, pa, pb, h0, rayleigh, mia,
                                                 inv_wave, ground_radius, atmosphere_radius, 5);
    return light_intensity * inv_wave * integrated;
}

#endif //MILKYWAYVR_UTIL_H


