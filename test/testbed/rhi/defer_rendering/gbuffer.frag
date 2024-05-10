#version 450

layout(location = 0) in FsIn {
    vec3 position;
    vec2 uv;
    vec3 normal;
} fs_in;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;

void main() {
    outPosition = fs_in.position;
    outNormal = fs_in.normal;
}