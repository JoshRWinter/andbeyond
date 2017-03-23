#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android_native_app_glue.h>
#include <math.h>

#include <vector>

#include "defs.h"

bool too_close(const base_s &base,const std::vector<saw_s*> &saw_list,const std::vector<electro_s*> &electro_list,const std::vector<smasher_s*> &smasher_list){
	const float tolerance=-5.0f;

	for(std::vector<saw_s*>::const_iterator iter=saw_list.begin();iter!=saw_list.end();++iter){
		if(base.collide_y(**iter,tolerance)){
			logcat("this.y==%.2f, base.y==%.2f",base.y,(*iter)->y);
			return true;
		}
	}

	for(std::vector<electro_s*>::const_iterator iter=electro_list.begin();iter!=electro_list.end();++iter){
		if(base.collide_y(**iter,tolerance))
			return true;
	}

	for(std::vector<smasher_s*>::const_iterator iter=smasher_list.begin();iter!=smasher_list.end();++iter){
		if(base.collide_y((*iter)->left,tolerance))
			return true;
	}

	return false;
}

bool base_s::collide(const base_s &b,float tolerance)const{
	return x+w>b.x+tolerance&&x<b.x+b.w-tolerance&&y+h>b.y+tolerance&&y<b.y+b.h-tolerance;
}

bool base_s::collide_y(const base_s &b,float tolerance)const{
	return y+h>b.y+tolerance&&y<b.y+b.h-tolerance;
}

int base_s::correct(const base_s &b){
	if(!this->collide(b,0.0f))
		return 0;

	// this object collides with the <left|right|bottom|top> side of <b>
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
		y=b.y+b.h;
		return COLLIDE_BOTTOM;
	}
}

void renderer_s::draw(const base_s &base,bool xflip)const{
	const float size=1.0f/base.count;
	const float left=size*base.frame;
	const float right=left+size;

	if(xflip)
		glUniform4f(uniform.texcoords,right,left,0.0f,1.0f);
	else
		glUniform4f(uniform.texcoords,left,right,0.0f,1.0f);
	glUniform2f(uniform.vector,base.x,base.y);
	glUniform2f(uniform.size,base.w,base.h);
	glUniform1f(uniform.rot,base.rot);

	glDrawArrays(GL_TRIANGLE_STRIP,0,4);
}
