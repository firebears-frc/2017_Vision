#include <car.h>
#include <la.h>
#include <la_ro.h>
//#include "jlgr.h"
#include "ccv.h"

#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480
#define PIXEL_SIZE (IMAGE_WIDTH * IMAGE_HEIGHT * 3)

enum {
	MODE_EDIT,
	MODE_MAXX,
} mode;

typedef struct {
	la_safe_t lock;
	uint8_t pixels[PIXEL_SIZE];
}tv_pixel_t;

typedef struct{
//	jl_sprite_t slider[3];
//	float s1[3];
//	float s2[3];
	la_window_t* window;

	uint8_t hasMenu;
	uint32_t lightTex;
	car_camera_t camera;
	void* video_stream;
	uint32_t video_stream_texture;
	tv_pixel_t pixels;
	la_ro_t vo;
	la_ro_t display;
	la_ro_t pointer;
	ccv_dense_matrix_t* matrix;
	ccv_array_t* vision_objects;

	struct{
		SDL_mutex* mutex;
		la_rect_t rect[8]; // Detect up to 8
		uint8_t count; // 0 - 8
	} detections;

}ctx_t;
