#include "main.h"
#include "la_video.h"

#include <SDL_thread.h>

uint8_t tv_vision_sync(ctx_t* ctx) {
	int i;

	if(ctx->matrix) ccv_matrix_free(ctx->matrix);
	ccv_read(ctx->pixels, &ctx->matrix, CCV_IO_RGB_RAW, 480, 640,
		640*3);

	if(ctx->vision_objects == NULL) return 1;

	ctx->detections.count = (ctx->vision_objects->rnum < 8) ?
		ctx->vision_objects->rnum : 8;

	for(i = 0; i < ctx->detections.count; i ++) {
		ccv_comp_t* face = (ccv_comp_t*)ccv_array_get(
			ctx->vision_objects, i);
		ctx->detections.rect[i] = (jl_rect_t) {
			((float)face->rect.x) / 640.f,
			((float)face->rect.y) / 480.f,
			((float)face->rect.width) / 640.f,
			((float)face->rect.height) / 480.f
		};
	}
	printf("detect.\n");
	return 0;
}

int tv_vision(ctx_t* ctx) {
	while(1) {
		SDL_LockMutex(ctx->detections.mutex);
		tv_vision_sync(ctx);
		SDL_UnlockMutex(ctx->detections.mutex);

		if(ctx->vision_objects) ccv_array_free(ctx->vision_objects);
		ctx->vision_objects = ccv_scd_detect_objects(ctx->matrix,
			&ctx->cascade, 1, ccv_scd_default_params /*param*/ );
	}
}
