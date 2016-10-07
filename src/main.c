#include "main.h"
#include "la_video.h"

#include <la_thread.h>

int tv_vision(void* data);

float color[] = { 0.f, 1.f, 0.f, 1.f };

static void tv_exit(ctx_t* ctx) {
	const char* error;

	if((error = car_camera_kill(&ctx->camera)))
		la_panic("car_camera_kill error %s:", error);
	ccv_scd_classifier_cascade_free(ctx->cascade);
}

static void tv_panic(ctx_t* ctx, const char* format, ...) {
	va_list arglist;

	tv_exit(ctx);

	va_start(arglist, format);
	la_panic(format, arglist);
	va_end(arglist);
}

void tv_loop(ctx_t* ctx) {
}

uint8_t tv_locked_loop(ctx_t* ctx, la_window_t* window) {
/*	ccv_scd_param_t param;
	param.interval = 1;
	param.min_neighbors = 0;
	param.size = (ccv_size_t) { 100, 100 };
	param.step_through = 50;*/

	int i;
	for (i = 0; i < ctx->detections.count; i++) {
		jl_rect_t rc = ctx->detections.rect[i];
		printf("%f %f %f %f\n", rc.x, rc.y, rc.w, rc.h);
		rc.y *= jl_gl_ar(window);
		rc.h *= jl_gl_ar(window);
		rc.x *= jl_gl_ar(window) * (640.f/480.f);
		rc.w *= jl_gl_ar(window) * (640.f/480.f);

		la_ro_rect(window, &ctx->pointer, rc.w, rc.h);
		la_ro_change_image(&ctx->pointer, window->textures.game, 16, 16, 250, 0);
//		jlgr_vo_txmap(jlgr,  0, 16, 16, 250);
		la_ro_move(&ctx->pointer, (la_v3_t) { rc.x, rc.y, 0.f });
		la_ro_draw(&ctx->pointer);

	}

	return 0;
}

static void tv_draw(ctx_t* ctx, la_window_t* window) {
	const char* error;
	uint16_t w, h;

	if((error = car_camera_loop(&ctx->camera)))
		tv_panic(ctx, "camera loop error: %s", error);
	SDL_LockMutex(ctx->detections.mutex);
	la_video_load_jpeg(ctx->pixels, ctx->video_stream, ctx->camera.size, &w, &h);
	la_texture_set(window, ctx->video_stream_texture, ctx->pixels, 640, 480, 3);
	la_ro_change_image(&ctx->display, ctx->video_stream_texture, 0, 0, -1, 0);
	la_ro_draw(&ctx->display);

	// Vision

	tv_locked_loop(ctx, window);
	SDL_UnlockMutex(ctx->detections.mutex);
}

static void tv_resize(ctx_t* ctx, la_window_t* window) {
	const float height = jl_gl_ar(window);
	const float width = jl_gl_ar(window) * (640.f/480.f);

	// Retain aspect ratio for camera display.
	la_ro_rect(window, &ctx->display, width, height);
}

static inline void tv_init_camera(ctx_t* ctx, la_window_t* window) {
	const char* error;
	if((error = car_camera_init(&ctx->camera, 0, 640, 480, &ctx->video_stream)))
		tv_panic(ctx, "car_camera_init error %s:", error);
	ctx->video_stream_texture = la_texture_new(window, NULL, 640, 480, 3);
}

static void tv_init(ctx_t* ctx, la_window_t* window) {
	jlgr_draw_msge(window, window->textures.logo, 0, "Loading Camera....");
	tv_init_camera(ctx, window);
	jlgr_draw_msge(window, window->textures.logo, 0, "Loaded Sqlite File....");
        ctx->cascade = ccv_scd_classifier_cascade_read("face.sqlite3");
	jlgr_draw_msge(window, window->textures.logo, 0, "Done!");
//	jl_thread_mutex_new(jl, &ctx->detections.mutex);
	la_thread_new(NULL, tv_vision, "vision", ctx);

	la_draw_fnchange(window, (la_draw_fn_t)tv_draw, la_draw_dont, (la_draw_fn_t)tv_resize);
}

int main(int argc, char* argv[]) {
	return la_start(tv_init, (la_fn_t) tv_loop, (la_fn_t) tv_exit,
		"2017 Vision Tool", sizeof(ctx_t));
}
