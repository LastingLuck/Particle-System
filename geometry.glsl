#version 140

layout(points) in;
//layout(points, max_vertices = 1) out;
layout(triangle_strip, max_vertices = 4) out;

void main() {
    gl_Position = gl_in[0].gl_Position + vec4(1.0, 0.0, 1.0, 0.0);
    EmitVertex();
    
    gl_Position = gl_in[0].gl_Position + vec4(1.0, 0.0, -1.0, 0.0);
    EmitVertex();
    
    gl_Position = gl_in[0].gl_Position + vec4(-1.0, 0.0, -1.0, 0.0);
    EmitVertex();
    
    gl_Position = gl_in[0].gl_Position + vec4(-1.0, 0.0, 1.0, 0.0);
    EmitVertex();
    
    EndPrimitive();
}
