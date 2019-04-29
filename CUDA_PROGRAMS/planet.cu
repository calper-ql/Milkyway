#include <optix.h>
#include <optixu/optixu_math_namespace.h>
#include <optixu/optixu_matrix_namespace.h>
#include "app_config.h"
#include "payloads.h"
#include "util.h"

rtDeclareVariable(rtObject, sysTopObject, , );

rtDeclareVariable(optix::Ray, theRay, rtCurrentRay, );
rtDeclareVariable(float, theIntersectionDistance, rtIntersectionDistance, );
rtDeclareVariable(PerRayData, thePrd, rtPayload, );

rtDeclareVariable(optix::float3, varGeoNormal, attribute GEO_NORMAL, );
rtDeclareVariable(optix::float2, varTexCoord, attribute TEXCOORD, );
rtDeclareVariable(optix::float3, varShadingNormal, attribute SHADING_NORMAL, );

rtDeclareVariable(int, texture_id, , );

rtBuffer<char> color_graph;

using namespace optix;

RT_PROGRAM void planet()
{
	PerRayData pyld;
	pyld.visible = true;
	pyld.main_light_position = thePrd.main_light_position;
    pyld.main_light_intensity = thePrd.main_light_intensity;

	float3 origin = theRay.origin + theRay.direction * theIntersectionDistance;
	float3 direction = normalize(origin-thePrd.main_light_position);

	//float3 world_geo_normal = normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, varGeoNormal));
	//float3 world_shade_normal = normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, varShadingNormal));
	//float3 ffnormal = faceforward(world_shade_normal, -theRay.direction, world_shade_normal);

    float shadow = calculate_sphere_shadow(make_float3(0.0), origin, thePrd.main_light_position, 0.04, 0.00);

	float Ldist = length(thePrd.main_light_position - origin);
	optix::Ray shadow_ray(origin, direction, 1, 1.00, Ldist);
	rtTrace(sysTopObject, shadow_ray, pyld);


	if (pyld.visible) {
		thePrd.radiance = make_float3(optix::rtTex2D<float4>(texture_id, varTexCoord.x, varTexCoord.y)) * shadow;
	} else {
		thePrd.radiance = make_float3(optix::rtTex2D<float4>(texture_id, varTexCoord.x, varTexCoord.y)) * shadow * 0.8;
	}

    thePrd.distance = theIntersectionDistance;
    thePrd.medium_last_pos = origin;
    thePrd.last_state = OTHER;
}


