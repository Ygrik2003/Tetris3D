// #version 330

attribute vec2 i_position;
attribute vec2 i_tex_coord;
attribute vec4 i_color;

uniform float width;
uniform float height;

varying vec2 Frag_UV;
varying vec4 Frag_Color;

void main()
{
    Frag_UV     = i_tex_coord;
    Frag_Color  = i_color;
    gl_Position = vec4(
        2. * vec3(i_position.x / width, 1. - i_position.y / height, 0.) - 1.,
        1.);
}