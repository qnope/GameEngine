glslangValidator.exe -V RenderFullScreen\quad.vert -o RenderFullScreen\quad_vert.spv
glslangValidator.exe -V RenderFullScreen\visualizationbuffer.frag -o RenderFullScreen\visualizationbuffer.spv
glslangValidator.exe -V RenderFullScreen\finalrendering.frag -o RenderFullScreen\finalrendering.spv

glslangValidator.exe -V RenderSceneGraphRelated\renderscenegraph.vert -o RenderSceneGraphRelated\renderscenegraph_vert.spv
glslangValidator.exe -V RenderSceneGraphRelated\fillgbufferalbedocolor.frag -o RenderSceneGraphRelated\fillgbufferalbedocolor_frag.spv
glslangValidator.exe -V RenderSceneGraphRelated\fillgbufferalbedotexture.frag -o RenderSceneGraphRelated\fillgbufferalbedotexture_frag.spv
glslangValidator.exe -V RenderSceneGraphRelated\fillgbufferpbrtexture.frag -o RenderSceneGraphRelated\fillgbufferpbrtexture_frag.spv

glslangValidator.exe -V ImGUI\imgui.vert -o ImGUI\imgui_vert.spv
glslangValidator.exe -V ImGUI\imgui.frag -o ImGUI\imgui_frag.spv

glslangValidator.exe -V Voxelization\voxelization.vert -o Voxelization\voxelization_vert.spv
glslangValidator.exe -V Voxelization\voxelization.geom -o Voxelization\voxelization_geom.spv
glslangValidator.exe -V Voxelization\voxelizationgeometry.frag -o Voxelization\voxelizationgeometry_frag.spv

glslangValidator.exe -V Voxelization\geometrydownsampler.comp -o Voxelization\geometrydownsampler.spv

glslangValidator.exe -V Voxelization\visualization.frag -o Voxelization\visualization.spv
glslangValidator.exe -V Voxelization\vxao.frag -o Voxelization\vxao.spv

pause