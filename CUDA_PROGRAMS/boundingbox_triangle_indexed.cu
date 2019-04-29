#include <optix.h>
#include <optixu/optixu_aabb_namespace.h>
#include <optixu/optixu_math_namespace.h>

rtBuffer<float3> vertexBuffer;
rtBuffer<float2> uvBuffer;
rtBuffer<uint3> indicesBuffer;

RT_PROGRAM void boundingbox_triangle_indexed(int primitiveIndex, float result[6])
{
  const uint3 indices = indicesBuffer[primitiveIndex];

  const float3 v0 = vertexBuffer[indices.x];
  const float3 v1 = vertexBuffer[indices.y];
  const float3 v2 = vertexBuffer[indices.z];

  const float area = optix::length(optix::cross(v1 - v0, v2 - v0));

  optix::Aabb *aabb = (optix::Aabb *) result;

  if (0.0f < area && !isinf(area))
  {
    aabb->m_min = fminf(fminf(v0, v1), v2);
    aabb->m_max = fmaxf(fmaxf(v0, v1), v2);
  }
  else
  {
    aabb->invalidate();
  }
}
