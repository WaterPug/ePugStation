#version 330 core

in ivec2 vertexPosition; 
in uvec3 vertexColor;

out vec3 color;

uniform ivec2 offset;

void main() 
{ 
    ivec2 position = vertexPosition + offset;
    // Convert VRAM coordinates (0;1023, 0;511) into
    // OpenGL coordinates (-1;1 , -1;1)
    float xpos = (float(position.x) / 512) - 1.0;
    // VRAM puts 0 at the top, OpenGL at the bottom,
    // we must mirror vertically.
    float ypos = 1.0 - (float(position.y) / 256);
    gl_Position.xyzw = vec4(xpos, ypos, 0.0, 1.0);

    // Convert the components from [0;255] to [0;1]
    color = vec3(float(vertexColor.r) / 255,
                 float(vertexColor.g) / 255,
                 float(vertexColor.b) / 255);
}