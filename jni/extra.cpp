#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android_native_app_glue.h>
#include <math.h>

#include "defs.h"

bool base_s::collide(const base_s &b,float tolerance){
	return x+w>b.x+tolerance&&x<b.x+b.w-tolerance&&y+h>b.y+tolerance&&y<b.y+b.h-tolerance;
}

int base_s::correct(const base_s &b){
	if(!this->collide(b,0.0f))
		return 0;

	// this object collides with the <left|right|bottom|top> side
	float left,right,bottom,top;
	left=fabs((x+w)-b.x);
	right=fabs(x-(b.x+b.w));
	bottom=fabs(y-(b.y+b.h));
	top=fabs((y+h)-b.y);

	float smallest=left;
	if(right<smallest)
		smallest=right;
	if(bottom<smallest)
		smallest=bottom;
	if(top<smallest)
		smallest=top;

	if(smallest==top){
		y=b.y-h;
		return COLLIDE_TOP;
	}
	else if(smallest==right){
		x=b.x+b.w;
		return COLLIDE_RIGHT;
	}
	else if(smallest==left){
		x=b.x-w;
		return COLLIDE_LEFT;
	}
	else{
		y=b.y+h;
		return COLLIDE_BOTTOM;
	}
}

void renderer_s::draw(const base_s &base,bool xflip){
	glUniform4f(uniform.texcoords,xflip,!xflip,0.0f,1.0f);
	glUniform2f(uniform.vector,base.x,base.y);
	glUniform2f(uniform.size,base.w,base.h);
	glUniform1f(uniform.rot,base.rot);

	glDrawArrays(GL_TRIANGLE_STRIP,0,4);
}
