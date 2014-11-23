#ifndef SAMPLE_H
#define SAMPLE_H

#include "common.h"


class sample_t
{
public:
   sample_t(char const *vs_name, char const *fs_name);
   virtual ~sample_t();

   void init_buffer();
   void draw_frame( float time_from_start );

protected:
   virtual vector<vec2> const drawing_data() const;
   virtual float camera_dist() const;
   virtual mat4 const additional_transform() const;

   float const near_;
   float const far_;

private:
   bool wireframe_;
   GLuint vs_, fs_, program_;
   GLuint vx_buf_;
   quat   rotation_by_control_;
};


#endif // SAMPLE_H
