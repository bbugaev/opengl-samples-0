#include "hw.h"
#include "filenames.h"
#include "callbacks.h"
#include "shader.h"
#include "texture_loader.h"
#include "tiny_obj_loader.h"


HW::HW(char const *model_name):
    near_(0.1f),
    far_(100.0f),
    wireframe_(false),
    tex_coef_(1),
    camera_dist_coef_inc_(0.05f),
    camera_dist_coef_(1.0f),
    angle_inc_(0.01f),
    y_angle_(0),
    x_angle_(0)
{
    init_tw();
    init_shaders();
    init_texture(TEX_FILE);
    init_model(model_name);
    init_vao();
}


HW::~HW()
{
    glDeleteProgram(program_);
    glDeleteShader(vs_);
    glDeleteShader(fs_);

    glDeleteProgram(wireframe_program_);
    glDeleteShader(wireframe_vs_);
    glDeleteShader(wireframe_fs_);

    glDeleteTextures(1, &tex_);

    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &positions_);
    glDeleteBuffers(1, &indices_);
    glDeleteBuffers(1, &normals_);

    TwDeleteAllBars();
    TwTerminate();
}


void HW::draw_frame()
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

    draw_model(mvp);
    if (wireframe_) draw_wireframe(mvp);
}


void HW::zoom_in()
{
    camera_dist_coef_ = std::max(near_,
                                 camera_dist_coef_ - camera_dist_coef_inc_);
}


void HW::zoom_out()
{
    camera_dist_coef_ = std::min(far_,
                                 camera_dist_coef_ + camera_dist_coef_inc_);
}


void HW::rotate_camera(int dx, int dy)
{
    x_angle_ += angle_inc_ * dy;
    y_angle_ += angle_inc_ * dx;
}


void HW::draw_model(mat4 const &mvp)
{
    glUseProgram(program_);

    GLuint const mvp_attr = glGetUniformLocation(program_, "mvp");
    glUniformMatrix4fv(mvp_attr, 1, GL_FALSE, &mvp[0][0]);

    GLuint const coef_attr = glGetUniformLocation(program_, "coef_st");
    glUniform1f(coef_attr, tex_coef_);

    glBindTexture(GL_TEXTURE_2D, tex_);
    GLuint const tex_attr = glGetUniformLocation(program_, "tex");
    glUniform1i(tex_attr, 0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_);

    glDrawElements(GL_TRIANGLES, size_, GL_UNSIGNED_INT, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}


void HW::draw_wireframe(mat4 const &mvp)
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


void HW::init_tw()
{
    TwInit(TW_OPENGL, 0);

    TwBar *bar = TwNewBar("Parameters");
    TwDefine("Parameters size='500 170' color='70 100 120' valueswidth=220 "
             "iconpos=topleft");

    TwAddVarRW(bar, "Wireframe mode", TW_TYPE_BOOLCPP, &wireframe_,
               "true='ON' false='OFF' key=W");

    TwAddButton(bar, "Fullscreen toggle", toggle_fullscreen_callback, 0,
                "label='Toggle fullscreen mode' key=F");

    TwAddVarRW(bar, "Texture coef", TW_TYPE_FLOAT, &tex_coef_,
               "min=0.1 max=10 step=0.1 keyincr=+ keydecr=-");

    TwAddVarRW(bar, "ObjRotation", TW_TYPE_QUAT4F, &rotation_by_control_,
               "label='Object orientation' opened=true "
               "help='Change the object orientation.' ");
}


void HW::init_shaders()
{
    vs_ = create_shader(GL_VERTEX_SHADER, MAIN_VS_FILE);
    fs_ = create_shader(GL_FRAGMENT_SHADER, MAIN_FS_FILE);
    program_ = create_program(vs_, fs_);

    wireframe_vs_ = create_shader(GL_VERTEX_SHADER, WIREFRAME_VS_FILE);
    wireframe_fs_ = create_shader(GL_FRAGMENT_SHADER, WIREFRAME_FS_FILE);
    wireframe_program_ = create_program(wireframe_vs_, wireframe_fs_);
}


void HW::init_texture(char const *tex_name)
{
    TextureLoader tl(tex_name);
    if (!tl.bits()) return;

    glGenTextures(1, &tex_);
    glBindTexture(GL_TEXTURE_2D, tex_);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, tl.width(), tl.height(), 0,
                 GL_BGR, GL_UNSIGNED_BYTE, tl.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
}


void HW::init_model(char const *model_name)
{
    using namespace tinyobj;
    vector<shape_t> shapes;
    vector<material_t> materials;
    LoadObj(shapes, materials, model_name);
    mesh_t const &mesh = shapes.front().mesh;

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

    glGenBuffers(1, &texcoords_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, texcoords_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(float) * mesh.texcoords.size(), mesh.texcoords.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void HW::init_vao()
{
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, positions_);
    GLuint pos_attr = glGetAttribLocation(program_, "in_pos");
    glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(pos_attr);

    pos_attr = glGetAttribLocation(wireframe_program_, "in_pos");
    glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(pos_attr);

    glBindBuffer(GL_ARRAY_BUFFER, normals_);
    GLuint const normal_attr = glGetAttribLocation(program_, "normal");
    glVertexAttribPointer(normal_attr, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(normal_attr);

    glBindBuffer(GL_ARRAY_BUFFER, texcoords_);
    GLuint const texcoords_attr = glGetAttribLocation(program_, "in_st");
    glVertexAttribPointer(texcoords_attr, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(texcoords_attr);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
