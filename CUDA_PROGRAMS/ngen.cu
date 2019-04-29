
#include <optix.h>
#include <optixu/optixu_math_namespace.h>

#define RT_USE_TEMPLATED_RTCALLABLEPROGRAM 1

RT_CALLABLE_PROGRAM void calculate_NGEN(char* procedure, float x, float y, float &z) {

	//return 0.0;
	z = x + y;
}