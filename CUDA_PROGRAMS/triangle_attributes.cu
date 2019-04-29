#include <optix.h>
#include <optixu/optixu_math_namespace.h>

rtDeclareVariable(float2, barycentrics, attribute barycentrics, );

rtDeclareVariable(optix::float3, varGeoNormal, attribute GEO_NORMAL, );
rtDeclareVariable(optix::float2, varTexCoord, attribute TEXCOORD, );
rtDeclareVariable(float3, varShadingNormal, attribute SHADING_NORMAL, );

rtBuffer<float3> vertexBuffer;
rtBuffer<float2> uvBuffer;
rtBuffer<uint3> indicesBuffer;
rtBuffer<float3> normalBuffer;

//rtBuffer<char> procedure;

__device__ void test(char* bas) {

}

RT_PROGRAM void triangle_attributes() {
	const uint3 v_idx = indicesBuffer[rtGetPrimitiveIndex()];
	const float3 v0 = vertexBuffer[v_idx.x];
	const float3 v1 = vertexBuffer[v_idx.y];
	const float3 v2 = vertexBuffer[v_idx.z];
	const float3 n0 = normalBuffer[v_idx.x];
	const float3 n1 = normalBuffer[v_idx.y];
	const float3 n2 = normalBuffer[v_idx.z];
	varShadingNormal = optix::cross(v1 - v0, v2 - v0);

	const float2 t0 = uvBuffer[v_idx.x];
	const float2 t1 = uvBuffer[v_idx.y];
	const float2 t2 = uvBuffer[v_idx.z];

	barycentrics = rtGetTriangleBarycentrics();

	const float3 Ng = n1 * barycentrics.x + n2 * barycentrics.y + n0 * (1.0f - barycentrics.x - barycentrics.y);
	varGeoNormal = optix::normalize(Ng);

	const float3 temp = make_float3(t1*barycentrics.x + t2 * barycentrics.y + t0 * (1.0f - barycentrics.x - barycentrics.y));
	varTexCoord = make_float2(temp.x, temp.y);
}