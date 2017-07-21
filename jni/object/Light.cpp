#include "../andbeyond.h"

Light::Light(const State &state){
	w=LIGHT_WIDTH;
	h=LIGHT_HEIGHT;
	x=randomint(state.renderer.rect.left*10.0f,(state.renderer.rect.right-LIGHT_WIDTH)*10.0f)/10.0f;
	y=state.renderer.rect.top-LIGHT_HEIGHT;
	rot=0.0f;
	texture=AID_LIGHT_1+randomint(0,3);

	yv=0.6f;
	choose_color();
}

void Light::process(State &state){
	// new light
	if(state.height>600.0f){
		int p=Light::probability(state.height);
		if(p>0){
			if(onein(p)){
				state.light_list.push_back(Light(state));
			}
		}
	}

	for(std::vector<Light>::iterator it=state.light_list.begin();it!=state.light_list.end();){
		Light &light=*it;

		if(state.player.alive_y<PLAYER_BASELINE)
			light.y+=PLAYER_BASELINE-state.player.alive_y;

		// delete if off screen
		if(light.y>state.renderer.rect.bottom){
			it=state.light_list.erase(it);
			continue;
		}

		// push downward
		light.y+=light.yv;

		++it;
	}
}

void Light::render(const Renderer &renderer,const std::vector<Light> &light_list){
	for(const Light &light:light_list){
		glUniform4f(renderer.uniform.rgba,light.rgb[0],light.rgb[1],light.rgb[2],1.0f);
		renderer.draw(light,&renderer.atlas);
	}

	glUniform4f(renderer.uniform.rgba,1.0f,1.0f,1.0f,1.0f);
}

int Light::probability(int height){
	int p;

	if(height<LIGHT_STEADY_HEIGHT)
		p=LIGHT_STEADY_HEIGHT-height;
	else if(height<LIGHT_STOP_HEIGHT)
		p=1;
	else
		p=0;

	if(p<0)
		p=0;

	return p;
}

void Light::choose_color(){
	int r=randomint(0,5);
	switch(r){
	case 0: // red
		rgb[0]=0.6f;rgb[1]=0.3f;rgb[2]=0.25f;
		break;
	case 1: // green
		rgb[0]=0.25f;rgb[1]=0.65f;rgb[2]=0.203f;
		break;
	case 2: // blue
		rgb[0]=0.3f;rgb[1]=0.2f;rgb[2]=0.75f;
		break;
	case 3: // yellow
		rgb[0]=0.671f;rgb[1]=0.7f;rgb[2]=0.3f;
		break;
	case 4: // cyan
		rgb[0]=0.33f;rgb[1]=0.71f;rgb[2]=0.75f;
		break;
	case 5: // magenta
		rgb[0]=0.79f;rgb[1]=0.33f;rgb[2]=0.71f;
	}
}
