#ifndef CALLBACKS_H
#define CALLBACKS_H

#include "common.h"


inline void TW_CALL toggle_fullscreen_callback( void * )
{
   glutFullScreenToggle();
}


// Переисовка кадра в отсутствии других сообщений
inline void idle_func()
{
   glutPostRedisplay();
}


// Отработка изменения размеров окна
inline void reshape_func( int width, int height )
{
   if (width <= 0 || height <= 0)
      return;
   glViewport(0, 0, width, height);
   TwWindowSize(width, height);
}


#ifndef APIENTRY
   #define APIENTRY
#endif

// callback на различные сообщения от OpenGL
inline void APIENTRY gl_debug_proc(  GLenum         //source
                                   , GLenum         type
                                   , GLuint         //id
                                   , GLenum         //severity
                                   , GLsizei        //length
                                   , GLchar const * message
                                   
                                   , GLvoid * //user_param
                                     )
{
   if (type == GL_DEBUG_TYPE_ERROR_ARB)
   {
      cerr << message << endl;
      exit(1);
   }
}


#endif // CALLBACKS_H
