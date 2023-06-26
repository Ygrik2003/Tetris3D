// #version 330

varying vec2 Frag_UV;
varying vec4 Frag_Color;

uniform sampler2D u_texture;

void main()
{
    gl_FragColor = Frag_Color * texture2D(u_texture, Frag_UV);
}
