#ifndef HW0_H
#define HW0_H

#include "sample.h"


class HW0: public sample_t
{
public:
    HW0(char const *vs_name, char const *fs_name);

    void zoom_in();
    void zoom_out();
    void rotate_camera(int dx, int dy);

protected:
    vector<vec2> const drawing_data() const;
    float camera_dist() const;
    mat4 const additional_transform() const;

private:
    float const camera_dist_coef_inc_;
    float camera_dist_coef_;

    float const angle_inc_;
    float y_angle_;
    float x_angle_;
};


#endif // HW0_H
