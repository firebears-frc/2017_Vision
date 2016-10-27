#include "main.h"
#include "la_video.h"

#include <SDL_thread.h>

uint8_t tv_vision_sync(ctx_t* ctx) {
	int i;
	uint8_t pixels[PIXEL_SIZE];

	if(ctx->matrix) ccv_matrix_free(ctx->matrix);
	la_safe_get(&ctx->pixels, pixels, PIXEL_SIZE);
	ccv_read(pixels, &ctx->matrix, CCV_IO_RGB_RAW, 480, 640,
		640*3);

	if(ctx->vision_objects == NULL) return 1;

	ctx->detections.count = (ctx->vision_objects->rnum < 8) ?
		ctx->vision_objects->rnum : 8;

	for(i = 0; i < ctx->detections.count; i ++) {
		la_print("GOT %d", i);
		ccv_comp_t* face = (ccv_comp_t*)ccv_array_get(
			ctx->vision_objects, i);
		ctx->detections.rect[i] = (la_rect_t) {
			((float)face->rect.x) / 640.f,
			((float)face->rect.y) / 480.f,
			((float)face->rect.width) / 640.f,
			((float)face->rect.height) / 480.f
		};
	}
//	printf("detect.\n");
	return 0;
}

int tv_vision(ctx_t* ctx) {
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
			.maximum_feature = 1,
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

	unlink("temp.sqlite3");
	la_print("creating cascade....");
	ccv_scd_classifier_cascade_t* cascade =
		ccv_scd_classifier_cascade_new(posfiles, negfiles, 4,
			"temp.sqlite3", params);
	la_print("created cascade....");
	while(1) {
		SDL_LockMutex(ctx->detections.mutex);
		tv_vision_sync(ctx);
		SDL_UnlockMutex(ctx->detections.mutex);

		if(ctx->vision_objects) ccv_array_free(ctx->vision_objects);
		la_print("detecting....");
		ctx->vision_objects = ccv_scd_detect_objects(ctx->matrix,
			&cascade, 1, ccv_scd_default_params /*param*/ );
		la_print("detected.");
	}

	ccv_scd_classifier_cascade_free(cascade);
}
