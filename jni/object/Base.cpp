#include "../andbeyond.h"

bool Base::collide(const Base &b,float tolerance)const{
	return x+w>b.x+tolerance&&x<b.x+b.w-tolerance&&y+h>b.y+tolerance&&y<b.y+b.h-tolerance;
}

bool Base::collide_y(const Base &b,float tolerance)const{
	return y+h>b.y+tolerance&&y<b.y+b.h-tolerance;
}

void Base::background(const Renderer &renderer){
	x=renderer.rect.left;
	y=renderer.rect.top;
	w=renderer.rect.right*2.0f;
	h=renderer.rect.bottom*2.0f;
	rot=0.0f;
	frame=0;
	count=1;
}

bool Base::touching(const crosshair *pointer)const{
	return pointer[0].x>x&&pointer[0].x<x+w&&pointer[0].y>y&&pointer[0].y<y+h;
}

int Base::correct(const Base &b){
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

bool Base::too_close(const std::vector<Saw*> &saw_list,const std::vector<Electro*> &electro_list,const std::vector<Smasher*> &smasher_list){
	const float tolerance=-5.0f;

	for(const Saw *saw:saw_list){
		if(collide_y(*saw,tolerance)){
			return true;
		}
	}

	for(const Electro *electro:electro_list){
		if(collide_y(*electro,tolerance))
			return true;
	}

	for(const Smasher *smasher:smasher_list){
		if(collide_y(smasher->left,tolerance))
			return true;
	}

	return false;
}
