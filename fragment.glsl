#version 140

in vec3 fColor;
in vec3 normal;
in vec3 pos;
in vec3 lightDir;

uniform sampler2D tex;
uniform int texID;

out vec4 outColor;

const float ambient = 0.5f;
void main() {
    vec3 color;
    float alpha = 1.0f;
    if(texID == -1)
        color = fColor;
    else {
        color = texture(tex, gl_PointCoord).rgb;
        alpha = (color.x+color.y+color.z) / 3.0f;
    }
    vec3 diffuseC = color*max(dot(-lightDir,normal),0.0);
    vec3 ambC = color*ambient;
    vec3 viewDir = normalize(-pos); //We know the eye is at (0,0)!
    vec3 reflectDir = reflect(viewDir,normal);
    float spec = max(dot(reflectDir,lightDir),0.0);
    if (dot(-lightDir,normal) <= 0.0)spec = 0;
    vec3 specC = vec3(1.0,1.0,1.0)*pow(spec,4);
    outColor = vec4(ambC+diffuseC+specC, alpha);
}
