#version 300 es
precision mediump float;

in vec3 v_position;
in vec2 v_tex_coord;
in vec3 v_normal;
in vec3 camera_pos;

uniform sampler2D u_texture;

const vec3  light_color      = vec3(1., 1., 1.);
const float ambient_strength = 0.3f;
const vec3  ambient          = ambient_strength * light_color;

out vec4 o_color;

void main()
{
    vec3 light_pos = camera_pos;
    vec3 v_normal_facing;
    if (gl_FrontFacing)
        v_normal_facing = -v_normal;
    else
        v_normal_facing = v_normal;

    vec4 color = texture(u_texture, v_tex_coord);

    vec3  light_dir = normalize(light_pos - v_position);
    float diff      = max(dot(v_normal_facing, light_dir), 0.);
    vec3  diffuse   = diff * light_color;

    float specular_strength = 0.9f;
    vec3  view_dir          = normalize(camera_pos - v_position);
    vec3  reflect_dir       = reflect(-light_dir, v_normal_facing);

    vec3 dist = v_position - light_pos;

    vec3 result = (ambient + diffuse) * color.xyz;

    o_color = vec4(result, color.w);
}