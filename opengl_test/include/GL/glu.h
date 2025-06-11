#ifndef __glu_h__
#define __glu_h__

#ifdef __cplusplus
extern "C" {
#endif

void gluOrtho2D(double left, double right, double bottom, double top);

#ifdef __cplusplus
}
#endif

#define GL_PROJECTION 0x1701
#define GL_MODELVIEW  0x1700

#endif
