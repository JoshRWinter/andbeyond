#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android_native_app_glue.h>

#include "defs.h"

void renderer_s::draw(base_s &base){
	glUniform4f(uniform.texcoords,0.0f,1.0f,0.0f,1.0f);
	glUniform2f(uniform.vector,base.x,base.y);
	glUniform2f(uniform.size,base.w,base.h);
	glUniform1f(uniform.rot,base.rot);
	
	glDrawArrays(GL_TRIANGLE_STRIP,0,4);
}
