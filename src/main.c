#include "main.h"
#include "la_video.h"

#include <la_thread.h>
#include <la_file.h>
#include <la_memory.h>
#include <la_string.h>

int tv_vision(void* data);

float color[] = { 0.f, 1.f, 0.f, 1.f };

static void tv_exit(ctx_t* ctx) {
	const char* error;

	la_print("EXIT %p", ctx);
//	car_camera_kill(&ctx->camera);
	if((error = car_camera_kill(&ctx->camera)))
		la_panic("car_camera_kill error %s:", error);

}

static void tv_panic(ctx_t* ctx, const char* format, ...) {
	va_list arglist;

	tv_exit(ctx);

	va_start(arglist, format);
	la_panic(format, arglist);
	va_end(arglist);
}

void tv_loop(ctx_t* ctx) {
	la_window_t* window = ctx->window;
	uint8_t pixels[PIXEL_SIZE];

	if(window->input.keyboard.k == ' ' && window->input.keyboard.p &&
		window->input.keyboard.h)
	{
		la_buffer_t savefile;
		char filename[256];
		uint32_t photo_count = 1;

//		jlgr_notify(window, "PICTURE");
		la_safe_get(&ctx->pixels, pixels, PIXEL_SIZE);
		la_video_make_jpeg(&savefile, 90, pixels, 640, 480);

		while(1) {
			la_memory_clear(filename, 256);
			la_string_append(filename, "photo_");
			la_string_append(filename, la_string_fint(photo_count));
			la_string_append(filename, ".jpg");
			printf("PHOTO: %s\n", filename);
			if(la_file_exist(filename) == FILE_TYPE_NONE) {
//			la_file_truncate("picture.jpg");
				la_file_append(filename, savefile.data,
					savefile.size);
				la_buffer_free(&savefile);
				break;
			}
			photo_count++;
		}
	}
}

uint8_t tv_locked_loop(ctx_t* ctx, la_window_t* window) {
/*	ccv_scd_param_t param;
	param.interval = 1;
	param.min_neighbors = 0;
	param.size = (ccv_size_t) { 100, 100 };
	param.step_through = 50;*/

	int i;
	for (i = 0; i < ctx->detections.count; i++) {
		la_rect_t rc = ctx->detections.rect[i];
//		printf("%f %f %f %f\n", rc.x, rc.y, rc.w, rc.h);
		rc.y *= la_ro_ar(window);
		rc.h *= la_ro_ar(window);
		rc.x *= la_ro_ar(window) * (640.f/480.f);
		rc.w *= la_ro_ar(window) * (640.f/480.f);

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
	uint32_t w, h;
	uint8_t pixels[PIXEL_SIZE];

	if((error = car_camera_loop(&ctx->camera)))
		tv_panic(ctx, "camera loop error: %s", error);
	SDL_LockMutex(ctx->detections.mutex);

	//
	la_video_load_jpeg(pixels, ctx->video_stream, ctx->camera.size, &w, &h);
	la_safe_set(&ctx->pixels, pixels, PIXEL_SIZE);
	la_texture_set(window, ctx->video_stream_texture, pixels, 640, 480, 3);
	la_ro_change_image(&ctx->display, ctx->video_stream_texture, 0, 0, -1, 0);
	la_ro_draw(&ctx->display);

	// Vision
	tv_locked_loop(ctx, window);
	SDL_UnlockMutex(ctx->detections.mutex);
}

static void tv_resize(ctx_t* ctx, la_window_t* window) {
	const float height = la_ro_ar(window);
	const float width = la_ro_ar(window) * (640.f/480.f);

	// Retain aspect ratio for camera display.
	la_ro_rect(window, &ctx->display, width, height);
}

static inline void tv_init_camera(ctx_t* ctx, la_window_t* window) {
	const char* error;
	if((error = car_camera_init(&ctx->camera, 0, 640, 480, &ctx->video_stream)))
		tv_panic(ctx, "car_camera_init error %s:", error);
	la_print("%p", ctx);
	ctx->video_stream_texture = la_texture_new(window, NULL, 640, 480, 3);
}

static void tv_init(ctx_t* ctx, la_window_t* window) {
	tv_init_camera(ctx, window);
//	ctx->cascade = ccv_scd_classifier_cascade_read("new.sqlite3");
	ctx->detections.mutex = SDL_CreateMutex();
	la_thread_new(NULL, tv_vision, "vision", ctx);

	la_draw_fnchange(window, (la_draw_fn_t)tv_draw, la_draw_dont,
		(la_draw_fn_t)tv_resize);
	ctx->window = window;
}

int main(int argc, char* argv[]) {
	if(argc > 1) {
		char* positive[] = {
			"positive6.jpg",
			"positive7.jpg",
			"positive8.jpg",
			"positive9.jpg",
			"positive5.jpg"
		};

		char* negative[] = {
			"negative1.jpg",
			"negative2.jpg",
			"negative3.jpg",
			"negative4.jpg"
		};

		ccv_scd_train_param_t params = {
			.boosting = 10,
			.size = ccv_size(48, 48),
			.feature = {
				.base = ccv_size(8, 8),
				.range_through = 4,
				.step_through = 4,
			},
			.stop_criteria = {
				.hit_rate = 0.995,
				.false_positive_rate = 0.5,
				.accu_false_positive_rate = 1e-7,
				.auc_crit = 1e-5,
				.maximum_feature = 2048,
				.prune_stage = 3,
				.prune_feature = 4,
			},
			.weight_trimming = 0.98,
			.C = 0.0005,
			.grayscale = 0,
		};

		ccv_array_t* posfiles = ccv_array_new(sizeof(ccv_file_info_t),
			5, 0);
		ccv_array_t* negfiles = ccv_array_new(sizeof(ccv_file_info_t),
			4, 0);

		ccv_file_info_t pos_info[5];
		ccv_file_info_t neg_info[4];

		for(int i = 0; i < 5; i++) {
			pos_info[i].filename = positive[i];
			ccv_array_push(posfiles, &pos_info[i]);
		}

		for(int i = 0; i < 4; i++) {
			neg_info[i].filename = negative[i];
			ccv_array_push(negfiles, &neg_info[i]);
		}

		la_print("creating cascade....");
		ccv_scd_classifier_cascade_t* cascade =
			ccv_scd_classifier_cascade_new(posfiles, negfiles, 4,
				"new.sqlite3", params);
		ccv_scd_classifier_cascade_write(cascade, "new.sqlite3");
		return 0;
	}
	return la_start(tv_init, (la_fn_t) tv_loop, (la_fn_t) tv_exit,
		"2846 Vision Tool - 2017", sizeof(ctx_t));
}
