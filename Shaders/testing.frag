#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform bool enable_texture;
uniform vec3 color;
uniform sampler2D texture0;

void main() {
    if (enable_texture) {
        FragColor = texture(texture0, TexCoord);
    } else {
        FragColor = vec4(color, 1.0f);
    }  
} 