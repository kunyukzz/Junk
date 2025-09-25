glslc -fshader-stage=vert assets/shaders/triangle.vert.glsl -o assets/shaders/triangle.vert.spv
glslc -fshader-stage=frag assets/shaders/triangle.frag.glsl -o assets/shaders/triangle.frag.spv

glslc -fshader-stage=vert assets/shaders/ui.vert.glsl -o assets/shaders/ui.vert.spv
glslc -fshader-stage=frag assets/shaders/ui.frag.glsl -o assets/shaders/ui.frag.spv

glslc -fshader-stage=vert assets/shaders/ui.vert.glsl -o assets/shaders/test.vert.spv
glslc -fshader-stage=frag assets/shaders/ui.frag.glsl -o assets/shaders/test.frag.spv