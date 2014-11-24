#include "hw0.h"


HW0::HW0(char const *vs_name, char const *fs_name):
    sample_t(vs_name, fs_name),
    camera_dist_coef_inc_(0.05f),
    camera_dist_coef_(1.0f),
    angle_inc_(0.01f),
    y_angle_(0),
    x_angle_(0)
{
}


void HW0::zoom_in()
{
    camera_dist_coef_ = std::max(near_,
                                 camera_dist_coef_ - camera_dist_coef_inc_);
}


void HW0::zoom_out()
{
    camera_dist_coef_ = std::min(far_,
                                 camera_dist_coef_ + camera_dist_coef_inc_);
}


void HW0::rotate_camera(int dx, int dy)
{
    x_angle_ += angle_inc_ * dy;
    y_angle_ += angle_inc_ * dx;
}


vector<vec2> const HW0::drawing_data() const
{
    return vector<vec2>{
        vec2(-1, -1),
        vec2(1, -1),
        vec2(1, 1)
    };
}


mat4 const HW0::view_matrix() const
{
    vec3 eye(rotateX(vec3(0, 0, 8 * camera_dist_coef_), x_angle_));
    vec3 up(0, 1, 0);
    if (eye.z < 0) up *= -1;
    eye = rotateY(eye, y_angle_);
    return lookAt(eye, vec3(0, 0, 0), up);
}
