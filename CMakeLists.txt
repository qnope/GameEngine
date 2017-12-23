cmake_minimum_required(VERSION 3.4)

project(GameEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

LINK_DIRECTORIES(libraries/lib)
INCLUDE_DIRECTORIES(libraries/include)
INCLUDE_DIRECTORIES(Moteur)

add_executable(GameEngine
    Moteur/main.cpp
    Moteur/interface.cpp
    Moteur/rendererfacade.cpp

    # Descriptor
    Moteur/Descriptor/descriptorpoolwrapper.cpp
    ##########################################################

    # imgui
    Moteur/imgui/imgui.cpp
    Moteur/imgui/imgui_demo.cpp
    Moteur/imgui/imgui_draw.cpp
    Moteur/imgui/imguiinstance.cpp
    ##########################################################

    # Passes
    Moteur/Pass/ImGui/imguipass.cpp
    Moteur/Pass/ImGui/imguisubpass.cpp

    Moteur/Pass/RenderFullScreen/fullscreensubpass.cpp
    Moteur/Pass/RenderFullScreen/renderfullscreenpass.cpp

    Moteur/Pass/RenderSceneGraph/fillgbuffersubpass.cpp
    Moteur/Pass/RenderSceneGraph/renderdeferredpasses.cpp

    Moteur/Pass/Voxelization/voxelizationgeometrysubpass.cpp
    Moteur/Pass/Voxelization/voxelizationpass.cpp
    Moteur/Pass/Voxelization/vxaopass.cpp

    Moteur/Pass/framebufferbuilder.cpp
    Moteur/Pass/pipelinebuilder.cpp
    Moteur/Pass/renderpass.cpp
    Moteur/Pass/renderpassbuilder.cpp
    Moteur/Pass/subpass.cpp
    ###########################################################

    # Physics
    Moteur/Physics/dynaobject.cpp
    ###########################################################

    # SceneGraph
    Moteur/SceneGraph/Material/abstractuniquematerialmanager.cpp
    Moteur/SceneGraph/Material/albedocolormaterialmanager.cpp
    Moteur/SceneGraph/Material/albedotexturematerialmanager.cpp
    Moteur/SceneGraph/Material/materialsmanager.cpp
    Moteur/SceneGraph/Material/pbrnormaltexturematerialmanager.cpp
    Moteur/SceneGraph/ModelImporter/material.cpp
    Moteur/SceneGraph/ModelImporter/mesh.cpp
    Moteur/SceneGraph/ModelImporter/modelimporter.cpp
    Moteur/SceneGraph/entity.cpp
    Moteur/SceneGraph/entitymanager.cpp
    Moteur/SceneGraph/meshmanager.cpp
    Moteur/SceneGraph/node.cpp
    Moteur/SceneGraph/scenegraph.cpp
    Moteur/SceneGraph/scenegraphbuffer.cpp
    ############################################################

    # Tools
    Moteur/Tools/geometry.cpp
    Moteur/Tools/Logger.cpp
    ############################################################

    # Transfer
    Moteur/Transfer/bufferfactory.cpp
    Moteur/Transfer/imagefactory.cpp
    ############################################################

    # Window
    Moteur/Window/window.cpp
    ############################################################

    # Vulkan
    Moteur/Vulkan/buffer.cpp
    Moteur/Vulkan/chunkallocator.cpp
    Moteur/Vulkan/commandbuffersubmitter.cpp
    Moteur/Vulkan/commandpool.cpp
    Moteur/Vulkan/descriptorpool.cpp
    Moteur/Vulkan/descriptorsetlayout.cpp
    Moteur/Vulkan/device.cpp
    Moteur/Vulkan/deviceallocator.cpp
    Moteur/Vulkan/fence.cpp
    Moteur/Vulkan/framebuffer.cpp
    Moteur/Vulkan/image.cpp
    Moteur/Vulkan/imageloader.cpp
    Moteur/Vulkan/imageview.cpp
    Moteur/Vulkan/input.cpp
    Moteur/Vulkan/instance.cpp
    Moteur/Vulkan/memoryblock.cpp
    Moteur/Vulkan/memorychunk.cpp
    Moteur/Vulkan/pipeline.cpp
    Moteur/Vulkan/pipelinelayout.cpp
    Moteur/Vulkan/querypool.cpp
    Moteur/Vulkan/ResourceLimits.cpp
    Moteur/Vulkan/sampler.cpp
    Moteur/Vulkan/shader.cpp
    Moteur/Vulkan/ShaderCompiler.cpp
    Moteur/Vulkan/structhelper.cpp
    Moteur/Vulkan/swapchain.cpp
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
ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)
