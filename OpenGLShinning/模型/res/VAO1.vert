#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0); //
    gl_Position = pos.xyww; 
    //结果就是天空盒只会在没有可见物体的地方渲染了
    //（只有这样才能通过深度测试，其它所有的东西都在天空盒前面）。
}  