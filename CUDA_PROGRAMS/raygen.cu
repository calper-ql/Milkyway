#include <optix.h>
#include <optixu/optixu_math_namespace.h>
#include <optixu/optixu_matrix_namespace.h>

#include "app_config.h"
#include "payloads.h"
#include "util.h"

using namespace optix;

rtBuffer<float4, 2>   result_buffer;

rtDeclareVariable(uint2, launch_index, rtLaunchIndex, );
rtDeclareVariable(uint2, theLaunchDim, rtLaunchDim, );
rtDeclareVariable(uint2, theLaunchIndex, rtLaunchIndex, );

rtDeclareVariable(Matrix3x3, view, , );
rtDeclareVariable(float3, origin, , );
rtDeclareVariable(float, focus, , );

rtDeclareVariable(rtObject, sysTopObject, , );

rtDeclareVariable(int, in_medium, , );
rtDeclareVariable(float3, medium_color, , );
rtDeclareVariable(float, medium_density, , );
rtDeclareVariable(float3, light_position, , );
rtDeclareVariable(float, light_intensity, , );
rtDeclareVariable(float, ground_radius, , );
rtDeclareVariable(float, atmosphere_radius, , );
rtDeclareVariable(float, rayleigh, , );
rtDeclareVariable(float, mia, , );

RT_PROGRAM void raygen()
{
	PerRayData pyld;
	pyld.radiance = make_float3(0.0);
	pyld.main_light_position = light_position;
	pyld.main_light_intensity = light_intensity;
	
	const float2 pixel = make_float2(theLaunchIndex);
	const float2 fragment = pixel + make_float2(0.5f);
	const float2 screen = make_float2(theLaunchDim);
	const float2 ndc = (fragment / screen) * 2.0f - 1.0f;

	float aspectRatio = screen.x / screen.y;

	float3 direction = view * normalize(make_float3(-ndc.x * aspectRatio, -ndc.y, focus));

	pyld.origin = origin;
	optix::Ray ray = optix::make_Ray(origin, direction, 0, 0.0f, RT_DEFAULT_MAX);
	rtTrace(sysTopObject, ray, pyld);

	if(in_medium == 1 && (pyld.last_state == OTHER || pyld.last_state == ATMOSPHERE)){
        float3 as = atmospheric_scatter(light_position, light_intensity, medium_density, rayleigh, mia, make_float3(0, 0, 0),
            origin, pyld.medium_last_pos, ground_radius, atmosphere_radius);
        if(as.x!=as.x) as.x = 0.0;
        if(as.y!=as.y) as.y = 0.0;
        if(as.z!=as.z) as.z = 0.0;
        pyld.radiance = pyld.radiance + as;
	}

	result_buffer[launch_index] = make_float4(pyld.radiance , 1.0f);
}