/*
 * File:     $RCSfile: display.h,v $
 * Author:   Jean-François LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/04/15 12:45:51 $
 * Version:  $Revision: 1.2 $
 * ID:       $Id: display.h,v 1.2 2004/04/15 12:45:51 arutha Exp $
 */
/**
 * @file display.h
 * @brief Gestion de l'affichage OpenGL.
 */

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <GL/glut.h>
#include "SDL.h"

/** position de la fenêtre en X */
#define WIN_POS_X 100
/** position de la fenêtre en Y */
#define WIN_POS_Y 100
/** taille de la fenêtre en X */
#define WIN_SIZE_X 500
/** taille de la fenêtre en Y */
#define WIN_SIZE_Y 500
/** titre de la fenêtre */
#define WIN_TITLE "Fast Stereo"
/** mode d'affichage pour SDL */
#define DISPLAY_MODE SDL_OPENGL | SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_RESIZABLE
// #define DISPLAY_MODE SDL_OPENGL
/** zoom par rapport aux images chargées */
#define ZOOM 2.0

/** couleur du fond */
#define BG_COLOR 0.0, 0.0, 0.0, 0.0

/** angle d'ouverture de la caméra */
#define CAM_THETA 90
/** near clipping plane */
#define NEAR_PLANE 0.5
/** far clipping plane */
#define FAR_PLANE 1000.0
/** position de la caméra */
#define CAM_POS {0.0, 0.0, NEAR_PLANE, 1.0}
/** point de référence */
#define REF_POINT {0.0, 0.0, 0.0, 1.0}
/** vecteur UP pour la caméra */
#define CAM_UP {0.0, 1.0, 0.0}

void init_display();
void start_display(void);
void init_gl(void);
void construct_scene(void);
void refresh_display(void);
void reshape_display(SDL_Event *event);
void destroy_display(void);
void mouse_event(SDL_Event *event);

extern float g_zoom;
extern GLuint g_scene;

#endif /* _DISPLAY_H_ */

/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
