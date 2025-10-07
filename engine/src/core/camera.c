#include "junk/jnk_camera.h"
#include "junk/jnk_math.h"
#include "junk/jnk_log.h"

typedef struct {
    jnk_camera_t c;
} camera_state_t;

static camera_state_t *g_camera = JNK_NULL;

b8 camera_system_init(u64 *memory_req, void *state) {
    if (memory_req) *memory_req = sizeof(camera_state_t);
    if (!state) return true;
    g_camera = (camera_state_t *)state;

    jnk_log_info(CH_CORE, "Camera system initialize");
    return true;
}

void camera_system_kill(void *state) {
    if (!state) return;
    jnk_log_info(CH_CORE, "Camera system kill");
}

jnk_camera_t *jnk_camera_get_active(void) { return &g_camera->c; }

void jnk_camera_create(f32 fov_deg, f32 aspect_ratio, f32 near_clip,
                       f32 far_clip) {
    g_camera->c.fov = fov_deg;
    g_camera->c.aspect_ratio = aspect_ratio;
    g_camera->c.near_clip = near_clip;
    g_camera->c.far_clip = far_clip;

    g_camera->c.position = vec3_zero();
    g_camera->c.rotation = vec3_zero();

    g_camera->c.projection =
        mat4_column_perspective(fov_deg, aspect_ratio, near_clip, far_clip);
    g_camera->c.view = mat4_identity();
    g_camera->c.viewproj =
        mat4_column_multi(g_camera->c.projection, g_camera->c.view);
}

void jnk_camera_yaw(jnk_camera_t *cam, f32 amount) {
    cam->rotation.comp1.y += amount;
    cam->dirty = true;
}

void jnk_camera_pitch(jnk_camera_t *cam, f32 amount) {
    cam->rotation.comp1.x += amount;
    f32 limit = deg_to_rad(89.0f);
    cam->rotation.comp1.x = JNK_CLAMP(cam->rotation.comp1.x, -limit, limit);
    cam->dirty = true;
}

void jnk_camera_set_position(jnk_camera_t *camera, vec3 pos) {
    camera->position = pos;
}

void jnk_camera_set_rotation(jnk_camera_t *camera, vec3 rot) {
    camera->rotation = rot;
}

void jnk_camera_update(jnk_camera_t *camera) {
    mat4 t = mat4_translate(camera->position);
    mat4 r = mat4_euler_xyz(camera->rotation.comp1.x, camera->rotation.comp1.y,
                            camera->rotation.comp1.z);

    camera->view = mat4_column_multi(r, t);
    camera->viewproj = mat4_column_multi(camera->projection, camera->view);
}
