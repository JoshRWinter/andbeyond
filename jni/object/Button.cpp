#include "../andbeyond.h"

void Button::init(float x,float y,const char *label){
	this->x=x;
	this->y=y;
	w=BUTTON_WIDTH;
	h=BUTTON_HEIGHT;
	rot=0.0f;
	count=1;
	frame=0;

	active=false;
	this->label=label;
}

bool Button::process(State &state){
	if(touching(state.pointer)){
		if(!state.pointer[0].active){
			if(active){
				active=false;
				return true;
			}
		}
		else
			active=true;
	}
	else
		active=false;

	return false;
}

void Button::render(const Renderer &renderer,float yoffset)const{
	Button copy=*this;

	if(active)
		copy.y+=BUTTON_PUSH_DOWN;

	renderer.draw(copy,yoffset);
}

void Button::render_text(const Renderer &renderer,const ftfont *font,float yoffset)const{
	float ypos=y+(BUTTON_HEIGHT/2.0f)-(font->fontsize/2.0f)+0.1f;

	if(active)
		ypos+=BUTTON_PUSH_DOWN;

	drawtextcentered(font,x+(BUTTON_WIDTH/2.0f),ypos+yoffset,label);
}
