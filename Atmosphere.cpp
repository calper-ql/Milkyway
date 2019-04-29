//
// Created by calper on 4/20/19.
//

#include "Atmosphere.h"

AtmoshpereRadialGenerator::AtmoshpereRadialGenerator(double radius, glm::vec3 gas_color) : RadialGenerator(radius) {
    this->gas_color = gas_color;
}

AtmoshpereRadialGenerator::~AtmoshpereRadialGenerator() {

}

glm::dvec3 AtmoshpereRadialGenerator::generate_vertex(glm::dvec3 v) {
    v_ = glm::normalize(v) / 2.0 + 1.0;

    auto r = v * (radius / glm::length(v));
    return r;
}


glm::dvec3 AtmoshpereRadialGenerator::generate_normal(glm::dvec3 v) {
    v_ = glm::normalize(v) / 2.0 + 1.0;
    auto normal = v_;
    return normal;
}

glm::dvec4 AtmoshpereRadialGenerator::generate_color(glm::dvec3 v) {
    v_ = glm::normalize(v) / 2.0 + 1.0;
    return glm::dvec4(gas_color/2.0f, 1.0);
}

Atmosphere::Atmosphere(glm::vec3 position, float radius, glm::vec3 gas_color, float gas_density, float rayleigh, float mia)
        : HiveEngine::Entity(position, radius, 0.001)
{
    this->position = position;
    this->gas_density = gas_density;
    this->rayleigh = rayleigh;
    this->mia = mia;
    HiveEngine::RadialGenerator* pgn = new AtmoshpereRadialGenerator(radius, gas_color);
    this->dslod = new HiveEngine::DynamicSphere(pgn, 3);
}