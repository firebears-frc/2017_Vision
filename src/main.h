#include <car.h>
#include <la.h>
#include <la_ro.h>
//#include "jlgr.h"
#include "ccv.h"

enum {
	MODE_EDIT,
	MODE_MAXX,
} mode;

typedef struct{
//	jl_sprite_t slider[3];
//	float s1[3];
//	float s2[3];
	uint8_t hasMenu;
	uint32_t lightTex;
	car_camera_t camera;
	void* video_stream;
	uint32_t video_stream_texture;
	uint8_t pixels[640*480*3];
	ccv_scd_classifier_cascade_t* cascade;
	la_ro_t vo;
	la_ro_t display;
	la_ro_t pointer;
	ccv_dense_matrix_t* matrix;
	ccv_array_t* vision_objects;

	struct{
		SDL_mutex* mutex;
		jl_rect_t rect[8]; // Detect up to 8
		uint8_t count; // 0 - 8
	} detections;

}ctx_t;
