//
// Created by calpe on 4/2/2019.
//

#include "DSLODOptixRenderer.h"

int __dslod_optix_dummy_func() {
    return 10;
}

DSLODOptixRenderer::DSLODOptixRenderer(HiveEngine::DynamicSphere* dslod, HiveEngine::Entity* e, OptixApp* render_app, std::string closest_hit_program_name, std::string anyhit_program_name){
    this->dslod = dslod;
	dslod->secondary = this;
    this->e = e;
    this->render_app = render_app;
    this->program_name = closest_hit_program_name;

    dyn_val = std::async(__dslod_optix_dummy_func);

    context = render_app->context;

    geometry_group = context->createGeometryGroup();
    geometry_group->setAcceleration(context->createAcceleration("Trbvh"));

    geometry_group->getAcceleration()->setProperty("vertex_buffer_name", "vertexBuffer");
    geometry_group->getAcceleration()->setProperty("vertex_buffer_stride", "12");

    geometry_group->getAcceleration()->setProperty("index_buffer_name", "indicesBuffer");
    geometry_group->getAcceleration()->setProperty("index_buffer_stride", "12");

    material = context->createMaterial();
    material->setClosestHitProgram(0, render_app->program_space[closest_hit_program_name]);
    material->setAnyHitProgram(1, render_app->program_space[anyhit_program_name]);

    transform = context->createTransform();
    auto pos = glm::vec3(e->calculate_position());
    auto rot = glm::mat3(e->calculate_rotation_matrix());
    glm::mat4 mat = glm::mat4(1.0);
    mat[3] = glm::vec4(pos, 1.0);
    optix::Matrix4x4 matrixPlane(glm::value_ptr(mat));
    transform->setMatrix(true, matrixPlane.getData(), matrixPlane.inverse().getData());

    render_app->root_group->addChild(transform);
    transform->setChild(geometry_group);

    dslod->max_depth = 12;
    dslod->min_depth = 0;
    dslod->limiter = 9;
	target_depth = 2;

}

void DSLODOptixRenderer::dynamic_check(glm::vec3 pos) {
    auto position = glm::dvec3(pos) - this->e->get_position();


	if (target_depth > dslod->min_depth) {
		if (dyn_val.wait_for(std::chrono::nanoseconds(0)) == std::future_status::ready) {
			dslod->min_depth++;
			auto rval = dyn_val.get();
			push_changes();
			dyn_val = std::async(std::launch::async, &HiveEngine::DynamicSphere::absolute_check, dslod, dslod->min_depth);
		}
	} else {
		if (dyn_val.wait_for(std::chrono::nanoseconds(0)) == std::future_status::ready) {
			auto rval = dyn_val.get();
			push_changes();
			dyn_val = std::async(std::launch::async, &HiveEngine::DynamicSphere::dynamic_check, dslod, position, fork_max, fork_min);
		}
	}

}

void DSLODOptixRenderer::push_changes() {

	for (auto sd : sync_data) {
		__add_LODSquare(sd);
	}

	sync_data.clear();
}

void DSLODOptixRenderer::created(HiveEngine::LODSquare *n) {
	LODSquareSyncData sd;
	sd.ptr = n;
	sd.operation = CREATED;
	sync_data.push_back(sd);
	if (secondary) secondary->created(n);
}

void DSLODOptixRenderer::deleted(HiveEngine::LODSquare *n) {
	LODSquareSyncData sd;
	sd.ptr = n;
	sd.operation = DELETED;
	sync_data.push_back(sd);
	if(secondary) secondary->deleted(n);
}

void DSLODOptixRenderer::changed(HiveEngine::LODSquare *n) {
	LODSquareSyncData sd;
	sd.ptr = n;
	sd.operation = CHANGED;
	sync_data.push_back(sd);
	if (secondary) secondary->changed(n);
}

void DSLODOptixRenderer::__add_LODSquare(LODSquareSyncData sd) {
	HiveEngine::LODSquare* ls = sd.ptr;
	void* dst;
	if (sd.operation == CREATED) {
		LODSquareRenderData* lsrd = new LODSquareRenderData();
		lsrd->geometry_triangle = context->createGeometryTriangles();
		lsrd->texture_sampler = context->createTextureSampler();
		lsrd->instance = context->createGeometryInstance();

		HiveEngine::Texture* texture = dslod->textures[sd.ptr->payload_idx];

		optix::Buffer vertexBuffer = context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, ls->vertices.size());
		dst = vertexBuffer->map(0, RT_BUFFER_MAP_WRITE);
		memcpy(dst, ls->vertices.data(), sizeof(glm::vec3) * ls->vertices.size());
		vertexBuffer->unmap();

		optix::Buffer uvBuffer = context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT2, ls->uvs.size());
		dst = uvBuffer->map(0, RT_BUFFER_MAP_WRITE);
		memcpy(dst, ls->uvs.data(), sizeof(glm::vec2) * ls->uvs.size());
		uvBuffer->unmap();

		optix::Buffer indicesBuffer = context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_UNSIGNED_INT3, ls->indices.size());
		dst = indicesBuffer->map(0, RT_BUFFER_MAP_WRITE);
		memcpy(dst, ls->indices.data(), sizeof(glm::uvec3) * ls->indices.size());
		indicesBuffer->unmap();

		optix::Buffer normalBuffer = context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, ls->normals.size());
		dst = normalBuffer->map(0, RT_BUFFER_MAP_WRITE);
		memcpy(dst, ls->normals.data(), sizeof(glm::vec3) * ls->normals.size());
		normalBuffer->unmap();

		lsrd->vertex_buffer = vertexBuffer;
		lsrd->uv_buffer = uvBuffer;
		lsrd->indices_buffer = indicesBuffer;
		lsrd->normal_buffer = normalBuffer;

		lsrd->geometry_triangle["vertexBuffer"]->setBuffer(vertexBuffer);
		lsrd->geometry_triangle["indicesBuffer"]->setBuffer(indicesBuffer);
		lsrd->geometry_triangle["uvBuffer"]->setBuffer(uvBuffer);
		lsrd->geometry_triangle["normalBuffer"]->setBuffer(normalBuffer);

		lsrd->geometry_triangle->setTriangleIndices(indicesBuffer, RT_FORMAT_UNSIGNED_INT3);
		lsrd->geometry_triangle->setVertices(ls->vertices.size(), vertexBuffer, RT_FORMAT_FLOAT3);
		lsrd->geometry_triangle->setBuildFlags(RTgeometrybuildflags(0));
		lsrd->geometry_triangle->setPrimitiveCount((unsigned)ls->indices.size());

		lsrd->geometry_triangle->setAttributeProgram(render_app->program_space["triangle_attributes"]);

		RTformat format = RT_FORMAT_BYTE;
		if (texture->channel == 3) format = RT_FORMAT_BYTE3;
		if (texture->channel == 4) format = RT_FORMAT_BYTE4;

		optix::Buffer texture_buffer = context->createBuffer(RT_BUFFER_INPUT, format, texture->width, texture->height);
		dst = texture_buffer->map(0, RT_BUFFER_MAP_WRITE);
		memcpy(dst, texture->data.data(), texture->width * texture->height * texture->channel);
		texture_buffer->unmap();

		lsrd->texture_sampler->setWrapMode(0, RT_WRAP_CLAMP_TO_EDGE);
		lsrd->texture_sampler->setWrapMode(1, RT_WRAP_CLAMP_TO_EDGE);
		lsrd->texture_sampler->setWrapMode(2, RT_WRAP_CLAMP_TO_EDGE);
		lsrd->texture_sampler->setBuffer(texture_buffer);
		lsrd->texture_sampler->setFilteringModes(RT_FILTER_LINEAR, RT_FILTER_LINEAR, RT_FILTER_NONE);
		lsrd->texture_sampler->setIndexingMode(RT_TEXTURE_INDEX_NORMALIZED_COORDINATES);
		lsrd->texture_sampler->setReadMode(RT_TEXTURE_READ_NORMALIZED_FLOAT);

		lsrd->instance->setGeometryTriangles(lsrd->geometry_triangle);
		lsrd->instance->addMaterial(material);
		lsrd->instance["texture_id"]->setInt(lsrd->texture_sampler->getId());

		geometry_group->addChild(lsrd->instance);
		geometry_group->getAcceleration()->markDirty();
		render_app->root_group->getAcceleration()->markDirty();

		render_data[ls] = lsrd;
	}
	else if (sd.operation == CHANGED) {
		auto iter = render_data.find(ls);
		if (iter != render_data.end()) {
			ls->is_changed();
		
			auto lsrd = render_data[ls];

			auto geo = lsrd->geometry_triangle;
			auto texture_sampler = lsrd->texture_sampler;

			geo["indicesBuffer"]->getBuffer()->setSize(ls->indices.size());
			dst = geo["indicesBuffer"]->getBuffer()->map(0, RT_BUFFER_MAP_WRITE);
			memcpy(dst, ls->indices.data(), sizeof(glm::uvec3) * ls->indices.size());
			geo["indicesBuffer"]->getBuffer()->unmap();

			geo->setPrimitiveCount((unsigned)ls->indices.size());

			geometry_group->getAcceleration()->markDirty();
			render_app->root_group->getAcceleration()->markDirty();
		}
		
	}
	else if (sd.operation == DELETED) {
		auto iter = render_data.find(ls);
		if (iter != render_data.end()) {
			auto lsrd = render_data[ls];
			render_data.erase(iter);

			geometry_group->removeChild(lsrd->instance);
			geometry_group->getAcceleration()->markDirty();
			render_app->root_group->getAcceleration()->markDirty();

			lsrd->geometry_triangle->destroy();
			lsrd->texture_sampler->destroy();
			lsrd->instance->destroy();
			lsrd->indices_buffer->destroy();
			lsrd->vertex_buffer->destroy();
			lsrd->uv_buffer->destroy();
			lsrd->normal_buffer->destroy();

			delete lsrd;
		}
	}

}
