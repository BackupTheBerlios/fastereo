/*
 * File:     $RCSfile: display.h,v $
 * Author:   Jean-François LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/04/26 20:24:40 $
 * Version:  $Revision: 1.5 $
 * ID:       $Id: display.h,v 1.5 2004/04/26 20:24:40 arutha Exp $
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

/** nombre de keyframes par défaut dans l'animation */
#define ANIM_NB_KEYFRAMES 10

void init_display();
void start_display(void);
void init_gl(void);
void construct_scene(char destroyDL);
void display_cam(Camera_t *pcam);
void set_color(Color_t *color, Color_t *tint);
void refresh_display(void);
void reshape_display(SDL_Event *event);
void destroy_display(void);
void mouse_event(SDL_Event *event);
void keyboard_event(SDL_Event *event);

extern float g_zoomim;
extern GLuint g_scene;
extern int g_anim_nb_keyframes;

#endif /* _DISPLAY_H_ */

/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
