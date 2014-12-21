#include "ubuntu_fix.h"
#include "common.h"
#include "hw.h"
#include "callbacks.h"


unique_ptr<HW> hw;


// отрисовка кадра
void display_func()
{
   static chrono::system_clock::time_point const start = chrono::system_clock::now();

   // вызов функции отрисовки с передачей ей времени от первого вызова
   hw->draw_frame(chrono::duration<float>(chrono::system_clock::now() - start).count());

   // отрисовка GUI
   TwDraw();

   // смена front и back buffer'а (напоминаю, что у нас используется режим двойной буферизации)
   glutSwapBuffers();
}


// Очищаем все ресурсы, пока контекст ещё не удалён
void close_func()
{
   hw.reset();
}


void process_key_press(unsigned char button, int x, int y)
{
   if (TwEventKeyboardGLUT(button, x, y))
      return;

   switch(button)
   {
   case 'w':
       hw->zoom_in();
       break;
   case 's':
       hw->zoom_out();
       break;
   case 27:
      // hw.reset();
      exit(0);
   }
}

struct
{
    int x;
    int y;
}
mouse_position;

void process_mouse_press(int button, int state, int x, int y)
{
    if (TwEventMouseButtonGLUT(button, state, x, y))
        return;

    if (button == 3 && state == GLUT_UP) {
        hw->zoom_in();
    } else if (button == 4 && state == GLUT_UP) {
        hw->zoom_out();
    } else {
        mouse_position.x = x;
        mouse_position.y = y;
    }
}

void process_mouse_motion(int x, int y)
{
    if (TwEventMouseMotionGLUT(x, y))
        return;

    hw->rotate_camera(x - mouse_position.x, y - mouse_position.y);
    mouse_position.x = x;
    mouse_position.y = y;
}


int main( int argc, char ** argv )
{
   if (argc != 2)
   {
      cerr << "Usage: " << argv[0]  << " (3 | 4)" << endl;
      return 0;
   }

   // Размеры окна по-умолчанию
   size_t const default_width  = 800;
   size_t const default_height = 800;

   glutInit               (&argc, argv);
   glutInitWindowSize     (default_width, default_height);
   // Указание формата буфера экрана:
   // - GLUT_DOUBLE - двойная буферизация
   // - GLUT_RGB - 3-ёх компонентный цвет
   // - GLUT_DEPTH - будет использоваться буфер глубины
   glutInitDisplayMode    (GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
   // Создаем контекст версии 3.2
   glutInitContextVersion (3, 0);
   // Контекст будет поддерживать отладку и "устаревшую" функциональность, которой, например, может пользоваться библиотека AntTweakBar
  // glutInitContextFlags   (GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
   // Указание либо на core либо на compatibility профил
   //glutInitContextProfile (GLUT_COMPATIBILITY_PROFILE );
   glutCreateWindow("HW");

   // Инициализация указателей на функции OpenGL
   if (glewInit() != GLEW_OK)
   {
      cerr << "GLEW init failed" << endl;
      return 1;
   }

   // Проверка созданности контекста той версии, какой мы запрашивали
   if (!GLEW_VERSION_3_0)
   {
      cerr << "OpenGL 3.0 not supported" << endl;
      return 1;
   }

   // подписываемся на оконные события
   glutReshapeFunc(reshape_func);
   glutDisplayFunc(display_func);
   glutIdleFunc   (idle_func   );
   glutCloseFunc  (close_func  );

   glutKeyboardFunc(process_key_press);
   glutMouseFunc(process_mouse_press);
   glutMotionFunc(process_mouse_motion);

   // подписываемся на события для AntTweakBar'а
   glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
   glutSpecialFunc      ((GLUTspecialfun    )TwEventSpecialGLUT    );
   TwGLUTModifiersFunc  (glutGetModifiers);

   try
   {
      // Создание класса-примера
      string const vs_name("shaders//hw1_" + string(argv[1]) + ".glslvs");
      string const fs_name("shaders//hw1_" + string(argv[1]) + ".glslfs");
      hw.reset(new HW(vs_name.c_str(), fs_name.c_str()));
      // Вход в главный цикл приложения
      glutMainLoop();
   }
   catch( std::exception const & except )
   {
      std::cout << except.what() << endl;
      return 1;
   }

   return 0;
}
