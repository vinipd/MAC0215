#version 330
#extension GL_EXT_geometry_shader4: enable

#define sampling 15

layout(triangles) in;
layout(line_strip) out;
layout (max_vertices =  sampling) out;

out vec4 pixel_color;

vec3 bezier_quadratic_eval (float t, vec3 control_p[3]) {
    vec3 p[3];
    p[0] = control_p[0];
    p[1] = control_p[1];
    p[2] = control_p[2];

    float one_minus = 1.0 - t;
    float one_minus_2 = one_minus * one_minus;
    float one_minus_t_2 = one_minus * t * 2.0;
    float t_2 = t * t;

    vec3 ans = one_minus_2 * p[0] + one_minus_t_2 * p[1] + t_2 * p[2];

    return ans/(sampling * sampling);
}

void test () {
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    EndPrimitive();

}

void main() {
    vec3 control_p[3];
    control_p[0] = gl_in[0].gl_Position.xyz;
    control_p[1] = gl_in[1].gl_Position.xyz;
    control_p[2] = gl_in[2].gl_Position.xyz;

    float delta = 1/float(sampling-1.0);

    for (int i = 0; i < sampling; ++i) {
        float t = i * delta;
        vec3 sample = bezier_quadratic_eval (t, control_p);
        gl_Position = vec4(sample.xyz,1.0);
        pixel_color = vec4 (0,0,0,1);
        EmitVertex();
    }
    EndPrimitive();
}
