/*
 * File:     $RCSfile: display.h,v $
 * Author:   Jean-François LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/04/15 05:21:22 $
 * Version:  $Revision: 1.1 $
 * ID:       $Id: display.h,v 1.1 2004/04/15 05:21:22 arutha Exp $
 */
/**
 * @file display.h
 * @brief Gestion de l'affichage OpenGL.
 */

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <GL/glut.h>

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
/** mode d'affichage pour GLUT */
#define DISPLAY_MODE GLUT_SINGLE | GLUT_RGB

/** couleur du fond */
#define BG_COLOR 0.0, 0.0, 0.0, 0.0
/** position de la caméra */
#define CAM_POS {0.0, 0.0, 0.0, 1.0}
/** point de référence */
#define REF_POINT {0.0, 0.0, -2.0, 1.0}
/** vecteur UP pour la caméra */
#define CAM_UP {0.0, 1.0, 0.0}

/** angle d'ouverture de la caméra */
#define CAM_THETA 90
/** near clipping plane */
#define NEAR_PLANE 0.5
/* far clipping plane */
#define FAR_PLANE 100.0

void init_display(int *argcp, 
                  char **argv);
void start_display(void);
void init_gl(void);
void refresh_display(void);
void reshape_display(GLint new_width, 
                     GLint new_height);

#endif /* _DISPLAY_H_ */

/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
