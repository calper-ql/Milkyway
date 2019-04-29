#ifndef OPTIXAPP_H
#define OPTIXAPP_H

#include "HiveEngine/DynamicSphere.h"
#include "Utilities.h"
#include "TextureRenderer.h"
#include "RTCamera.h"

#include "HiveEngine/Entity.h"

#include <vector>
#include <map>
#include <optixu/optixpp_namespace.h>
#include <optix.h>
#include <optixu/optixu_math_namespace.h>
#include <optixu/optixu_matrix_namespace.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <filesystem>

struct Perspective {
    // Perspective
	unsigned width;
	unsigned height;
	optix::Buffer image;
	RTCamera* camera;
	GLuint gl_buf;

	// Main light;
    float light_intensity = 1e10;
    glm::vec3 light_position = {1000000, 0.0, 0.0};

	// MEDIUM
	int in_medium;
	glm::vec3 medium_color;
	float medium_density;
	float ground_radius;
    float atmosphere_radius;
	float rayleigh;
	float mia;
};

class OptixApp {
public:

	std::map<std::string, optix::Program> program_space;

	optix::Context context;

	optix::Group root_group;
	
	std::vector<Perspective> perspectives;

	size_t texture_push_count;

	OptixApp();
	~OptixApp();

	size_t create_perspective(unsigned width, unsigned height, RTCamera* camera);
	void edit_perspective_size(unsigned idx, unsigned width, unsigned height);

	void load_programs();

	void render_perspective(size_t i);
};

#endif 