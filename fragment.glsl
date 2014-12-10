#version 140

in vec3 fColor;
in vec3 normal;

out vec4 outColor;

void main() {
    outColor = vec4(fColor, 1.0);
}
