#version 450 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Pos;

uniform sampler2D texture0;
uniform sampler2D texture1;

uniform float mixValue;

void main()
{
    // FragColor = vec4( (Pos.x + 0.5f) / 2,  (Pos.y + 0.5f) / 2, (Pos.z + 0.5f) / 2, 1.0f);
    // FragColor = texture(texture1, TexCoord);
    FragColor = mix(texture(texture0, TexCoord), texture(texture1, TexCoord), mixValue);
}