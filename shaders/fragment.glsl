#version 330 core

in vec3 color; 
in vec2 TexCoord;
out vec4 frag_color;

uniform sampler2D vramTexture4;
uniform sampler2D vramTexture16;
uniform int clut4[16];
uniform int colorDepth;

vec4 split_colors(int data)
{
    // TODO: Check if the value shouldn't be shifted to get a range of [0 to 31], set alpha to 31.0f and divide by 31.0f outside
    vec4 color;
    color.r = (data << 3) & 0xf8; // This will give a range of 0, [8, 248]. Not 0 to 255
    color.g = (data >> 2) & 0xf8;
    color.b = (data >> 7) & 0xf8;
    color.a = 255.0f;

    return color;
}

vec4 sample_texel()
{
    if (colorDepth == 4)
    {
        int index = int(texture2D(vramTexture4, TexCoord / vec2(4096.0, 512.0)).r * 255.0);
        int texel = clut4[int(index)];
        return split_colors(texel) / vec4(255.0f);
    }
    else
    {
        return vec4(1.0, 1.0, 0.0, 1.0);
        // TODO: vramTexture16 here
        //int texel = int(texture2D(vramTexture4, TexCoord).r * 255);
        //return split_colors(texel) / vec4(255.0f);
    }
}

void main() 
{
    if (TexCoord != vec2(69.0, 69.0))
    {
       frag_color = sample_texel();
    }
    else
    {
       frag_color = vec4(color, 1.0);
    }
}