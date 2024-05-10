#version 430

in FsIn {
    vec3 position;
    vec2 uv;
    vec3 normal;
} fs_in;

out vec3 outPosition;
out vec3 outNormal;

void main() {
    outPosition = fs_in.position;
    outNormal = fs_in.normal;
}