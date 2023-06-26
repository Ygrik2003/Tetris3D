#version 330
precision mediump float;

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec3 i_normal;
layout(location = 2) in vec2 i_tex_coord;
layout(location = 3) in vec4 i_color;

out vec3 v_position;
out vec3 v_normal;
out vec2 v_tex_coord;
out vec3 camera_pos;

uniform vec3  u_normal;
uniform float u_alpha;            // For animation
uniform vec2  u_size_window;      // Resolution
uniform vec3  u_rotate_obj;       // Rotate object
uniform vec3  u_rotate_camera;    // Rotate camera
uniform vec3  u_translate_obj;    // Translate object
uniform vec3  u_translate_camera; // Translate camera
uniform vec3  u_scale_obj;        // Scale object

const float front = 0.01f;
const float back  = 30.f;
float       fovy  = 3.14159 / 2.;

mat4 rotate_matrix(vec3 rotate)
{
    return mat4(vec4(cos(rotate.x) * cos(rotate.y),
                     sin(rotate.x) * cos(rotate.y),
                     -sin(rotate.y),
                     0),
                vec4(cos(rotate.x) * sin(rotate.y) * sin(rotate.z) -
                         sin(rotate.x) * cos(rotate.z),
                     sin(rotate.x) * sin(rotate.y) * sin(rotate.z) +
                         cos(rotate.x) * cos(rotate.z),
                     cos(rotate.y) * sin(rotate.z),
                     0),
                vec4(cos(rotate.x) * sin(rotate.y) * cos(rotate.z) +
                         sin(rotate.x) * sin(rotate.z),
                     sin(rotate.x) * sin(rotate.y) * cos(rotate.z) -
                         cos(rotate.x) * sin(rotate.z),
                     cos(rotate.y) * cos(rotate.z),
                     0),
                vec4(0., 0., 0., 1.));
}

mat4 translate_matrix(vec3 tr)
{
    return mat4(vec4(1., 0., 0., tr.x),
                vec4(0., 1., 0., tr.y),
                vec4(0., 0., 1., tr.z),
                vec4(0., 0., 0., 1.));
}

mat4 scale_matrix(vec3 scale)
{
    return mat4(vec4(scale.x, 0., 0., 0.),
                vec4(0., scale.y, 0., 0.),
                vec4(0., 0., scale.z, 0.),
                vec4(0., 0., 0., 1.));
}

mat4 perspective_matrix(float fovy, float aspect, float front, float back)
{
    return mat4(vec4(1. / (aspect * tan(fovy / 2.)), 0., 0., 0.),
                vec4(0., 1. / tan(fovy / 2.), 0., 0.),
                vec4(0.,
                     0.,
                     (back + front) / (back - front),
                     -2. * back * front / (back - front)),
                vec4(0., 0., 1., 0.));
}

void main()
{

    mat4 projection = perspective_matrix(
        fovy, u_size_window.x / u_size_window.y, front, back);

    v_tex_coord = i_tex_coord;
    camera_pos  = -u_translate_camera;

    mat4 model = scale_matrix(u_scale_obj) * translate_matrix(u_translate_obj) *
                 rotate_matrix(u_rotate_obj);

    v_position = vec3(vec4(i_position, 1.) * model);

    v_normal = normalize((vec4(i_normal, 0.f) * model).xyz);

    gl_Position = vec4(v_position, 1.) * translate_matrix(u_translate_camera) *
                  rotate_matrix(u_rotate_camera) * projection;
}