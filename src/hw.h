#ifndef HW_H
#define HW_H

#include "common.h"


class HW
{
public:
    HW(char const *model_name);
    ~HW();

    void draw_frame();

    void zoom_in();
    void zoom_out();
    void rotate_camera(int dx, int dy);

private:
    void draw_model(mat4 const &mvp);
    void draw_wireframe(mat4 const &mvp);

    void init_tw();
    void init_shaders();
    void init_texture(char const *tex_name);
    void init_model(char const *model_name);
    void init_vao();

private:
    GLuint vs_;
    GLuint fs_;
    GLuint program_;

    GLuint wireframe_vs_;
    GLuint wireframe_fs_;
    GLuint wireframe_program_;

    GLuint tex_;

    float const near_;
    float const far_;

    bool wireframe_;
    quat rotation_by_control_;

    float const camera_dist_coef_inc_;
    float camera_dist_coef_;

    float const angle_inc_;
    float y_angle_;
    float x_angle_;

    size_t size_;
    GLuint positions_;
    GLuint indices_;
    GLuint normals_;
    GLuint texcoords_;
    GLuint vao_;
};


#endif // HW_H
