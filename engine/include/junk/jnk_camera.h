#ifndef JUNK_CAMERA_H
#define JUNK_CAMERA_H

#include "jnk_define.h"
#include "jnk_math_type.h"

JNK_EXTERN_C_BEGIN

typedef struct jnk_camera_t {
    vec3 position;
    vec3 rotation;

    f32 fov;
    f32 near_clip;
    f32 far_clip;

    f32 aspect_ratio;

    mat4 view;
    mat4 projection;

    b8 dirty;

    mat4 viewproj;
} jnk_camera_t;

b8 camera_system_init(u64 *memory_req, void *state);
void camera_system_kill(void *state);

JNK_API jnk_camera_t *jnk_camera_get_active(void);

JNK_API void jnk_camera_create(f32 fov_deg, f32 aspect_ratio, f32 near_clip,
                               f32 far_clip);

JNK_API void jnk_camera_yaw(jnk_camera_t *cam, f32 amount);
JNK_API void jnk_camera_pitch(jnk_camera_t *cam, f32 amount);

// JNK_API void jnk_camera_set_position(jnk_camera_t *camera, vec3 pos);
// JNK_API void jnk_camera_set_rotation(jnk_camera_t *camera, vec3 rot);
JNK_API void jnk_camera_update(jnk_camera_t *camera);

JNK_EXTERN_C_END
#endif // JUNK_CAMERA_H
