#version 450

layout(points) in;
layout(triangle_strip, max_vertices = 3) out;

layout(location = 0) in vec3 outColor[];
layout(location = 0) out vec3 fragColor;

void main() {
    fragColor = outColor[0];

    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    fragColor = outColor[0];
    gl_Position = gl_in[0].gl_Position + vec4(0.1, 0, 0, 0);
    EmitVertex();

    fragColor = outColor[0];
    gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.1, 0, 0);
    EmitVertex();

    EndPrimitive();
}