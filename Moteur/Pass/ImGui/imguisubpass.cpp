#include "ImGUISubpass.h"

ImGUISubpass::ImGUISubpass(ImGUIInstance &imGUIInstance, vk::Device device, BufferFactory & bufferFactory) :
    mImGUIInstance(imGUIInstance),
    mDevice(device),
    mBufferFactory(bufferFactory)
{
    addColorAttachment(0);
}

void ImGUISubpass::execute(vk::CommandBuffer cmd)
{
    auto drawData{ ImGui::GetDrawData() };
    auto io{ ImGui::GetIO() };

    auto vertexSize = drawData->TotalVtxCount * sizeof(ImDrawVert);

    if (vertexSize == 0)
        return;

    if (mVbo == nullptr || mVbo->getSize() < vertexSize)
        mVbo = std::make_unique<Buffer>(mBufferFactory.createEmptyBuffer(vertexSize, vk::BufferUsageFlagBits::eVertexBuffer, false));

    auto indexSize = drawData->TotalIdxCount * sizeof(ImDrawIdx);
    if (mIbo == nullptr || mIbo->getSize() < indexSize)
        mIbo = std::make_unique<Buffer>(mBufferFactory.createEmptyBuffer(indexSize, vk::BufferUsageFlagBits::eIndexBuffer, false));

    mVbo->resetOffset();
    mIbo->resetOffset();

    for (int i = 0; i < drawData->CmdListsCount; ++i) {
        const auto list = drawData->CmdLists[i];

        mVbo->push_data(list->VtxBuffer.Data, list->VtxBuffer.Size * sizeof(ImDrawVert));
        mIbo->push_data(list->IdxBuffer.Data, list->IdxBuffer.Size * sizeof(ImDrawIdx));
    }

    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline);
    cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mImGUIInstance.pipelineLayout, 0, mImGUIInstance.descriptorSet, {});
    cmd.bindVertexBuffers(0, (vk::Buffer)*mVbo, vk::DeviceSize{ 0 });
    cmd.bindIndexBuffer((vk::Buffer)*mIbo, 0, vk::IndexType::eUint16);

    vk::Viewport viewport(0.0f, 0.0f, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y, 0.0f, 1.0f);
    cmd.setViewport(0, viewport);

    float scale[2], translate[2];
    scale[0] = 2.0f / io.DisplaySize.x;
    scale[1] = 2.0f / io.DisplaySize.y;
    translate[0] = -1.0f;
    translate[1] = -1.0f;

    cmd.pushConstants(mImGUIInstance.pipelineLayout, vk::ShaderStageFlagBits::eVertex, (uint32_t)0, vk::ArrayProxy<const float>{(uint32_t)2, scale});
    cmd.pushConstants(mImGUIInstance.pipelineLayout, vk::ShaderStageFlagBits::eVertex, (uint32_t)(2 * sizeof(float)), vk::ArrayProxy<const float>{(uint32_t)2, translate});

    int vtxOffset{ 0 };
    int idxOffset{ 0 };

    for (int i = 0; i < drawData->CmdListsCount; ++i) {
        const auto list = drawData->CmdLists[i];

        for (int j = 0; j < list->CmdBuffer.Size; ++j) {
            const auto pcmd = &list->CmdBuffer[j];

            vk::Rect2D scissor;
            scissor.offset.x = (int32_t)(pcmd->ClipRect.x) > 0 ? (int32_t)(pcmd->ClipRect.x) : 0;
            scissor.offset.y = (int32_t)(pcmd->ClipRect.y) > 0 ? (int32_t)(pcmd->ClipRect.y) : 0;
            scissor.extent.width = (uint32_t)(pcmd->ClipRect.z - pcmd->ClipRect.x);
            scissor.extent.height = (uint32_t)(pcmd->ClipRect.w - pcmd->ClipRect.y + 1); // FIXME: Why +1 here?

            cmd.setScissor(0, scissor);
            cmd.drawIndexed(pcmd->ElemCount, 1, idxOffset, vtxOffset, 0);
            idxOffset += pcmd->ElemCount;
        }
        vtxOffset += list->VtxBuffer.Size;
    }
}

void ImGUISubpass::create(vk::RenderPass renderpass, vk::Extent2D extent, uint32_t indexPass)
{
    mPipeline = PipelineBuilder::buildImGUIPipeline(mImGUIInstance, renderpass);
}
