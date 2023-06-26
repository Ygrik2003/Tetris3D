#version 330

in vec2 Frag_UV;
in vec4 Frag_Color;

uniform sampler2D u_texture;

out vec4 o_color;

void main()
{
    o_color = Frag_Color * texture(u_texture, Frag_UV);
}
