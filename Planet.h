#ifndef PLANET_H
#define PLANET_H

#include <HiveEngine/DynamicSphere.h>
#include <NGEN/SimpleModules.h>
#include <HiveEngine/Entity.h>
#include <chrono>

class __PlanetTimer
{
public:
	__PlanetTimer();
	void reset();
	double elapsed() const;

private:
	typedef std::chrono::high_resolution_clock clock_;
	typedef std::chrono::duration<double, std::ratio<1> > second_;
	std::chrono::time_point<clock_> beg_;
};

class PlanetProceduralGenerator : public HiveEngine::RadialGenerator {
public:
	double kl;
	double pi;
	double height;
	glm::vec3 v_;

    NGEN::ModulePool mp;
    NGEN::OutputModule* om;
	NGEN::RGBOutputModule* rgbm;

	PlanetProceduralGenerator(double radius);
	virtual ~PlanetProceduralGenerator();

	virtual glm::dvec3 generate_vertex(glm::dvec3 v);
	virtual glm::dvec3 generate_normal(glm::dvec3 v);
	virtual glm::dvec4 generate_color(glm::dvec3 v);
};

class Planet : public HiveEngine::Entity {
public:
	size_t seed;
    HiveEngine::DynamicSphere* dslod;

	Planet(glm::dvec3 position, double radius, double mass, size_t seed);

};

#endif