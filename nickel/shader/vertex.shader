#version 450 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec4 aColor;

out vec2 TexCoord;
out vec4 Color;

uniform mat4 Project;
uniform mat4 Model;

void main() {
    TexCoord = aTexCoord;
    Color = aColor;
    gl_Position = Project * Model * vec4(aPos, 0.0, 1.0);
}
