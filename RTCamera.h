//
// Created by calper on 4/19/19.
//

#ifndef MILKYWAYVR_RTCAMERA_H
#define MILKYWAYVR_RTCAMERA_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Utilities.h"

class RTCamera {
private:
    glm::dvec3 position;
    glm::dquat orientation;
    glm::dmat4 perspective;
    double focus;
    int m_press = 0;

    glm::ivec2 last_m_pos;

public:
    float traverse_modifier;
    float rotate_modifier;

    RTCamera();

    glm::dvec3 get_position();

    glm::dquat get_orientation();

    glm::dmat4 get_perspective();

    void set_perspective(float fov, float ratio, float near, float far);

    void set_position(glm::dvec3 position);

    void rotate(double angle, double x, double y, double z);

    void yaw(float angle);

    void pitch(float angle);

    void roll(float angle);

    glm::mat4 get_view(bool apply_perspective);

    double get_focus();

    void set_focus(double new_focus);

    void get_user_input(GLFWwindow *window, bool print);

    void get_user_movement(GLFWwindow *window, bool print);

    void look_at(glm::vec3 point);

    glm::dvec3 get_focus_center();
};


#endif //MILKYWAYVR_RTCAMERA_H
