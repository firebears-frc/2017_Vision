#include "main.h"
#include "la_video.h"

int tv_vision(void* data);

float color[] = { 0.f, 1.f, 0.f, 1.f };

static void tv_exit(jl_t* jl) {
	ctx_t* ctx = la_context(jl);
	const char* error;

	if((error = car_camera_kill(&ctx->camera)))
		la_panic(jl, "car_camera_kill error %s:", error);
	ccv_scd_classifier_cascade_free(ctx->cascade);
}

static void tv_panic(jl_t* jl, const char* format, ...) {
	va_list arglist;

	tv_exit(jl);

	va_start( arglist, format );
	la_panic( jl, format, arglist );
	va_end( arglist );
}

void tv_edit_loop(jl_t* jl) {
}

uint8_t tv_locked_loop(jlgr_t* jlgr, ctx_t* ctx) {
/*	ccv_scd_param_t param;
	param.interval = 1;
	param.min_neighbors = 0;
	param.size = (ccv_size_t) { 100, 100 };
	param.step_through = 50;*/

	int i;
	for (i = 0; i < ctx->detections.count; i++) {
		jl_rect_t rc = ctx->detections.rect[i];
		printf("%f %f %f %f\n", rc.x, rc.y, rc.w, rc.h);
		rc.y *= jl_gl_ar(jlgr);
		rc.h *= jl_gl_ar(jlgr);
		rc.x *= jl_gl_ar(jlgr) * (640.f/480.f);
		rc.w *= jl_gl_ar(jlgr) * (640.f/480.f);

		jl_rect_t rc2 = { 0.f, 0.f, rc.w, rc.h };
		jlgr_vo_set_image(jlgr, &ctx->pointer, rc2, jlgr->textures.game);
		jlgr_vo_txmap(jlgr, &ctx->pointer, 0, 16, 16, 250);
		jlgr_vo_move(&ctx->pointer, (jl_vec3_t) { rc.x, rc.y, 0.f });
		jlgr_vo_draw(jlgr, &ctx->pointer);

	}

	return 0;
}

static void tv_wdns(jl_t* jl) {
	jlgr_t* jlgr = jl->jlgr;
	ctx_t* ctx = la_context(jl);
	const char* error;
	uint16_t w, h;

	if((error = car_camera_loop(&ctx->camera)))
		tv_panic(jl, "camera loop error: %s", error);
	jl_thread_mutex_lock(&ctx->detections.mutex);
	la_video_load_jpeg(jl, ctx->pixels, ctx->video_stream, ctx->camera.size, &w, &h);
	la_texture_set(jlgr, ctx->video_stream_texture, ctx->pixels, 640, 480, 3);
	jlgr_vo_image(jlgr, &ctx->display, ctx->video_stream_texture);
	jlgr_vo_draw(jlgr, &ctx->display);

	// Vision

	tv_locked_loop(jlgr, ctx);
	jl_thread_mutex_unlock(&ctx->detections.mutex);
}

static void tv_resize(jl_t* jl) {
	jlgr_t* jlgr = jl->jlgr;
	ctx_t* ctx = la_context(jl);
	const float height = jl_gl_ar(jlgr);
	const float width = jl_gl_ar(jlgr) * (640.f/480.f);
	jl_rect_t rc = { 0.f, 0.f, width, height };

	// Retain aspect ratio for camera display.
	jlgr_vo_rect(jlgr, &ctx->display, &rc);
}

void tv_edit_init(jl_t* jl) {
	jlgr_loop_set(jl->jlgr, tv_wdns, la_dont, tv_wdns, tv_resize);
}

static inline void tv_init_modes(jl_t* jl) {
	jl_mode_set(jl, MODE_EDIT, (jl_mode_t) {
		tv_edit_init, tv_edit_loop, jl_dont
	});
}

static inline void tv_init_camera(jlgr_t* jlgr, ctx_t* ctx) {
	const char* error;
	if((error = car_camera_init(&ctx->camera, 0, 640, 480, &ctx->video_stream)))
		tv_panic(jlgr->jl, "car_camera_init error %s:", error);
	ctx->video_stream_texture = la_texture_new(jlgr, NULL, 640, 480, 3);
}

static void tv_init(jl_t* jl) {
	jlgr_t* jlgr = jl->jlgr;
	ctx_t* ctx = la_context(jl);

	jlgr_draw_msge(jlgr, jlgr->textures.logo, 0, "Loading Camera....");
	tv_init_camera(jlgr, la_context(jl));
	jlgr_draw_msge(jlgr, jlgr->textures.logo, 0, "Loaded Sqlite File....");
        ctx->cascade = ccv_scd_classifier_cascade_read("face.sqlite3");
	jlgr_draw_msge(jlgr, jlgr->textures.logo, 0, "Done!");
	jl_thread_mutex_new(jl, &ctx->detections.mutex);
	jl_thread_new(jl, "vision", tv_vision);

	// Create the modes & initialize one.
	tv_init_modes(jl);
	//Set mode
	jl_mode_switch(jl, MODE_EDIT);
}

static void tv_main(jl_t* jl) {
	jlgr_init(jl, 0, tv_init); // Open Window
}

int main(int argc, char* argv[]) {
	return la_start(tv_main, tv_exit, "Test-Video!", sizeof(ctx_t));
}
