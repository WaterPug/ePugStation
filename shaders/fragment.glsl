#version 330 core
 
in vec3 color; 
in vec2 tex_coords;
out vec4 frag_color;
 
uniform int texture_depth;

uniform sampler2D vramTexture;

uniform int clut4[16];

vec4 split_colors(int data)
{
    vec4 color;
    color.r = (data << 3) & 0xf8;
    color.g = (data >> 2) & 0xf8;
    color.b = (data >> 7) & 0xf8;
    color.a = 255.0f;

    return color;
}

vec4 sample_texel()
{
    // For now only testing with depth 4
    vec4 index = texture2D(vramTexture, tex_coords);
    int texel = clut4[int(index.r * 255)];

    return split_colors(texel) / vec4(255.0f);
}

void main() 
{ 
    frag_color = vec4(color, 1.0);
    //if (tex_coords.x == -1 && tex_coords.y == -1) // hack for color vs tex ? See if better solution
    //{
    //    frag_color = vec4(color, 1.0);
    //}
    //else
    //{
    //    frag_color = sample_texel(); //todo
    //}
}