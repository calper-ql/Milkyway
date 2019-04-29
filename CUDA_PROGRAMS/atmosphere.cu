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
rtDeclareVariable(float, density, , );
rtDeclareVariable(float, ground_radius, , );
rtDeclareVariable(float, atmosphere_radius, , );
rtDeclareVariable(float, inv_altitude, , );
rtDeclareVariable(float, rayleigh, , );
rtDeclareVariable(float, mia, , );

rtBuffer<char> color_graph;

using namespace optix;

RT_PROGRAM void atmosphere()
{
	PerRayData pyld;
	pyld.visible = true;
	pyld.main_light_position = thePrd.main_light_position;
    pyld.main_light_intensity = thePrd.main_light_intensity;

	float3 origin = theRay.origin + theRay.direction * theIntersectionDistance;
	float3 direction = normalize(pyld.main_light_position - origin);

	float3 world_geo_normal = normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, varGeoNormal));
	float3 world_shade_normal = normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, varShadingNormal));
	float3 ffnormal = faceforward(world_shade_normal, -theRay.direction, world_shade_normal);

	optix::Ray density_ray(origin, theRay.direction, 0, 0.1, RT_DEFAULT_MAX);

	rtTrace(sysTopObject, density_ray, pyld);

	float3 tex_color = make_float3(optix::rtTex2D<float4>(texture_id, varTexCoord.x, varTexCoord.y));

    if(pyld.last_state == NONE){
        thePrd.radiance = pyld.radiance;
    } else {
            float3 as = atmospheric_scatter(thePrd.main_light_position, thePrd.main_light_intensity, density, rayleigh, mia, make_float3(0, 0, 0), origin,
                pyld.medium_last_pos, ground_radius, atmosphere_radius);
            thePrd.radiance = pyld.radiance + as ;
    }

    thePrd.distance = theIntersectionDistance;
    thePrd.medium_last_pos = origin;
    thePrd.last_state = ATMOSPHERE;
}


