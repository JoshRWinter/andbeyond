#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android_native_app_glue.h>

#include "defs.h"

bool base_s::collide(const base_s &b,float tolerance){
	return x+w>b.x+tolerance&&x<b.x+b.w-tolerance&&y+h>b.y+tolerance&&y<b.y+b.h-tolerance;
}

void renderer_s::draw(const base_s &base,bool xflip){
	glUniform4f(uniform.texcoords,xflip,!xflip,0.0f,1.0f);
	glUniform2f(uniform.vector,base.x,base.y);
	glUniform2f(uniform.size,base.w,base.h);
	glUniform1f(uniform.rot,base.rot);

	glDrawArrays(GL_TRIANGLE_STRIP,0,4);
}
