#include <optix.h>
#include <optixu/optixu_math_namespace.h>

rtBuffer<float3> vertexBuffer;
rtBuffer<float2> uvBuffer;
rtBuffer<uint3> indicesBuffer;

rtDeclareVariable(optix::Ray, theRay, rtCurrentRay, );

rtDeclareVariable(optix::float3, varGeoNormal, attribute GEO_NORMAL, );
rtDeclareVariable(optix::float2, varTexCoord, attribute TEXCOORD, );

RT_PROGRAM void intersection_triangle_indexed(int primitiveIndex)
{
  const uint3 indices = indicesBuffer[primitiveIndex];

  const float3 v0 = vertexBuffer[indices.x];
  const float3 v1 = vertexBuffer[indices.y];
  const float3 v2 = vertexBuffer[indices.z];

  const float2 uv0 = uvBuffer[indices.x];
  const float2 uv1 = uvBuffer[indices.y];
  const float2 uv2 = uvBuffer[indices.z];

  float3 n;
  float  t;
  float  beta;
  float  gamma;

  if (intersect_triangle(theRay, v0, v1, v2, n, t, beta, gamma))
  {
    if (rtPotentialIntersection(t))
    {
		const float alpha = 1.0f - beta - gamma;
		varGeoNormal = n;
		varTexCoord = uv0 * alpha + uv1 * beta + uv2 * gamma;
		rtReportIntersection(0);
    }
  }
}
