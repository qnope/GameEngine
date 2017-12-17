#pragma once

#include "rendererfacade.h"

class Interface {
public:
    Interface(std::string name);
    EngineParameter getParameters() const;

    virtual void execute();

    void setVoxelizationProfiling(const std::vector<Profiling> *ptr);

    void checkError();

private:
    std::string mName;
    bool mSettingsOpen{ false };
    bool mVoxelSettingsOpen{ false };
    bool mAOSettingsOpen{ false };
    bool mVisualizeSettingsOpen{ false };
    bool mProfilingOpen{ false };
    EngineParameter mEngineParameter;

    const std::vector<Profiling> *mVoxelizationProfiling{ nullptr };

    void createOpenCloseButton(std::string text, bool &button);
    void createEngineWindow();
    void createSettingsWindow();
    void createVoxelSettingsWindow();
    void createAOSettingsWindow();
    void createProfilingWindow();
};
