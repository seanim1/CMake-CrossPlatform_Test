set GLSLC=C:\VulkanSDK\1.3.296.0\Bin\glslc.exe

%GLSLC% box.vert -o ../shaderBinary/box.vert.spv -O -fshader-stage=vertex

%GLSLC% box.frag -o ../shaderBinary/box.frag.spv -O -fshader-stage=fragment

pause