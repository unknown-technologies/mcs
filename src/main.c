/* vim:set ts=8 sts=8 sw=8 tw=80 cc=80 noet: */
#include <stdio.h>

#include <mcs/types.h>
#include <mcs/kms.h>
#include <mcs.h>

static MCS mcs;

int main(void)
{
	EGL egl;

	DRM* drm = init_drm(NULL, NULL, 0);
	if(!drm) {
		fprintf(stderr, "failed to initialize DRM\n");
		return 1;
	}

	GBM* gbm = init_gbm(drm->fd, drm->mode->hdisplay,
			drm->mode->vdisplay, DRM_FORMAT_ARGB8888,
			DRM_FORMAT_MOD_LINEAR, false);
	if(!gbm) {
		fprintf(stderr, "failed to initialize GBM\n");
		return 1;
	}

	printf("Screen size: %dx%d\n", drm->mode->hdisplay,
			drm->mode->vdisplay);

	init_egl(&egl, gbm, 0);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int maxtexsz;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxtexsz);
	printf("Max texture size: %d\n", maxtexsz);

	MCSInit(&mcs, drm->mode->hdisplay, drm->mode->vdisplay);

	/* main loop */
	while(1) {
		egl_before(gbm, drm, &egl);
		glClear(GL_COLOR_BUFFER_BIT);

		/* process events */
		MCSProcess(&mcs);

		/* render UI */
		MCSRender(&mcs);

		egl_done(gbm, drm, &egl);
	}

	MCSFree(&mcs);

	free_egl(&egl, gbm);
	free_gbm(gbm);
	free_drm(drm);
}
