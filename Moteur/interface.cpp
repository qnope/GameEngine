#include "Interface.h"

Interface::Interface(std::string name) : mName(name)
{
}

EngineParameter Interface::getParameters() const
{
    return mEngineParameter;
}

void Interface::execute() {
    createEngineWindow();
    createSettingsWindow();
    createVoxelSettingsWindow();
    createAOSettingsWindow();
    createProfilingWindow();
}

void Interface::setVoxelizationProfiling(const std::vector<Profiling>* ptr)
{
    mVoxelizationProfiling = ptr;
}

void Interface::checkError()
{
    if (!mEngineParameter.voxelizationPassParameter.enable) {
        if (mEngineParameter.presentationPassParameter.type == VOXEL_GRID)
            mEngineParameter.presentationPassParameter.type = FINAL_RENDERING;

        if (mEngineParameter.presentationPassParameter.type == AMBIENT_OCCLUSION && !mEngineParameter.ambientOcclusionParameter.ssao)
            mEngineParameter.presentationPassParameter.type = FINAL_RENDERING;
        mEngineParameter.ambientOcclusionParameter.vxao = false;
    }

    else {
        if (mEngineParameter.presentationPassParameter.type == AMBIENT_OCCLUSION && !mEngineParameter.ambientOcclusionParameter.enable())
            mEngineParameter.presentationPassParameter.type = FINAL_RENDERING;
    }
}

void Interface::createOpenCloseButton(std::string text, bool & button)
{
    std::string s = (button) ? "Close " : "Open ";
    if (button) {
        if (ImGui::Button((s + text).c_str()))
            button = false;
    }

    else {
        if (ImGui::Button((s + text).c_str()))
            button = true;
    }
}

void Interface::createEngineWindow()
{
    static bool o = true;
    ImGui::Begin(mName.c_str(), &o, ImGuiWindowFlags_AlwaysAutoResize);

    //	ImGui::RadioButton("Final rendering", &mEngineParameter.presentationPassParameter.type, FINAL_RENDERING);
    ImGui::RadioButton("Albedo map", &mEngineParameter.presentationPassParameter.type, ALBEDO);
    ImGui::RadioButton("Tangent Map", &mEngineParameter.presentationPassParameter.type, TANGENT);
    ImGui::RadioButton("Normal Map", &mEngineParameter.presentationPassParameter.type, NORMAL);
    ImGui::RadioButton("Ambient Occlusion", &mEngineParameter.presentationPassParameter.type, AMBIENT_OCCLUSION);
    //	ImGui::RadioButton("Voxel Grid", &mEngineParameter.presentationPassParameter.type, VOXEL_GRID);

    createOpenCloseButton("Settings Window", mSettingsOpen);
    createOpenCloseButton("Profiling Window", mProfilingOpen);
    ImGui::End();
}

void Interface::createSettingsWindow()
{
    if (mSettingsOpen) {
        ImGui::Begin("Settings Window", &mSettingsOpen, ImGuiWindowFlags_AlwaysAutoResize);

        createOpenCloseButton("Voxel Settings", mVoxelSettingsOpen);
        createOpenCloseButton("Ambient Occlusion Settings", mAOSettingsOpen);

        ImGui::End();
    }
}

void Interface::createVoxelSettingsWindow()
{
    if (mVoxelSettingsOpen) {
        ImGui::Begin("Voxel Settings", &mVoxelSettingsOpen, ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::Checkbox("Enable voxelization", &mEngineParameter.voxelizationPassParameter.enable);
        ImGui::Text("2 ^ "); ImGui::SameLine();
        ImGui::SliderInt("Grid Resolution", &mEngineParameter.voxelizationPassParameter.resolutionInPowerOfTwo, 5, 8);

        ImGui::End();
    }
}

void Interface::createAOSettingsWindow()
{
    if (mAOSettingsOpen) {
        ImGui::Begin("Ambient Occlusion Settings", &mAOSettingsOpen, ImGuiWindowFlags_AlwaysAutoResize);

        if (mEngineParameter.voxelizationPassParameter.enable) {
            ImGui::Checkbox("VXAO", &mEngineParameter.ambientOcclusionParameter.vxao);
            //	ImGui::SliderInt("Down sampling", &mEngineParameter.ambientOcclusionParameter.downSampling, 1, 8);

            if (mEngineParameter.ambientOcclusionParameter.vxao) {
                auto &p = mEngineParameter.ambientOcclusionParameter.vxaoParameter;
                ImGui::Text("VXAO Settings");
                ImGui::Separator();
                ImGui::SliderInt("Number of cones", &p.numberCones, 1, 128);
                ImGui::SliderFloat("Offset Cone in dt", &p.offsetCone, 0.5f, 4.0f);
                ImGui::SliderFloat("Over sampling", &p.oversampling, 0.1f, 4.0f);
                ImGui::SliderFloat("Distance Attenuation", &p.distanceAttenuation, 0.0f, 10.0f);
                ImGui::SliderFloat("Force", &p.force, 0.0f, 1.0f);
                ImGui::SliderFloat("Maximum Distance", &p.maximumDistance, 0.0f, 1.0f);
            }
        }

        ImGui::End();
    }
}

void Interface::createProfilingWindow() {
    if (mProfilingOpen) {
        ImGui::Begin("Profiling", &mProfilingOpen, ImGuiWindowFlags_AlwaysAutoResize);

        if (mVoxelizationProfiling != nullptr) {
            auto &[name, time] = mVoxelizationProfiling->back();
            bool open = ImGui::TreeNode((name + " : ").c_str());
            ImGui::SameLine(); ImGui::Text("%f ms", (float)time / 1000000);
            if (open) {
                for (int i = 0; i < mVoxelizationProfiling->size() - 1; ++i) {
                    auto &[name, time] = (*mVoxelizationProfiling)[i];
                    ImGui::Text("%s %f ms", name.c_str(), (float)time / 1000000);
                }
                ImGui::TreePop();
            }
        }
        ImGui::End();
    }
}
