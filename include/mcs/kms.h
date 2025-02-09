/* vim:set ts=8 sts=8 sw=8 tw=80 cc=80 noet: */
#ifndef __KMS_H__
#define __KMS_H__

#undef	GL_GLEXT_PROTOTYPES
#undef	EGL_EGLEXT_PROTOTYPES

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <gbm.h>
#include <drm_fourcc.h>

#include <xf86drm.h>
#include <xf86drmMode.h>

#include "types.h"

#define NUM_BUFFERS 2

extern PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT;
extern PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR;
extern PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHR;
extern PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES;
extern PFNEGLCREATESYNCKHRPROC eglCreateSyncKHR;
extern PFNEGLDESTROYSYNCKHRPROC eglDestroySyncKHR;
extern PFNEGLWAITSYNCKHRPROC eglWaitSyncKHR;
extern PFNEGLCLIENTWAITSYNCKHRPROC eglClientWaitSyncKHR;
extern PFNEGLDUPNATIVEFENCEFDANDROIDPROC eglDupNativeFenceFDANDROID;

typedef struct {
	drmModeObjectProperties*	props;
	drmModePropertyRes**		props_info;
} DRMModeInfo;

typedef struct {
	DRMModeInfo			info;
	drmModePlane*			plane;
} Plane;

typedef struct {
	DRMModeInfo			info;
	drmModeCrtc*			crtc;
} CRTC;

typedef struct {
	DRMModeInfo			info;
	drmModeConnector*		connector;
} Connector;

typedef struct {
	struct gbm_device*		dev;
	struct gbm_surface*		surface;
	struct gbm_bo*			bos[NUM_BUFFERS];
	u32				format;
	int				width;
	int				height;
} GBM;

typedef struct {
	EGLImageKHR			image;
	GLuint				tex;
	GLuint				fb;
} Framebuffer;

typedef struct {
	int			fd;

	Plane*			plane;
	CRTC*			crtc;
	Connector*		connector;
	int			crtc_index;
	int			kms_in_fence_fd;
	int			kms_out_fence_fd;

	drmModeModeInfo*	mode;
	u32			crtc_id;
	u32			connector_id;

	drmModeConnector*	con;
	drmModeEncoder*		encoder;
} DRM;

typedef struct {
	EGLDisplay		display;
	EGLConfig		config;
	EGLContext		context;
	EGLSurface		surface;
	Framebuffer		fbs[NUM_BUFFERS];

	bool			modifiers_supported;

	struct gbm_bo*		bo;

	unsigned int		fb;
	u32			flags;

	EGLSyncKHR		kms_fence; /* in-fence to gpu, out-fence from kms */
} EGL;

DRM* init_drm(const char* device, const char* mode_str, unsigned int vrefresh);
GBM* init_gbm(int drm_fd, int w, int h, u32 format, u64 modifier,
		bool surfaceless);
int init_egl(EGL* egl, const GBM* gbm, int samples);

void free_drm(DRM* drm);
void free_gbm(GBM* gbm);
void free_egl(EGL* egl, const GBM* gbm);

void egl_before(const GBM* gbm, DRM* drm, EGL* egl);
void egl_done(const GBM* gbm, DRM* drm, EGL* egl);

#endif
