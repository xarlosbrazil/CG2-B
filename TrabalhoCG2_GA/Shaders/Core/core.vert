#version 330 core

layout ( location = 0 ) in vec3 vPosition;
layout ( location = 1 ) in vec2 texCoord;
layout (location = 2) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{

    // Posição do vértice no mundo
    FragPos = vec3(model * vec4(aPos, 1.0));

    // Normal corrigida pela rotação do objeto
    Normal = mat3(transpose(inverse(model))) * aNormal;

    TexCoord = aTexCoord;

    gl_Position = projection * view * model * vec4(vPosition, 1.0); // Applies the MVP matrix to the vertex position

}