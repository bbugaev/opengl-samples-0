#ifndef HW_H
#define HW_H

#include "common.h"


class HW
{
public:
    HW(char const *model_name);
    ~HW();

    void draw_frame(float time_from_start);

    void zoom_in();
    void zoom_out();
    void rotate_camera(int dx, int dy);

private:
    void draw_model(float time_from_start, mat4 const &model,
                    mat4 const &mvp);
    void draw_wireframe(mat4 const &mvp);

    void init_tw();
    void init_shaders();
    void init_texture(char const *tex_name);
    void init_constants(vector<float> const &positions);
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

    float v_;
    float k_;
    float max_dist_;
    vec3 center_;

    size_t size_;
    GLuint positions_;
    GLuint indices_;
    GLuint normals_;
    GLuint texcoords_;
    GLuint vao_;
};


#endif // HW_H
