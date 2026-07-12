#include "application/fractal_gui.h"
#include "core/printer.h"
#include "io/fractal_import.h"
#include "encoder/goo/goo_exporter.h"

#include "imgui/imgui.h"
#include "imguifiledialog/ImGuiFileDialog.h"
#include <exception>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <memory>

void FractalGui::render() {
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(350, camera.windowHeight), ImGuiCond_Always);
    
    ImGui::Begin("Control Panel", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    if (ImGui::Button("Import JSON", ImVec2(-1, 0))) {
        IGFD::FileDialogConfig config;
        config.path = "res/fractals_json/";
        config.flags |= ImGuiFileDialogFlags_Modal;
        ImGuiFileDialog::Instance()->OpenDialog("ChoseFileDlgKey", "Choose a JSON file", ".json", config);
    }
    if (ImGuiFileDialog::Instance()->Display("ChoseFileDlgKey", ImGuiWindowFlags_NoCollapse, ImVec2(700, 300))) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string file_path = ImGuiFileDialog::Instance()->GetFilePathName();
            FractalModel new_model = json_import::import_json(file_path);
            this->current_model = std::make_shared<FractalModel>(std::move(new_model));
        }
        ImGuiFileDialog::Instance()->Close();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::BeginTabBar("MainMainTabs")) {
        if (ImGui::BeginTabItem("3D Viewver")) {
            this->interactive_tab_open = true;
            draw_interactive_tab();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Slicer")) {
            this->interactive_tab_open = false;
            draw_slicer_tab();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
    draw_viewport();

    if (ImGuiFileDialog::Instance()->Display("ChooseConfigDlg", ImGuiWindowFlags_NoCollapse, ImVec2(700, 400))) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            snprintf(config_path_buf, sizeof(config_path_buf), "%s", filePathName.c_str());
        }
        ImGuiFileDialog::Instance()->Close();
    }

    if (ImGuiFileDialog::Instance()->Display("ChooseOutputDlg", ImGuiWindowFlags_NoCollapse, ImVec2(700, 400))) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            snprintf(output_path_buf, sizeof(output_path_buf), "%s", filePathName.c_str());
        }
        ImGuiFileDialog::Instance()->Close();
    }
}

void FractalGui::draw_interactive_tab() {
    ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "Interactive Fractal Explorer");
    ImGui::Spacing();

    if (viewer.is_working()) {
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Status: Generating...");
        ImGui::ProgressBar(viewer.get_progress());
    } else {
        ImGui::Text("Status: Idle");
    }
    ImGui::Separator();

    ImGui::SliderInt("Iterations", &this->iteration, 0, 10); // TODO Replace with iteration estimation

    if (ImGui::CollapsingHeader("Visibility and Cut Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Spacing();
        ImGui::Checkbox("Show Cut Plane", &this->show_plane);

        if (this->show_plane) {
            ImGui::Separator();
            ImGui::Text("Plane Normal (XYZ)");

            ImGui::SliderFloat("Normal X", &this->plane_settings.normal.x, -1.0f, 1.0f, "%.3f");
            ImGui::SliderFloat("Normal Y", &this->plane_settings.normal.y, -1.0f, 1.0f, "%.3f");
            ImGui::SliderFloat("Normal Z", &this->plane_settings.normal.z, -1.0f, 1.0f, "%.3f");
            
            ImGui::Spacing();
            ImGui::SliderFloat("Plane offset", &this->plane_settings.z_offset, -5.0f, 5.0f, "%.3f");
            ImGui::Spacing();
            if (ImGui::Button("Reset Plane Settings", ImVec2(-1, 0))) {
                this->plane_settings.normal = glm::vec3(0.0f, 0.0f, 1.0f); // По умолчанию логично резать по Z
                this->plane_settings.z_offset = 0.0f;
                this->plane_settings.cut_mode = 0;
            }
            ImGui::Separator();
        }

    }
    
    ImGui::Spacing();

    ImGui::BeginDisabled(viewer.is_working() || !current_model);
    if (ImGui::Button("Render 3D", ImVec2(-1, 40))) {
        viewer.start_generation(current_model, iteration, plane_settings);
    }
    ImGui::EndDisabled();

    if (viewer.is_working() && ImGui::Button("Cancel", ImVec2(-1 ,0))) {
        viewer.cancel();
    }
}

void FractalGui::draw_slicer_tab() {
    ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "Fractal Slicing Pipeline");
    ImGui::Spacing();

    if (slicer.is_working()) {
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Status: Slicing");
        ImGui::ProgressBar(slicer.get_progress());
        // ImGui::Text("Processed Layers: %d", slicer.get_current_layer());
    } else {
        ImGui::Text("Status: Ready");
    }
    ImGui::Separator();

    ImGui::SliderInt("Target Iteration", &iteration, 0, 10); // TODO Replace with iteration estimation
    ImGui::Spacing();

    ImGui::Text("Export Settings");

    if (ImGui::Button("Browse##Config")) {
        IGFD::FileDialogConfig config;
        config.path = "res/printers_json";
        config.flags |= ImGuiFileDialogFlags_Modal;
        
        ImGuiFileDialog::Instance()->OpenDialog("ChooseConfigDlg", "Choose Printer Config", ".json", config);
    }

    if (ImGui::Button("Save As##Output")) {
        IGFD::FileDialogConfig config;
        config.path = "res";
        config.filePathName = "model.goo";
        config.flags |= ImGuiFileDialogFlags_Modal;
        
        ImGuiFileDialog::Instance()->OpenDialog("ChooseOutputDlg", "Save Output File", ".goo", config);
    }
    
    ImGui::InputFloat("Scale (1 unit to mm)", &scale_1_to_mm, 0.1f, 10.0f, "%.3f");

    ImGui::Spacing();

    if (!slicer_error_msg.empty()) {
        ImGui::TextWrapped("Error:");
        ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "%s", slicer_error_msg.c_str());
        ImGui::Spacing();
    }

    ImGui::BeginDisabled(slicer.is_working() || !current_model);
    if (ImGui::Button("Start Slicing to .goo", ImVec2(-1, 40))) {
        slicer_error_msg = "";
        try {
            slice_renderer.clear_layers();
            
            std::string config_path = config_path_buf;
            std::string output_path = output_path_buf;

            auto exporter = std::make_shared<encoder::GooExporter>(config_path, output_path);
            Printer printer = exporter->get_printer(scale_1_to_mm);
            Printer::PrintableArea area = printer.get_printable_area();
            this->slice_renderer.update_physical_size(area.width_mm, area.height_mm);

            arma::rowvec arma_normal = {
                plane_settings.normal.x, 
                plane_settings.normal.y, 
                plane_settings.normal.z
            };

            slicer.start_slicing(current_model, iteration, arma_normal, plane_settings.z_offset, printer, exporter);
        } catch (const std::exception& e) {
            slicer_error_msg = e.what();
        }
    }
    ImGui::EndDisabled();

    if (slicer.is_working() && ImGui::Button("Cancel Slicing", ImVec2(-1, 0))) {
        slicer.cancel();
    }
}

void FractalGui::draw_viewport() {
    ImGui::SetNextWindowPos(ImVec2(350, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(camera.windowWidth - 350, camera.windowHeight), ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
    
    ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

    ImVec2 size = ImGui::GetContentRegionAvail();
    GLuint texture_id = 0;

    // Camera
    glm::mat4 projection = glm::perspective(glm::radians(camera.fov), size.x / size.y, 0.1f, 1000.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -camera.cameraDist));
    view = glm::rotate(view, glm::radians(camera.pitch), glm::vec3(-1.0f, 0.0f, 0.0f));
    view = glm::rotate(view, glm::radians(camera.yaw), glm::vec3(0.0f, 1.0f, 0.0f));

    // Plane
    glm::mat4 plane_model = glm::mat4(1.0f);
    if (this->show_plane) {
        glm::vec3 local_normal(0.0f, 0.0f, 1.0f);
        glm::vec3 target_normal = glm::normalize(this->plane_settings.normal);
        glm::mat4 rotation = glm::mat4(1.0f);

        float cos_theta = glm::dot(local_normal, target_normal);
        if (cos_theta < 0.999f) {
            if (cos_theta < -0.999f) rotation = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            else {
                glm::vec3 rotation_axis = glm::cross(local_normal, target_normal);
                rotation = glm::rotate(glm::mat4(1.0f), glm::acos(cos_theta), rotation_axis);
            }
        }

        glm::mat4 translation = glm::translate(glm::mat4(1.0f), target_normal * this->plane_settings.z_offset);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(this->plane_settings.size, this->plane_settings.size, 1.0f));

        plane_model = translation * rotation * scale;
    }
    
    if (slicer.is_working() || !this->interactive_tab_open) {
        texture_id = slice_renderer.render_to_texture(size.x, size.y, view, projection);
    } else {
        texture_id = frac_renderer.render_to_texture(size.x, size.y, view, projection, this->show_plane, plane_model);
    }

    if (texture_id != 0) {
        ImGui::Image((void*)(intptr_t)texture_id, size, ImVec2(0, 1), ImVec2(1, 0));
    }

    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) camera.isDragging = true;
    ImGui::End();
    
    if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) camera.isDragging = false;
    ImGui::PopStyleVar();
}
