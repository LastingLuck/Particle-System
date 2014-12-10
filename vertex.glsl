#version 140

in vec3 position;
uniform vec3 inColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 fColor;
//out vec3 normal;

void main() {
    fColor = inColor;
    gl_Position = proj * view * model * vec4(position, 1.0);
    //normal = normalize(-gl_Position);
}
