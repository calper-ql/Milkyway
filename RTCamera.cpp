//
// Created by calper on 4/19/19.
//

#include "RTCamera.h"

RTCamera::RTCamera() {
    orientation = glm::dquat(glm::dvec3(0, 0, 0));
    set_perspective(90, 1, 0.1, 1000.0);
    traverse_modifier = 0.01;
    rotate_modifier = 500.0;
    focus = 1.0;
}

glm::dvec3 RTCamera::get_position() {
    return this->position;
}

glm::dquat RTCamera::get_orientation() {
    return this->orientation;
}

glm::dmat4 RTCamera::get_perspective() {
    return this->perspective;
}

void RTCamera::set_perspective(float fov, float ratio, float near_, float far_) {
    this->perspective = glm::perspective(fov, ratio, near_, far_);
}

void RTCamera::set_position(glm::dvec3 position) {
    this->position = position;
}

void RTCamera::rotate(double angle, double x, double y, double z) {
    auto a = orientation;
    auto b = glm::angleAxis(angle*1.0/focus, glm::dvec3(x, y, z) * a);
    orientation = a * b;
}

void RTCamera::yaw(float angle) {
    this->rotate(angle, 0.0, 1.0, 0.0);
}

void RTCamera::pitch(float angle) {
    this->rotate(angle, 1.0, 0.0, 0.0);
}

void RTCamera::roll(float angle) {
    this->rotate(angle, 0.0, 0.0, 1.0);
}

glm::mat4 RTCamera::get_view(bool apply_perspective) {
    if(apply_perspective) return perspective * glm::translate(glm::mat4_cast(orientation), position);
    glm::mat4 model = glm::mat4(glm::mat3_cast(orientation));
    return model;
}

void RTCamera::get_user_movement(GLFWwindow *window, bool print) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        position += glm::dvec3(glm::row(glm::mat4_cast(get_orientation()), 2)) * (double)traverse_modifier;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position -= glm::dvec3(glm::row(glm::mat4_cast(get_orientation()), 2)) * (double)traverse_modifier;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        position += glm::dvec3(glm::row(glm::mat4_cast(get_orientation()), 0)) * (double)traverse_modifier;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position -= glm::dvec3(glm::row(glm::mat4_cast(get_orientation()), 0)) * (double)traverse_modifier;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        position -= glm::dvec3(glm::row(glm::mat4_cast(get_orientation()), 1)) * (double)traverse_modifier;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        position += glm::dvec3(glm::row(glm::mat4_cast(get_orientation()), 1)) * (double)traverse_modifier;
    }
}

void RTCamera::get_user_input(GLFWwindow *window, bool print) {

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        //pitch(rotate_modifier);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        //pitch(-rotate_modifier);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        //yaw(-rotate_modifier);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        //yaw(rotate_modifier);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        roll(0.015);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        roll(-0.015);
    }

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        if(m_press == 0){
            m_press++;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else {
            double dx = x - last_m_pos.x;
            double dy = y - last_m_pos.y;
            pitch(dy/rotate_modifier);
            yaw(dx/rotate_modifier);
        }
        last_m_pos.x = x;
        last_m_pos.y = y;
    }
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        m_press = 0;
    }

}

double RTCamera::get_focus() {
    return focus;
}

void  RTCamera::set_focus(double new_focus) {
    this->focus = new_focus;
}

void RTCamera::look_at(glm::vec3 point) {
    //orientation = glm::dquat(glm::dvec3(0, 0, 0));
    glm::dvec3 up = glm::dvec3(glm::row(glm::mat4_cast(get_orientation()), 1));
}

glm::dvec3 RTCamera::get_focus_center()
{
    glm::dvec3 forward = glm::dvec3(0.0, 0.0, 1.0);
    forward *= (focus);
    forward = glm::dmat3(get_view(false)) * forward;
    return forward + position;
}

