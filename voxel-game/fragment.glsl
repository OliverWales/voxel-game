#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;

// texture sampler
uniform sampler2D texture1;

void main()
{
    mediump vec4 ambient = vec4(0.52, 0.74, 0.98, 1.0);
    mediump vec3 lightColour = vec3(0.5, 0.5, 0.4);
    mediump vec3 lightDirection = normalize(vec3(-0.2, 1.0, -0.3));
    mediump vec3 directional = lightColour * max(dot(Normal, lightDirection), 0.0);

    FragColor = (ambient + vec4(directional, 1.0)) * texture(texture1, TexCoord);  
}
