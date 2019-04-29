//
// Created by calpe on 4/2/2019.
//

#ifndef MILKYWAYVR_DSLODOPTIXRENDERER_H
#define MILKYWAYVR_DSLODOPTIXRENDERER_H

#include "OptixApp.h"
#include "HiveEngine/DynamicSphere.h"
#include <future>
#include <map>
#include <chrono>

struct LODSquareRenderData {
	optix::GeometryTriangles geometry_triangle;
	optix::TextureSampler texture_sampler;
	optix::GeometryInstance instance;
	optix::Buffer vertex_buffer;
	optix::Buffer uv_buffer;
	optix::Buffer indices_buffer;
	optix::Buffer normal_buffer;
};

enum LODSquareOperation {
	CREATED = 0, CHANGED = 1, DELETED = 2
};

struct LODSquareSyncData {
	HiveEngine::LODSquare* ptr;
	LODSquareOperation operation;
};

class DSLODOptixRenderer : public HiveEngine::LODSquareCreateDelegate {
public:
    HiveEngine::Entity* e;
    HiveEngine::DynamicSphere* dslod;

    OptixApp* render_app;
    std::string program_name;

    optix::Material material;
    optix::Context context;
    optix::Transform transform;

    float fork_min = 0.15;
    float fork_max = 0.2;
	unsigned target_depth;

    std::future<int> dyn_val;
    optix::GeometryGroup geometry_group;

    DSLODOptixRenderer(HiveEngine::DynamicSphere* dslod, HiveEngine::Entity* e, OptixApp* render_app, std::string closest_hit_program_name, std::string anyhit_program_name);

    void dynamic_check(glm::vec3 pos);
    void push_changes();
    void __add_LODSquare(LODSquareSyncData sd);

	std::vector<LODSquareSyncData> sync_data;
	std::map<HiveEngine::LODSquare*, LODSquareRenderData*> render_data;

	virtual void created(HiveEngine::LODSquare* n);
	virtual void deleted(HiveEngine::LODSquare* n);
	virtual void changed(HiveEngine::LODSquare* n);
};


#endif //MILKYWAYVR_DSLODOPTIXRENDERER_H
