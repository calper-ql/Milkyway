//
// Created by calper on 4/20/19.
//

#ifndef MILKYWAYVR_ATMOSPHERE_H
#define MILKYWAYVR_ATMOSPHERE_H

#include <HiveEngine/RadialGenerator.h>
#include <HiveEngine/DynamicSphere.h>
#include <HiveEngine/Entity.h>

class AtmoshpereRadialGenerator : public HiveEngine::RadialGenerator {
public:
    glm::vec3 gas_color;
    glm::vec3 v_;

    AtmoshpereRadialGenerator(double radius, glm::vec3 gas_color);
    virtual ~AtmoshpereRadialGenerator();
    virtual glm::dvec3 generate_vertex(glm::dvec3 v);
    virtual glm::dvec3 generate_normal(glm::dvec3 v);
    virtual glm::dvec4 generate_color(glm::dvec3 v);
};

class Atmosphere : public HiveEngine::Entity {
public:
    glm::vec3 gas_color;
    float gas_density;
    HiveEngine::DynamicSphere* dslod;
    glm::vec3 position;
    float rayleigh;
    float mia;

    Atmosphere(glm::vec3 position, float radius, glm::vec3 gas_color, float gas_density, float rayleigh, float mia);
};


#endif //MILKYWAYVR_ATMOSPHERE_H
