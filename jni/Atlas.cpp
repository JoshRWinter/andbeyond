#include <zlib.h>
#include <GLES2/gl2.h>
#include <android/log.h>
#include <android/asset_manager.h>

#include "Atlas.h"

#define logcat(...) ((void)__android_log_print(ANDROID_LOG_INFO, "winter", __VA_ARGS__))

Atlas::Atlas(){
	object=0;
}

bool Atlas::load(AAssetManager *mgr,const char *name){
	AAsset *asset=AAssetManager_open(mgr,name,AASSET_MODE_UNKNOWN);
	if(!asset){
		logcat("atlas: couldn't open file");
		return false;
	}

	// find size of file
	int filesize=AAsset_seek(asset,0,SEEK_END);
	AAsset_seek(asset,0,SEEK_SET);
	logcat("filesize==%u",filesize);

	// check magic
	unsigned char magic[3]={0,0,0};
	AAsset_read(asset,magic,3);
	bool pass=magic[0]=='J'&&magic[1]=='R'&&magic[2]=='W';
	if(!pass){
		AAsset_close(asset);
		logcat("atlas: didn't pass magic");
		return false;
	}

	// read uncompressed size
	uLongf uncomp_size=0;
	AAsset_read(asset,&uncomp_size,sizeof(uLongf));
	logcat("uncompressed size: %lu",uncomp_size);
	if(uncomp_size==0){
		AAsset_close(asset);
		logcat("atlas: uncomp_size was zero");
		return false;
	}

	// read file into memory
	const int compressed_size=filesize-3-sizeof(unsigned int);
	unsigned char *compressed=new unsigned char[compressed_size];
	if(compressed_size!=AAsset_read(asset,compressed,compressed_size)){
		AAsset_close(asset);
		delete[] compressed;
		return false;
	}
	AAsset_close(asset);

	// allocate array for uncompressed data
	unsigned char *uncompressed=new unsigned char[uncomp_size];

	// uncompress with zlib
	int z_result=uncompress(uncompressed,&uncomp_size,compressed,compressed_size);
	delete[] compressed;
	if(z_result!=Z_OK){
		logcat("decompression failed");
		delete[] uncompressed;
		return false;
	}

	// make sure uncompressed chunk is at least as big as the initial header
	const int expected_header_length=2+2+2;
	if(uncomp_size<expected_header_length){
		delete[] uncompressed;
		logcat("failed expected header length check");
		return false;
	}

	// read number of textures present in atlas data
	unsigned short texture_count;
	memcpy(&texture_count,uncompressed,2);
	logcat("found %d textures",texture_count);

	// read atlas bitmap dimensions
	unsigned short canvas_width;
	unsigned short canvas_height;
	memcpy(&canvas_width,uncompressed+2,2);
	memcpy(&canvas_height,uncompressed+4,2);
	logcat("canvas dimensions are %hux%hu",canvas_width,canvas_height);

	// comprehensive size consistency check
	const int expected_length=2+2+2+(texture_count*sizeof(unsigned short)*4)+(canvas_width*canvas_height*4);
	if(expected_length!=uncomp_size){
		delete[] uncompressed;
		logcat("size mismatch: expected=%d, actual=%lu",expected_length,uncomp_size);
		return false;
	}

	// read coordinate data for all textures
	for(int i=0;i<texture_count;++i){
		unsigned short xpos,ypos,width,height;
		memcpy(&xpos,uncompressed+2+2+2+(i*sizeof(unsigned short)*4)+0,2);
		memcpy(&ypos,uncompressed+2+2+2+(i*sizeof(unsigned short)*4)+2,2);
		memcpy(&width,uncompressed+2+2+2+(i*sizeof(unsigned short)*4)+4,2);
		memcpy(&height,uncompressed+2+2+2+(i*sizeof(unsigned short)*4)+6,2);

		// convert to texture coordinates
		AtlasCoords ac;
		ac.left=(float)xpos/canvas_width;
		ac.right=(float)(xpos+width)/canvas_width;
		ac.bottom=(float)ypos/canvas_height;
		ac.top=(float)(ypos+height)/canvas_height;
		logcat("coords for texture %d are left: %.3f, right: %.3f, bottom: %.3f, top: %.3f",i,ac.left,ac.right,ac.bottom,ac.top);

		tex_coords.push_back(ac);
	}

	// texturize for opengl
	unsigned char *bmp_start=uncompressed+2+2+2+(texture_count*sizeof(unsigned short)*4);
	glGenTextures(1,&object);
	glBindTexture(GL_TEXTURE_2D,object);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,canvas_width,canvas_height,0,GL_RGBA,GL_UNSIGNED_BYTE,bmp_start);

	// not needed anymore
	delete[] uncompressed;
	logcat("made it to the end");

	return true;
}

void Atlas::unload(){
	glDeleteTextures(1,&object);
	tex_coords.clear();
}
