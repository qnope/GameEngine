#pragma once

#include "device.h"

using Profiling = std::tuple<std::string, uint32_t>;

class QueryPoolTimeStamp : public vk::UniqueQueryPool {
public:
    QueryPoolTimeStamp(Device &device, uint32_t number);

    void reset(vk::CommandBuffer cmd);

    void begin(vk::CommandBuffer cmd, std::string name);

    void end(vk::CommandBuffer cmd);

    std::vector<Profiling> getNanoSeconds();

private:
    vk::Device mDevice;
    uint32_t mNumber;
    std::vector<std::tuple<std::string, uint32_t>> mPreQueries;
    std::vector<std::tuple<std::string, uint32_t, uint32_t>> mPostQueries;
    uint32_t mTimestampTime;
    uint32_t mCurrent{ 0u };
};
