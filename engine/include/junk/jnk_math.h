#ifndef JUNK_MATH_H
#define JUNK_MATH_H

#include "jnk_define.h"
#include "jnk_math_type.h"

#define PI 3.14159265358979323846f
#define SQRT_2 1.41421356237309504880f
#define SQRT_3 1.73205080756887729252f
#define SEC2MS_MULTI 1000.0f
#define MS2SEC_MULTI 0.001f
#define INFINITE 1e30f
#define EPSILON 1.192092896e-07f

#define PI2 (2.0f * PI)
#define HALF_PI (0.5f * PI)
#define QUARTER_PI (0.25f * PI)
#define DEG2RAD (PI / 180.0f)
#define RAD2DEG (180.0f / PI)

JNK_EXTERN_C_BEGIN

JNK_INL b8 mat4_has_nan(const mat4 *m) {
    for (u64 i = 0; i < 16; i++) {
        if (m->data[i] != m->data[i]) {
            return true;
        }
    }
    return false;
}

JNK_API f32 jsinf(f32 x);
JNK_API f32 jcosf(f32 x);
JNK_API f32 jtanf(f32 x);
JNK_API f32 jacosf(f32 x);
JNK_API f32 jsqrtf(f32 x);
JNK_API f32 jabsf(f32 x);

JNK_API i32 random(void);
JNK_API i32 random_in_range(i32 min, i32 max);
JNK_API f32 frandom(void);
JNK_API f32 frandom_in_range(f32 min, f32 max);

JNK_INL b8 power_of_2(u64 value) {
    return (value != 0) && ((value & (value - 1)) == 0);
}

JNK_INL f32 deg_to_rad(f32 degree) { return degree * DEG2RAD; }
JNK_INL f32 rad_to_deg(f32 rad) { return rad * RAD2DEG; }

// Vector 2
JNK_API vec2 vec2_create(f32 x, f32 y);
JNK_API vec2 vec2_zero(void);
JNK_API vec2 vec2_one(void);
JNK_API vec2 vec2_add(vec2 v1, vec2 v2);
JNK_API vec2 vec2_sub(vec2 v1, vec2 v2);
JNK_API vec2 vec2_multi(vec2 v1, vec2 v2);
JNK_API vec2 vec2_divide(vec2 v1, vec2 v2);

JNK_API f32 vec2_length_square(vec2 v);
JNK_API f32 vec2_length(vec2 v);
JNK_API f32 vec2_dot(vec2 v1, vec2 v2);
JNK_API f32 vec2_distance(vec2 v1, vec2 v2);

// Vector 3
JNK_API vec3 vec3_create(f32 x, f32 y, f32 z);
JNK_API vec3 vec3_zero(void);
JNK_API vec3 vec3_one(void);
JNK_API vec3 vec3_add(vec3 v1, vec3 v2);
JNK_API vec3 vec3_sub(vec3 v1, vec3 v2);
JNK_API vec3 vec3_multi(vec3 v1, vec3 v2);
JNK_API vec3 vec3_divide(vec3 v1, vec3 v2);
JNK_API vec3 vec3_multi_scalar(vec3 v, f32 s);

JNK_API f32 vec3_length_square(vec3 v);
JNK_API f32 vec3_length(vec3 v);
JNK_API f32 vec3_dot(vec3 v1, vec3 v2);
JNK_API vec3 vec3_cross(vec3 v1, vec3 v2);
JNK_API void vec3_normalized(vec3 *v);
JNK_API vec3 vec3_get_normalized(vec3 v);
JNK_API f32 vec3_distance(vec3 v1, vec3 v2);
JNK_API b8 vec3_compared(vec3 v1, vec3 v2, f32 tolerance);

// Vector 4
JNK_API vec4 vec4_create(f32 x, f32 y, f32 z, f32 w);
JNK_API vec4 vec4_zero(void);
JNK_API vec4 vec4_one(void);
JNK_API vec4 vec4_add(vec4 v1, vec4 v2);
JNK_API vec4 vec4_sub(vec4 v1, vec4 v2);
JNK_API vec4 vec4_multi(vec4 v1, vec4 v2);
JNK_API vec4 vec4_divide(vec4 v1, vec4 v2);

JNK_API f32 vec4_length_square(vec4 v);
JNK_API f32 vec4_length(vec4 v);
JNK_API void vec4_normalized(vec4 *v);
JNK_API vec4 vec4_get_normalized(vec4 v);
JNK_API f32 vec4_dot_f32(f32 a0, f32 a1, f32 a2, f32 a3, f32 b0, f32 b1, f32 b2,
                         f32 b3);

// Matrix 4
JNK_API mat4 mat4_identity(void);
JNK_API mat4 mat4_translate(vec3 pos);
JNK_API mat4 mat4_scale(vec3 scale);

JNK_API vec3 mat4_forward(mat4 matrix);
JNK_API vec3 mat4_backward(mat4 matrix);
JNK_API vec3 mat4_up(mat4 matrix);
JNK_API vec3 mat4_down(mat4 matrix);
JNK_API vec3 mat4_left(mat4 matrix);
JNK_API vec3 mat4_right(mat4 matrix);

JNK_API vec4 mat4_mul_vec4(mat4 m, vec4 v);

JNK_API mat4 mat4_euler_x(f32 angle_rad);
JNK_API mat4 mat4_euler_y(f32 angle_rad);
JNK_API mat4 mat4_euler_z(f32 angle_rad);
JNK_API mat4 mat4_euler_xyz(f32 x_rad, f32 y_rad, f32 z_rad);

JNK_API mat4 mat4_row_multi(mat4 m1, mat4 m2);
JNK_API mat4 mat4_column_multi(mat4 m1, mat4 m2);

JNK_API mat4 mat4_row_ortho(f32 left, f32 right, f32 bottom, f32 top, f32 near,
                            f32 far);
JNK_API mat4 mat4_column_ortho(f32 left, f32 right, f32 bottom, f32 top,
                               f32 near, f32 far);
JNK_API mat4 mat4_row_perspective(f32 fov_rad, f32 aspect_ratio, f32 near,
                                  f32 far);
JNK_API mat4 mat4_column_perspective(f32 fov_rad, f32 aspect_ratio, f32 near,
                                     f32 far);

JNK_API mat4 mat4_row_lookat(vec3 pos, vec3 target, vec3 up);
JNK_API mat4 mat4_column_lookat(vec3 pos, vec3 target, vec3 up);

JNK_API mat4 mat4_transpose(mat4 matrix);
JNK_API mat4 mat4_inverse_rigid(mat4 matrix);
JNK_API mat4 mat4_inverse(mat4 matrix);

// Quaternion
quat quat_identity(void);
quat quat_get_normalized(quat q);
quat quat_conjugate(quat q);
quat quat_inverse(quat q);

f32 quat_normalized(quat q);
f32 quat_dot(quat q1, quat q2);

quat quat_multi(quat q1, quat q2);
quat quat_from_axis_angle(vec3 axis, f32 angle, b8 normalize);
quat quat_slerp(quat q_0, quat q_1, f32 percentage);

mat4 quat_to_rotation_matrix(quat q, vec3 center);

JNK_EXTERN_C_END
#endif // JUNK_MATH_H
