#include "sample.h"
#include "callbacks.h"
#include "shader.h"


sample_t::sample_t(char const *vs_name, char const *fs_name)
   : wireframe_(false)
   , near_(0.1f)
   , far_(100.0f)
{
   TwInit(TW_OPENGL, NULL);

   // Определение "контролов" GUI
   TwBar *bar = TwNewBar("Parameters");
   TwDefine(" Parameters size='500 150' color='70 100 120' valueswidth=220 iconpos=topleft");

   TwAddVarRW(bar, "Wireframe mode", TW_TYPE_BOOLCPP, &wireframe_, " true='ON' false='OFF' key=W");

   TwAddButton(bar, "Fullscreen toggle", toggle_fullscreen_callback, NULL,
               " label='Toggle fullscreen mode' key=F");

   TwAddVarRW(bar, "ObjRotation", TW_TYPE_QUAT4F, &rotation_by_control_,
              " label='Object orientation' opened=true help='Change the object orientation.' ");

   // Создание шейдеров
   vs_ = create_shader(GL_VERTEX_SHADER  , vs_name);
   fs_ = create_shader(GL_FRAGMENT_SHADER, fs_name);
   // Создание программы путём линковки шейдерова
   program_ = create_program(vs_, fs_);
}


sample_t::~sample_t()
{
   // Удаление русурсов OpenGL
   glDeleteProgram(program_);
   glDeleteShader(vs_);
   glDeleteShader(fs_);
   glDeleteBuffers(1, &vx_buf_);

   TwDeleteAllBars();
   TwTerminate();
}


void sample_t::init_buffer()
{
   // Создание пустого буфера
   glGenBuffers(1, &vx_buf_);
   // Делаем буфер активным
   glBindBuffer(GL_ARRAY_BUFFER, vx_buf_);

   // Данные для визуализации
   vector<vec2> const data(drawing_data());

   // Копируем данные для текущего буфера на GPU
   glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * data.size(), data.data(),
                GL_STATIC_DRAW);

   // Сбрасываем текущий активный буфер
   glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void sample_t::draw_frame( float time_from_start )
{
    //return;
   float const rotation_angle = time_from_start * 90;

   float const w                = (float)glutGet(GLUT_WINDOW_WIDTH);
   float const h                = (float)glutGet(GLUT_WINDOW_HEIGHT);
   // строим матрицу проекции с aspect ratio (отношением сторон) таким же, как у окна
   mat4  const proj             = perspective(45.0f, w / h, near_, far_);
   // преобразование из СК мира в СК камеры
   mat4  const view             = lookAt(vec3(0, 0, camera_dist()), vec3(0, 0, 0), vec3(0, 1, 0));

   // анимация по времени
   quat  const rotation_by_time = quat(vec3(radians(rotation_angle), 0, 0));
   mat4  const modelview        = view * additional_transform() * mat4_cast(rotation_by_control_ * rotation_by_time);
   mat4  const mvp              = proj * modelview;

   // выбор режима растеризации - только рёбра или рёбра + грани
   if (wireframe_)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   else
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

   // выключаем отсечение невидимых поверхностей
   glDisable(GL_CULL_FACE);
   // выключаем тест глубины
   glDisable(GL_DEPTH_TEST);
   // очистка буфера кадра
   glClearColor(0.2f, 0.2f, 0.2f, 1);
   glClearDepth(1);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // установка шейдеров для рисования
   glUseProgram(program_);

   // установка uniform'ов
   GLuint const mvp_location = glGetUniformLocation(program_, "mvp");
   glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);

   GLuint const time_location = glGetUniformLocation(program_, "time");
   glUniform1f(time_location, time_from_start);

   glBindBuffer(GL_ARRAY_BUFFER, vx_buf_);

   // запрашиваем индек аттрибута у программы, созданные по входным шейдерам
   GLuint const pos_location = glGetAttribLocation(program_, "in_pos");
   // устанавливаем формам данных для аттрибута "pos_location"
   // 2 float'а ненормализованных, шаг между вершиными равен sizeof(vec2), смещение от начала буфера равно 0
   glVertexAttribPointer(pos_location, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), 0);
   // "включаем" аттрибут "pos_location"
   glEnableVertexAttribArray(pos_location);

      // отрисовка
   glDrawArrays(GL_TRIANGLES, 0, 3);

   glDisableVertexAttribArray(pos_location);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
}


vector<vec2> const sample_t::drawing_data() const
{
   return vector<vec2>{
        vec2(0, 0)
      , vec2(1, 0)
      , vec2(1, 1)
   };
}


float sample_t::camera_dist() const
{
    return 8;
}


mat4 const sample_t::additional_transform() const
{
    return mat4();
}
