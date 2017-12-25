cmake_minimum_required(VERSION 3.10)

project(GameEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

INCLUDE_DIRECTORIES(libraries/include)
INCLUDE_DIRECTORIES(Moteur)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /permissive- /MP")
    LINK_DIRECTORIES(libraries/lib)
endif()

add_executable(GameEngine
    Moteur/main.cpp
    Moteur/Interface.cpp
    Moteur/RendererFacade.cpp

    # Descriptor
    Moteur/Descriptor/DescriptorPoolWrapper.cpp
    ##########################################################

    # imgui
    Moteur/imgui/imgui.cpp
    Moteur/imgui/imgui_demo.cpp
    Moteur/imgui/imgui_draw.cpp
    Moteur/imgui/ImGUIInstance.cpp
    ##########################################################

    # ShaderGenerator
    Moteur/ShaderGenerator/ShaderGenerator.cpp
    ##########################################################

    # Passes
    Moteur/Pass/ImGui/ImGUIPass.cpp
    Moteur/Pass/ImGui/ImGUISubpass.cpp

    Moteur/Pass/RenderFullScreen/FullScreenSubpass.cpp
    Moteur/Pass/RenderFullScreen/RenderFullscreenPass.cpp

    Moteur/Pass/RenderSceneGraph/FillGBufferSubpass.cpp
    Moteur/Pass/RenderSceneGraph/RenderDeferredPasses.cpp

    Moteur/Pass/Voxelization/VoxelizationGeometrySubpass.cpp
    Moteur/Pass/Voxelization/VoxelizationPass.cpp
    Moteur/Pass/Voxelization/VXAOPass.cpp

    Moteur/Pass/FramebufferBuilder.cpp
    Moteur/Pass/PipelineBuilder.cpp
    Moteur/Pass/RenderPass.cpp
    Moteur/Pass/RenderPassBuilder.cpp
    Moteur/Pass/Subpass.cpp
    ###########################################################

    # Physics
    Moteur/Physics/DynaObject.cpp
    ###########################################################

    # SceneGraph
    Moteur/SceneGraph/Material/AbstractUniqueMaterialManager.cpp
    Moteur/SceneGraph/Material/AlbedoColorMaterialManager.cpp
    Moteur/SceneGraph/Material/AlbedoTextureMaterialManager.cpp
    Moteur/SceneGraph/Material/MaterialsManager.cpp
    Moteur/SceneGraph/Material/PBRNormalTextureMaterialManager.cpp
    Moteur/SceneGraph/ModelImporter/Material.cpp
    Moteur/SceneGraph/ModelImporter/Mesh.cpp
    Moteur/SceneGraph/ModelImporter/ModelImporter.cpp
    Moteur/SceneGraph/Entity.cpp
    Moteur/SceneGraph/EntityManager.cpp
    Moteur/SceneGraph/MeshManager.cpp
    Moteur/SceneGraph/Node.cpp
    Moteur/SceneGraph/SceneGraph.cpp
    Moteur/SceneGraph/SceneGraphBuffer.cpp
    ############################################################

    # Tools
    Moteur/Tools/Geometry.cpp
    Moteur/Tools/Logger.cpp
    ############################################################

    # Transfer
    Moteur/Transfer/BufferFactory.cpp
    Moteur/Transfer/ImageFactory.cpp
    ############################################################

    # Window
    Moteur/Window/Window.cpp
    ############################################################

    # Vulkan
    Moteur/Vulkan/Buffer.cpp
    Moteur/Vulkan/ChunkAllocator.cpp
    Moteur/Vulkan/CommandBufferSubmitter.cpp
    Moteur/Vulkan/CommandPool.cpp
    Moteur/Vulkan/DescriptorPool.cpp
    Moteur/Vulkan/DescriptorSetLayout.cpp
    Moteur/Vulkan/Device.cpp
    Moteur/Vulkan/DeviceAllocator.cpp
    Moteur/Vulkan/Fence.cpp
    Moteur/Vulkan/Framebuffer.cpp
    Moteur/Vulkan/Image.cpp
    Moteur/Vulkan/ImageLoader.cpp
    Moteur/Vulkan/ImageView.cpp
    Moteur/Vulkan/Input.cpp
    Moteur/Vulkan/Instance.cpp
    Moteur/Vulkan/MemoryBlock.cpp
    Moteur/Vulkan/MemoryChunk.cpp
    Moteur/Vulkan/Pipeline.cpp
    Moteur/Vulkan/PipelineLayout.cpp
    Moteur/Vulkan/QueryPool.cpp
    Moteur/Vulkan/ResourceLimits.cpp
    Moteur/Vulkan/Sampler.cpp
    Moteur/Vulkan/Shader.cpp
    Moteur/Vulkan/ShaderCompiler.cpp
    Moteur/Vulkan/StructHelper.cpp
    Moteur/Vulkan/Swapchain.cpp
    )

TARGET_LINK_LIBRARIES(GameEngine
    vulkan-1
    glfw3
    assimp
    SDL2
    SDL2_image
    SDL2main
    )

if(CMAKE_BUILD_TYPE MATCHES Debug)
    MESSAGE( "Debug mode is used" )
    TARGET_LINK_LIBRARIES(GameEngine
        glslangd
        OGLCompilerd
        OSDependentd
        SPIRVd
    )

else()
    MESSAGE( "Release mode is used" )
    TARGET_LINK_LIBRARIES(GameEngine
        glslang
        OGLCompiler
        OSDependent
        SPIRV
    )
ENDIF()
