#include "querypool.h"

QueryPoolTimeStamp::QueryPoolTimeStamp(Device & device, uint32_t number) :
    mDevice(device),
    mNumber(number * 2) {
    vk::QueryPoolCreateInfo info{ vk::QueryPoolCreateFlags(), vk::QueryType::eTimestamp, mNumber };
    mTimestampTime = device.getTimestampTime();

    static_cast<vk::UniqueQueryPool&>(*this) = device->createQueryPoolUnique(info);

    mPreQueries.reserve(number);
    mPostQueries.reserve(number);
}

void QueryPoolTimeStamp::reset(vk::CommandBuffer cmd)
{
    cmd.resetQueryPool(*this, 0, mNumber);
    mPreQueries.clear();
    mPostQueries.clear();
    mCurrent = 0;
}

void QueryPoolTimeStamp::begin(vk::CommandBuffer cmd, std::string name)
{
    assert(mPreQueries.size() < mNumber / 2);
    mPreQueries.emplace_back(std::move(name), mCurrent);
    cmd.writeTimestamp(vk::PipelineStageFlagBits::eTopOfPipe, *this, mCurrent++);
}

void QueryPoolTimeStamp::end(vk::CommandBuffer cmd)
{
    assert(!mPreQueries.empty());
    assert(mPostQueries.size() < mNumber / 2);
    auto &[name, query] = mPreQueries.back();
    mPostQueries.emplace_back(std::move(name), query, mCurrent);
    mPreQueries.pop_back();
    cmd.writeTimestamp(vk::PipelineStageFlagBits::eBottomOfPipe, *this, mCurrent++);
}

std::vector<Profiling> QueryPoolTimeStamp::getNanoSeconds()
{
    static std::vector<uint32_t> queriesResult;
    std::vector<Profiling> results;

    assert(mNumber / 2 == mPostQueries.size());

    results.reserve(mNumber / 2);
    queriesResult.resize(mNumber);

    mDevice.getQueryPoolResults(*this, 0, mNumber, sizeof(uint32_t) * mNumber, queriesResult.data(), sizeof(uint32_t), vk::QueryResultFlagBits::eWait);

    for (auto &query : mPostQueries) {
        auto[name, pre, post] = query;
        uint32_t time = (queriesResult[post] - queriesResult[pre]) * mTimestampTime;
        results.emplace_back(std::move(name), time);
    }

    return results;
}
