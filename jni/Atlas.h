#ifndef ATLAS_H
#define ATLAS_H

#include <vector>

struct AtlasCoords{
	float left;
	float right;
	float bottom;
	float top;
};

class Atlas{
public:
	Atlas();
	bool load(AAssetManager*,const char*);
	void unload();
	unsigned texture(){return object;}
	void coords(int i,AtlasCoords &ac){ac=tex_coords[i];}

private:
	std::vector<AtlasCoords> tex_coords;
	unsigned object; // texture object;
};

#endif // ATLAS_H
