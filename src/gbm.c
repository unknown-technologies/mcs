/* vim:set ts=8 sts=8 sw=8 tw=80 cc=80 noet: */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <mcs/kms.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT;
PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR;
PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHR;
PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES;
PFNEGLCREATESYNCKHRPROC eglCreateSyncKHR;
PFNEGLDESTROYSYNCKHRPROC eglDestroySyncKHR;
PFNEGLWAITSYNCKHRPROC eglWaitSyncKHR;
PFNEGLCLIENTWAITSYNCKHRPROC eglClientWaitSyncKHR;
PFNEGLDUPNATIVEFENCEFDANDROIDPROC eglDupNativeFenceFDANDROID;

static struct gbm_bo* init_bo(GBM* gbm, u64 modifier)
{
	struct gbm_bo* bo = NULL;

	bo = gbm_bo_create_with_modifiers(gbm->dev, gbm->width, gbm->height,
			gbm->format, &modifier, 1);

	if(!bo) {
		if(modifier != DRM_FORMAT_MOD_LINEAR) {
			fprintf(stderr, "Modifiers requested but support isn't available\n");
			return NULL;
		}

		bo = gbm_bo_create(gbm->dev, gbm->width, gbm->height,
				gbm->format, GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
	}

	if(!bo) {
		fprintf(stderr, "failed to create gbm bo\n");
		return NULL;
	}

	return bo;
}

static GBM* init_surfaceless(GBM* gbm, u64 modifier)
{
	for(unsigned int i = 0; i < ARRAY_SIZE(gbm->bos); i++) {
		gbm->bos[i] = init_bo(gbm, modifier);
		if(!gbm->bos[i])
			return NULL;
	}
	return gbm;
}

static GBM* init_surface(GBM* gbm, u64 modifier)
{
	gbm->surface = gbm_surface_create_with_modifiers(gbm->dev, gbm->width,
			gbm->height, gbm->format, &modifier, 1);

	if(!gbm->surface) {
		if(modifier != DRM_FORMAT_MOD_LINEAR) {
			fprintf(stderr, "Modifiers requested but support isn't available\n");
			return NULL;
		}

		gbm->surface = gbm_surface_create(gbm->dev, gbm->width,
				gbm->height, gbm->format,
				GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
	}

	if(!gbm->surface) {
		fprintf(stderr, "failed to create gbm surface\n");
		return NULL;
	}

	return gbm;
}

GBM* init_gbm(int drm_fd, int w, int h, u32 format, u64 modifier,
		bool surfaceless)
{
	GBM* gbm = (GBM*) malloc(sizeof(GBM));

	memset(gbm, 0, sizeof(GBM));

	gbm->dev = gbm_create_device(drm_fd);
	gbm->format = format;
	gbm->surface = NULL;

	gbm->width = w;
	gbm->height = h;

	if(surfaceless)
		return init_surfaceless(gbm, modifier);
	else
		return init_surface(gbm, modifier);
}

void free_gbm(GBM* gbm)
{
	if(gbm->surface)
		gbm_surface_destroy(gbm->surface);
	else
		for(unsigned int i = 0; i < ARRAY_SIZE(gbm->bos); i++)
			gbm_bo_destroy(gbm->bos[i]);

	gbm_device_destroy(gbm->dev);

	free(gbm);
}

static bool has_ext(const char* extension_list, const char* ext)
{
	const char* ptr = extension_list;
	size_t len = strlen(ext);

	if(!ptr || !*ptr)
		return false;

	while(true) {
		ptr = strstr(ptr, ext);
		if(!ptr)
			return false;
		if(ptr[len] == ' ' || !ptr[len])
			return true;
		ptr += len;
	}
}

static int match_config_to_visual(EGLDisplay egl_display, EGLint visual_id,
		EGLConfig* configs, int count)
{
	for(int i = 0; i < count; i++) {
		EGLint id;
		if(!eglGetConfigAttrib(egl_display, configs[i],
					EGL_NATIVE_VISUAL_ID, &id))
			continue;
		if(id == visual_id)
			return i;
	}

	return -1;
}

static bool egl_choose_config(EGLDisplay egl_display, const EGLint* attribs,
		EGLint visual_id, EGLConfig* config_out)
{
	EGLint count = 0;
	EGLint matched = 0;
	EGLConfig* configs;
	int config_index = -1;

	if(!eglGetConfigs(egl_display, NULL, 0, &count) || count < 1) {
		fprintf(stderr, "No EGL configs to choose from.\n");
		return false;
	}

	configs = (EGLConfig*) malloc(count * sizeof(EGLConfig));
	if(!configs)
		return false;

	if(!eglChooseConfig(egl_display, attribs, configs, count, &matched) || !matched) {
		fprintf(stderr, "No EGL configs with appropriate attributes.\n");
		goto out;
	}

	if(!visual_id)
		config_index = 0;

	if(config_index == -1)
		config_index = match_config_to_visual(egl_display, visual_id,
				configs, matched);

	if(config_index != -1)
		*config_out = configs[config_index];

out:
	free(configs);
	return config_index != -1;
}

static bool create_framebuffer(const EGL* egl, struct gbm_bo* bo,
		Framebuffer* fb)
{
	int fd = gbm_bo_get_fd(bo);
	if(fd < 0) {
		fprintf(stderr, "failed to get fd for bo: %d\n", fd);
		return false;
	}

	EGLint khr_image_attrs[17] = {
		EGL_WIDTH, gbm_bo_get_width(bo),
		EGL_HEIGHT, gbm_bo_get_height(bo),
		EGL_LINUX_DRM_FOURCC_EXT, (int) gbm_bo_get_format(bo),
		EGL_DMA_BUF_PLANE0_FD_EXT, fd,
		EGL_DMA_BUF_PLANE0_OFFSET_EXT, 0,
		EGL_DMA_BUF_PLANE0_PITCH_EXT, gbm_bo_get_stride(bo),
		EGL_NONE, EGL_NONE,	/* modifier lo */
		EGL_NONE, EGL_NONE,	/* modifier hi */
		EGL_NONE,
	};

	if(egl->modifiers_supported) {
		const u64 modifier = gbm_bo_get_modifier(bo);
		if(modifier != DRM_FORMAT_MOD_LINEAR) {
			size_t attrs_index = 12;
			khr_image_attrs[attrs_index++] =
			    EGL_DMA_BUF_PLANE0_MODIFIER_LO_EXT;
			khr_image_attrs[attrs_index++] = modifier & 0xFFFFFFFFul;
			khr_image_attrs[attrs_index++] =
			    EGL_DMA_BUF_PLANE0_MODIFIER_HI_EXT;
			khr_image_attrs[attrs_index++] = modifier >> 32;
		}
	}

	fb->image = eglCreateImageKHR(egl->display, EGL_NO_CONTEXT,
			EGL_LINUX_DMA_BUF_EXT, NULL /* no client buffer */,
			khr_image_attrs);

	if(fb->image == EGL_NO_IMAGE_KHR) {
		fprintf(stderr, "failed to make image from buffer object\n");
		return false;
	}

	/* EGLImage takes the fd ownership. */
	close(fd);

	glGenTextures(1, &fb->tex);
	glBindTexture(GL_TEXTURE_2D, fb->tex);
	glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, fb->image);
	glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &fb->fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb->fb);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, fb->tex, 0);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "failed framebuffer check for created target buffer\n");
		glDeleteFramebuffers(1, &fb->fb);
		glDeleteTextures(1, &fb->tex);
		return false;
	}

	return true;
}

static void destroy_framebuffer(const EGL* egl, struct gbm_bo* bo,
		Framebuffer* fb)
{
	if(fb->image) {
		glDeleteFramebuffers(1, &fb->fb);
		glDeleteTextures(1, &fb->tex);
		eglDestroyImageKHR(egl->display, fb->image);
	}
}

int init_egl(EGL* egl, const GBM* gbm, int samples)
{
	EGLint major, minor;

	memset(egl, 0, sizeof(EGL));

	egl->bo = NULL;

	egl->flags = DRM_MODE_ATOMIC_NONBLOCK | DRM_MODE_ATOMIC_ALLOW_MODESET;

	static const EGLint context_attribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	const EGLint config_attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_DEPTH_SIZE, 24,
		EGL_STENCIL_SIZE, 8,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_SAMPLES, samples,
		EGL_NONE
	};
	const char *egl_exts_client, *egl_exts_dpy, *gl_exts;

#define get_proc_client(ext, name) { \
		if (has_ext(egl_exts_client, #ext)) \
			name = (void *)eglGetProcAddress(#name); \
	}
#define get_proc_dpy(ext, name) { \
		if (has_ext(egl_exts_dpy, #ext)) \
			name = (void *)eglGetProcAddress(#name); \
	}

#define get_proc_gl(ext, name) { \
		if (has_ext(gl_exts, #ext)) \
			name = (void *)eglGetProcAddress(#name); \
	}

	egl_exts_client = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
	get_proc_client(EGL_EXT_platform_base, eglGetPlatformDisplayEXT);

	if(eglGetPlatformDisplayEXT) {
		egl->display = eglGetPlatformDisplayEXT(EGL_PLATFORM_GBM_KHR,
				gbm->dev, NULL);
	} else {
		egl->display = eglGetDisplay((void*) gbm->dev);
	}

	if(!eglInitialize(egl->display, &major, &minor)) {
		fprintf(stderr, "failed to initialize\n");
		return -1;
	}

	egl_exts_dpy = eglQueryString(egl->display, EGL_EXTENSIONS);
	get_proc_dpy(EGL_KHR_image_base, eglCreateImageKHR);
	get_proc_dpy(EGL_KHR_image_base, eglDestroyImageKHR);
	get_proc_dpy(EGL_KHR_fence_sync, eglCreateSyncKHR);
	get_proc_dpy(EGL_KHR_fence_sync, eglDestroySyncKHR);
	get_proc_dpy(EGL_KHR_fence_sync, eglWaitSyncKHR);
	get_proc_dpy(EGL_KHR_fence_sync, eglClientWaitSyncKHR);
	get_proc_dpy(EGL_ANDROID_native_fence_sync, eglDupNativeFenceFDANDROID);

	egl->modifiers_supported = has_ext(egl_exts_dpy,
			"EGL_EXT_image_dma_buf_import_modifiers");

	printf("Using display %p with EGL version %d.%d\n", egl->display, major,
			minor);

	printf("===================================\n");
	printf("EGL information:\n");
	printf("  version: \"%s\"\n", eglQueryString(egl->display, EGL_VERSION));
	printf("  vendor: \"%s\"\n", eglQueryString(egl->display, EGL_VENDOR));
#ifdef DEBUG
	printf("  client extensions: \"%s\"\n", egl_exts_client);
	printf("  display extensions: \"%s\"\n", egl_exts_dpy);
#endif
	printf("===================================\n");

	if(!eglBindAPI(EGL_OPENGL_ES_API)) {
		fprintf(stderr, "failed to bind api EGL_OPENGL_ES_API\n");
		return -1;
	}

	if(!egl_choose_config(egl->display, config_attribs, gbm->format,
				&egl->config)) {
		fprintf(stderr, "failed to choose config\n");
		return -1;
	}

	egl->context = eglCreateContext(egl->display, egl->config,
			EGL_NO_CONTEXT, context_attribs);
	if(egl->context == NULL) {
		fprintf(stderr, "failed to create context\n");
		return -1;
	}

	if(!gbm->surface) {
		egl->surface = EGL_NO_SURFACE;
	} else {
		egl->surface = eglCreateWindowSurface(egl->display, egl->config,
				(EGLNativeWindowType) gbm->surface, NULL);
		if(egl->surface == EGL_NO_SURFACE) {
			fprintf(stderr, "failed to create egl surface\n");
			return -1;
		}
	}

	/* connect the context to the surface */
	eglMakeCurrent(egl->display, egl->surface, egl->surface,
			egl->context);

	gl_exts = (char*) glGetString(GL_EXTENSIONS);
	printf("OpenGL ES 2.x information:\n");
	printf("  version: \"%s\"\n", glGetString(GL_VERSION));
	printf("  shading language version: \"%s\"\n",
			glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("  vendor: \"%s\"\n", glGetString(GL_VENDOR));
	printf("  renderer: \"%s\"\n", glGetString(GL_RENDERER));
#ifdef DEBUG
	printf("  extensions: \"%s\"\n", gl_exts);
#endif
	printf("===================================\n");

	get_proc_gl(GL_OES_EGL_image, glEGLImageTargetTexture2DOES);

	if(!gbm->surface) {
		for(unsigned int i = 0; i < ARRAY_SIZE(gbm->bos); i++) {
			if(!create_framebuffer(egl, gbm->bos[i], &egl->fbs[i])) {
				fprintf(stderr, "failed to create framebuffer\n");
				return -1;
			}
		}
	}

	return 0;
}

void free_egl(EGL* egl, const GBM* gbm)
{
	if(!gbm->surface)
		for(unsigned int i = 0; i < ARRAY_SIZE(gbm->bos); i++)
			destroy_framebuffer(egl, gbm->bos[i], &egl->fbs[i]);

	eglMakeCurrent(egl->display, EGL_NO_SURFACE, EGL_NO_SURFACE,
			EGL_NO_CONTEXT);

	eglDestroyContext(egl->display, egl->context);

	if(egl->surface != EGL_NO_SURFACE) {
		eglDestroySurface(egl->display, egl->surface);
	}

	eglTerminate(egl->display);
}
