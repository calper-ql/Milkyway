#include "Planet.h"

PlanetProceduralGenerator::PlanetProceduralGenerator(double radius) : RadialGenerator(radius) {
	kl = 0.0;
	pi = 3.14159265359;
	std::ifstream i("test_graph.json");
	json j;
	i >> j;
	mp.from_json(j);
	om = nullptr;
	for (auto pair : mp.modules) {
		if (pair.second->type == "OutputModule") {
			auto temp = (NGEN::OutputModule*) pair.second;
			if (temp->name.value == "vertex") {
				om = temp;
			}
		}
		if (pair.second->type == "RGBOutputModule") {
			auto temp = (NGEN::RGBOutputModule*) pair.second;
			if (temp->name.value == "rgb") {
				rgbm = temp;
			}
		}
	}
	if (om == nullptr) exit(11);
	if (rgbm == nullptr) exit(12);

}

PlanetProceduralGenerator::~PlanetProceduralGenerator() {

}

glm::dvec3 PlanetProceduralGenerator::generate_vertex(glm::dvec3 v) {
	v_ = glm::normalize(v) / 2.0 + 1.0;

	mp.reset();
	kl = om->input.calculate(v_.x, v_.y, v_.z);
	height = radius + kl * (radius / 500.0);
	auto r = v * (height / glm::length(v));
	return r;
}

glm::vec3 __ppg_latlon_vec3(float lat, float lon, float pi) {
	glm::vec3 vec;
	auto phi = (lon + 90) / 180 * pi;
	auto theta = (90 - lat) / 180 * pi;

	vec.x = sin(theta) * sin(phi);
	vec.y = cos(theta);
	vec.z = sin(theta) * cos(phi);

	return vec;
}

glm::dvec3 PlanetProceduralGenerator::generate_normal(glm::dvec3 v) {
	v_ = glm::normalize(v) / 2.0 + 1.0;
	float lat = float(v_.y / 1.0);
	float lon = float(v_.x / v_.z);

	//glm::vec3 up = __ppg_latlon_vec3(lat, lon + 0.01, pi);
	//glm::vec3 left = __ppg_latlon_vec3(lat - 0.01, lon, pi);
	//glm::vec3 right = __ppg_latlon_vec3(lat + 0.01, lon, pi);
	//glm::vec3 down = __ppg_latlon_vec3(lat, lon - 0.01, pi);

	//auto up_g = om->input.calculate(up.x, up.y, up.z);
	//auto left_g = om->input.calculate(left.x, left.y, left.z);
	//auto right_g = om->input.calculate(right.x, right.y, right.z);
	//auto down_g = om->input.calculate(down.x, down.y, down.z);

	auto normal = v_;

	return normal;
}

glm::dvec4 PlanetProceduralGenerator::generate_color(glm::dvec3 v) {
	v_ = glm::normalize(v) / 2.0 + 1.0;
	rgbm->calculate(v_.x, v_.y, v_.z);
	return glm::dvec4(rgbm->stored[0] / 2.0, rgbm->stored[1] / 2.0, rgbm->stored[2] / 2.0, 1.0);
}



Planet::Planet(glm::dvec3 position, double radius, double mass, size_t seed)
	: HiveEngine::Entity(position, radius, mass)
{

	this->seed = seed;
    HiveEngine::RadialGenerator* pgn = new PlanetProceduralGenerator(radius);
	this->dslod = new HiveEngine::DynamicSphere(pgn, 15);
}

__PlanetTimer::__PlanetTimer() : beg_(clock_::now()) {}
void __PlanetTimer::reset() { beg_ = clock_::now(); }
double __PlanetTimer::elapsed() const {
	return std::chrono::duration_cast<second_>
		(clock_::now() - beg_).count();
}

