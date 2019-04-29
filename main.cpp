#include <iostream>

#include "Utilities.h"
#include "TextureRenderer.h"
#include "RTCamera.h"
#include "OptixApp.h"
#include "Planet.h"
#include "Atmosphere.h"
#include "DSLODOptixRenderer.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

int width = 1920;
int height = 1080;

void window_size_callback(GLFWwindow* window, int w, int h){
    width = w;
    height = h;
    glViewport(0, 0, w, h);
}

int main() {


    initGL();
    GLFWwindow* window = glfwCreateWindow(width, height, "MilkywayVR", nullptr, nullptr);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwMakeContextCurrent(window);
    initGL3W();

    TextureRenderer texture_renderer;
    if(!texture_renderer.init()){
        std::cerr << "texture renderer failure." << std::endl;
        return 1;
    }

    GLuint gl_tex_id;

    RTCamera camera;
    camera.set_position({ 0.3, 0.0, -10*1000});

    OptixApp app;
    app.create_perspective(width, height, &camera);

    Planet planet({ 0, 0, 0 }, 6356, 20000, 10);
    DSLODOptixRenderer planet_renderer(planet.dslod, &planet, &app, "planet", "anyhit");

    Atmosphere atmosphere({ 0, 0, 0 }, 6356+100, {1.0, 1.0, 1.0}, 7.994, 0.5, 0.0);
    DSLODOptixRenderer atmosphere_renderer(atmosphere.dslod, &planet, &app, "atmosphere", "atmosphere_anyhit");
    atmosphere_renderer.dslod->max_depth = 7;

    auto pos = camera.get_position();

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char* glsl_version = "#version 150";
    ImGui_ImplOpenGL3_Init(glsl_version);

    bool show_another_window = true;
    float focus = camera.get_focus();

    __PlanetTimer timer;

    double fps = 0;
    auto str = "fps: " + std::to_string(fps);

    bool refresh_state = true;
    app.perspectives[0].light_position.x = -1000000.0f;
    app.perspectives[0].light_intensity = 1e8;

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        auto pos = camera.get_position();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if(show_another_window) {
            ImGui::Begin("RTCamera");
            ImGui::Text(str.c_str());
            ImGui::SliderFloat("focus", &focus, 0.00001f, 6.0f);
            ImGui::SliderFloat("rot_mod", &camera.rotate_modifier, 500.0f, 2000.0f);
            ImGui::SliderFloat("tra_mod", &camera.traverse_modifier, 0.001f, 20.0f);
            ImGui::Text("light");
            ImGui::SliderFloat("x", &app.perspectives[0].light_position.x, -1000000.0f, 1000000.0f);
            ImGui::SliderFloat("y", &app.perspectives[0].light_position.y, -1000000.0f, 1000000.0f);
            ImGui::SliderFloat("z", &app.perspectives[0].light_position.z, -1000000.0f, 1000000.0f);
            ImGui::SliderFloat("intensity", &app.perspectives[0].light_intensity, 0.0f, 1e8);
            camera.set_focus(focus);
            ImGui::Text("atm-ground");
            ImGui::RadioButton("refresh", &refresh_state);
            ImGui::SliderFloat("h0", &atmosphere.gas_density, 0.9, 10.2);
            ImGui::SliderFloat("rayleigh", &atmosphere.rayleigh, 0.0, 1.0);
            ImGui::SliderFloat("mia", &atmosphere.mia, 0.0, 1.0);
            ImGui::End();
        }

        if(refresh_state){
            planet_renderer.dynamic_check(pos);
            atmosphere_renderer.dynamic_check(pos);
            float inv_alt = atmosphere.get_radius() - glm::distance(camera.get_position(), planet.calculate_position());
            app.perspectives[0].medium_color = atmosphere.gas_color;
            app.perspectives[0].medium_density = atmosphere.gas_density;
            app.perspectives[0].ground_radius = planet.get_radius();
            app.perspectives[0].rayleigh = atmosphere.rayleigh;
            app.perspectives[0].mia = atmosphere.mia;
            if(inv_alt >= 0.0){
                app.perspectives[0].in_medium = 1;
            } else {
                app.perspectives[0].in_medium = 0;
            }

            for (auto pair : atmosphere_renderer.render_data) {
                auto lsrd = pair.second;
                lsrd->instance["density"]->set1fv(&atmosphere.gas_density);
                lsrd->instance["ground_radius"]->setFloat(planet.get_radius());
                lsrd->instance["atmosphere_radius"]->setFloat(atmosphere.get_radius());
                lsrd->instance["inv_altitude"]->set1fv(&inv_alt);
                lsrd->instance["rayleigh"]->set1fv(&atmosphere.rayleigh);
                lsrd->instance["scatter"]->set1fv(&atmosphere.mia);
            }
        }

        app.edit_perspective_size(0, width, height);
        app.render_perspective(0);

        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
            //planet.set_position(planet.calculate_position() + glm::dvec3({ 10.0, 0.0, 0.0 }));
            auto diff = glm::distance(camera.get_focus_center(), camera.get_position());
            std::cout << "Cam diff " << diff << std::endl;
        }

        try {
            rt_buffer_to_gl(app.perspectives[0].image, gl_tex_id);
            texture_renderer.render(gl_tex_id);
        } catch (optix::Exception& e) {
            std::cerr << e.getErrorString() << std::endl;

        } catch (std::exception& e){
            std::cerr << e.what() << std::endl;
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if(!ImGui::IsAnyWindowFocused()){
            camera.get_user_input(window, false);
        }

        camera.get_user_movement(window, false);

        glfwSwapInterval(1);
        glfwSwapBuffers(window);
        glfwPollEvents();

        fps++;
        auto elapsed = timer.elapsed();
        if (elapsed > 1) {
            str = "fps: " + std::to_string(fps) + " - tex_id: " + std::to_string(gl_tex_id);
            fps = 0;
            timer.reset();
        }

    }

    return 0;
}