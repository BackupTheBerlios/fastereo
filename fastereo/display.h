/*
 * File:     $RCSfile: display.h,v $
 * Author:   Jean-Fran�ois LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/04/19 18:59:29 $
 * Version:  $Revision: 1.4 $
 * ID:       $Id: display.h,v 1.4 2004/04/19 18:59:29 arutha Exp $
 */
/**
 * @file display.h
 * @brief Gestion de l'affichage OpenGL.
 */

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <GL/gl.h>
#include <GL/glu.h>
#include "SDL.h"
#include "cameras.h"

/** position de la fen�tre en X */
#define WIN_POS_X 100
/** position de la fen�tre en Y */
#define WIN_POS_Y 100
/** taille de la fen�tre en X */
#define WIN_SIZE_X 500
/** taille de la fen�tre en Y */
#define WIN_SIZE_Y 500
/** titre de la fen�tre */
#define WIN_TITLE "Fast Stereo"
/** mode d'affichage pour SDL */
#define DISPLAY_MODE SDL_OPENGL | SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_RESIZABLE
// #define DISPLAY_MODE SDL_OPENGL
/** zoom par rapport aux images charg�es */
#define ZOOM 2.0

/** couleur du fond */
#define BG_COLOR 0.0, 0.0, 0.0, 0.0

#define STEP_Z 1.3

/** angle d'ouverture de la cam�ra */
#define EYE_THETA 90
/** near clipping plane */
#define NEAR_PLANE 10.0
/** far clipping plane */
#define FAR_PLANE 1000.0
/** position de la cam�ra */
#define EYE_POS {0.0, 0.0, 0.0, 1.0}
/** point de r�f�rence */
#define REF_POINT {0.0, 0.0, -NEAR_PLANE, 1.0}
/** vecteur UP pour la cam�ra */
#define EYE_UP {0.0, 1.0, 0.0}

/** nombre de keyframes par d�faut dans l'animation */
#define ANIM_NB_KEYFRAMES 10

void init_display();
void start_display(void);
void init_gl(void);
void construct_scene(void);
int compute_cam_position(Camera_t *cam);
void display_cam(Camera_t *pcam,
                 float hscreen,
                 float wscreen,
                 float step_x,
                 float step_y);
void set_color(Color_t *color, Color_t *tint);
void refresh_display(void);
void reshape_display(SDL_Event *event);
void destroy_display(void);
void mouse_event(SDL_Event *event);
void deprojGL_point(Point3d_t *point, Point3d_t *eye, float step_z, 
                    float label);

extern float g_zoom;
extern GLuint g_scene;
extern int g_anim_nb_keyframes;

#endif /* _DISPLAY_H_ */

/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
