#include <optix.h>
#include <optixu/optixu_math_namespace.h>
#include <optixu/optixu_matrix_namespace.h>

#include "payloads.h"

// Context global variables provided by the renderer system.
rtDeclareVariable(rtObject, sysTopObject, , );

// Semantic variables.
rtDeclareVariable(optix::Ray, theRay, rtCurrentRay, );
rtDeclareVariable(PerRayData, thePrd, rtPayload, );

// The shadow ray program for all materials with no cutout opacity.
RT_PROGRAM void anyhit()
{
  thePrd.radiance = make_float3(0.0, 0.0, 0.0);
  thePrd.visible = false;
  thePrd.distance = 0;
  thePrd.last_state = OTHER;
  rtTerminateRay();
}
