#pragma once

#ifndef PER_RAY_DATA_H
#define PER_RAY_DATA_H

enum MEDIUM_STATE {
    NONE = 0, ATMOSPHERE = 1, OTHER = 2
};

struct Light {
    int enabled;
    optix::float3 radiance;
    optix::float3 location;
};

struct PerRayData
{
	optix::float3 radiance; // Radiance along the current path segment.
	optix::float3 origin;
	optix::float3 main_light_position;
	float main_light_intensity;
	float distance;
	bool visible;
	optix::float3 medium_last_pos;
	MEDIUM_STATE last_state;
};

#endif // PER_RAY_DATA_H