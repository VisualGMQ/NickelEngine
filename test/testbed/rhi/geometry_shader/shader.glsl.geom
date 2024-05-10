#version 450

layout(points) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 outColor[];
out vec3 FragColor;

void main() {
    FragColor = outColor[0];

    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    FragColor = outColor[0];
    gl_Position = gl_in[0].gl_Position + vec4(0.1, 0, 0, 0);
    EmitVertex();

    FragColor = outColor[0];
    gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.1, 0, 0);
    EmitVertex();

    EndPrimitive();
}