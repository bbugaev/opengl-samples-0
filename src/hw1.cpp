#include "hw1.h"
#include "filenames.h"
#include "callbacks.h"
#include "shader.h"
#include "tiny_obj_loader.h"


HW1::HW1(char const *vs_name, char const *fs_name):
    near_(0.1f),
    far_(100.0f),
    wireframe_(false),
    camera_dist_coef_inc_(0.05f),
    camera_dist_coef_(1.0f),
    angle_inc_(0.01f),
    y_angle_(0),
    x_angle_(0)
{
    init_tw();
    init_shaders(vs_name, fs_name);
    init_model();
    init_vao();
}


HW1::~HW1()
{
    glDeleteProgram(program_);
    glDeleteShader(vs_);
    glDeleteShader(fs_);

    glDeleteProgram(wireframe_program_);
    glDeleteShader(wireframe_vs_);
    glDeleteShader(wireframe_fs_);

    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &positions_);
    glDeleteBuffers(1, &indices_);
    glDeleteBuffers(1, &normals_);

    TwDeleteAllBars();
    TwTerminate();
}


void HW1::draw_frame(float time_from_start)
{
    float const w = static_cast<float>(glutGet(GLUT_WINDOW_WIDTH));
    float const h = static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT));


    vec3 eye(rotateX(vec3(0, 0, 16 * camera_dist_coef_), x_angle_));
    vec3 up(0, 1, 0);
    if (eye.z < 0) up *= -1;
    eye = rotateY(eye, y_angle_);

    mat4 const proj(perspective(45.0f, w / h, near_, far_));
    mat4 const view(lookAt(eye, vec3(0, 0, 0), up));

    mat4 const model(mat4_cast(rotation_by_control_));
    mat4 const mvp(proj * view * model);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.2f, 0.2f, 1);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw_model(time_from_start, model, mvp);
    if (wireframe_) draw_wireframe(mvp);
}


void HW1::zoom_in()
{
    camera_dist_coef_ = std::max(near_,
                                 camera_dist_coef_ - camera_dist_coef_inc_);
}


void HW1::zoom_out()
{
    camera_dist_coef_ = std::min(far_,
                                 camera_dist_coef_ + camera_dist_coef_inc_);
}


void HW1::rotate_camera(int dx, int dy)
{
    x_angle_ += angle_inc_ * dy;
    y_angle_ += angle_inc_ * dx;
}


void HW1::draw_model(float time_from_start, mat4 const &model,
                     mat4 const &mvp)
{
    glUseProgram(program_);

    GLuint const mvp_attr = glGetUniformLocation(program_, "mvp");
    glUniformMatrix4fv(mvp_attr, 1, GL_FALSE, &mvp[0][0]);
    GLuint const model_attr = glGetUniformLocation(program_, "model");
    glUniformMatrix4fv(model_attr, 1, GL_FALSE, &model[0][0]);
    GLuint const time_attr = glGetUniformLocation(program_, "time");
    glUniform1f(time_attr, time_from_start);

    GLuint const v_attr = glGetUniformLocation(program_, "v");
    glUniform1f(v_attr, v_);
    GLuint const k_attr = glGetUniformLocation(program_, "k");
    glUniform1f(k_attr, k_);
    GLuint const max_dist_attr = glGetUniformLocation(program_, "max_dist");
    glUniform1f(max_dist_attr, max_dist_);
    GLuint const center_attr = glGetUniformLocation(program_, "center");
    glUniform3fv(center_attr, 1, &center_[0]);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_);

    glDrawElements(GL_TRIANGLES, size_, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void HW1::draw_wireframe(mat4 const &mvp)
{
    glUseProgram(wireframe_program_);

    GLuint const mvp_attr = glGetUniformLocation(wireframe_program_, "mvp");
    glUniformMatrix4fv(mvp_attr, 1, GL_FALSE, &mvp[0][0]);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-0.5f, 0.0f);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_);

    glDrawElements(GL_TRIANGLES, size_, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisable(GL_POLYGON_OFFSET_LINE);
}


void HW1::init_tw()
{
    TwInit(TW_OPENGL, 0);

    TwBar *bar = TwNewBar("Parameters");
    TwDefine("Parameters size='500 180' color='70 100 120' valueswidth=220 "
             "iconpos=topleft");

    TwAddVarRW(bar, "Wireframe mode", TW_TYPE_BOOLCPP, &wireframe_,
               "true='ON' false='OFF' key=W");

    TwAddButton(bar, "Fullscreen toggle", toggle_fullscreen_callback, 0,
                "label='Toggle fullscreen mode' key=F");

    TwAddVarRW(bar, "v", TW_TYPE_FLOAT, &v_, "min=0 max=10 step=0.1");
    TwAddVarRW(bar, "k", TW_TYPE_FLOAT, &k_, "min=0 max=10 step=0.1");

    TwAddVarRW(bar, "ObjRotation", TW_TYPE_QUAT4F, &rotation_by_control_,
               "label='Object orientation' opened=true "
               "help='Change the object orientation.' ");
}


void HW1::init_shaders(char const *vs_name, char const *fs_name)
{
    vs_ = create_shader(GL_VERTEX_SHADER  , vs_name);
    fs_ = create_shader(GL_FRAGMENT_SHADER, fs_name);
    program_ = create_program(vs_, fs_);

    wireframe_vs_ = create_shader(GL_VERTEX_SHADER, WIREFRAME_VS_FILE);
    wireframe_fs_ = create_shader(GL_FRAGMENT_SHADER, WIREFRAME_FS_FILE);
    wireframe_program_ = create_program(wireframe_vs_, wireframe_fs_);
}


void HW1::init_constants(vector<float> const &positions)
{
    center_ = vec3(0);
    for (size_t i = 0; i < positions.size() / 3; ++i)
    {
        vec3 const v(positions[3 * i], positions[3 * i + 1],
                     positions[3 * i + 2]);
        center_ += v;
    }
    center_ /= positions.size() / 3;

    max_dist_ = 0;
    for (size_t i = 0; i < positions.size() / 3; ++i)
    {
        vec3 const v(positions[3 * i], positions[3 * i + 1],
                     positions[3 * i + 2]);
        max_dist_ = std::max(max_dist_, length(v - center_));
    }
}


void HW1::init_model()
{
    using namespace tinyobj;
    vector<shape_t> shapes;
    vector<material_t> materials;
    LoadObj(shapes, materials, MODEL_FILE);
    mesh_t const &mesh = shapes.front().mesh;

    init_constants(mesh.positions);

    size_ = mesh.indices.size();

    glGenBuffers(1, &positions_);
    glBindBuffer(GL_ARRAY_BUFFER, positions_);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * mesh.positions.size(), mesh.positions.data(),
                 GL_STATIC_DRAW);

    glGenBuffers(1, &indices_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(float) * mesh.indices.size(), mesh.indices.data(),
                 GL_STATIC_DRAW);

    glGenBuffers(1, &normals_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, normals_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(float) * mesh.normals.size(), mesh.normals.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void HW1::init_vao()
{
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, positions_);
    GLuint pos_attr = glGetAttribLocation(program_, "position");
    glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(pos_attr);

    pos_attr = glGetAttribLocation(wireframe_program_, "position");
    glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(pos_attr);

    glBindBuffer(GL_ARRAY_BUFFER, normals_);
    GLuint const normal_attr = glGetAttribLocation(program_, "normal");
    glVertexAttribPointer(normal_attr, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(normal_attr);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
