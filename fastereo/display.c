/*
 * File:     $RCSfile: display.c,v $
 * Author:   Jean-François LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/04/15 05:21:22 $
 * Version:  $Revision: 1.1 $
 * ID:       $Id: display.c,v 1.1 2004/04/15 05:21:22 arutha Exp $
 * Comments:
 */
/**
 * @file display.c
 * @brief Gestion de l'affichage OpenGL.
 */

#include <stdlib.h>
#include <GL/glut.h>
#include "dbg.h"
#include "utils.h"
#include "display.h"
#include "geom.h"

/** Variable globale représentant la position de la caméra */
Point3d_t g_cam_pos = CAM_POS;
/** Variable globale représentant le point de référence */
Point3d_t g_ref_point = REF_POINT;
/** Variable globale représentant le vecteur UP de la caméra */
Vector3d_t g_cam_up = CAM_UP;
/** Variable globale représentant la largeur de la fenêtre */
unsigned int g_width = WIN_SIZE_X;
/** Variable globale représentant la hauteur de la fenêtre */
unsigned int g_height = WIN_SIZE_Y;

/**
 * Initialise l'affichage
 * @param argcp pointeur sur le nombre d'arguments passés sur la ligne de 
 *              commande
 * @param argv tableau contenant les arguments
 */
void init_display(int *argcp, 
                  char **argv)
{
    Edbg(("init_display(*argcp=%d, argv='%s')", *argcp, argv[0]));

    glutInit(argcp, argv);
    glutInitDisplayMode(DISPLAY_MODE);
    glutInitWindowPosition(WIN_POS_X, WIN_POS_Y);
    glutInitWindowSize(WIN_SIZE_X, WIN_SIZE_Y);
    glutCreateWindow(WIN_TITLE);

    init_gl();
    glutDisplayFunc(refresh_display);
    glutReshapeFunc(reshape_display);

    Rdbg(("init_display"));
}

/**
 * Boucle principale pour l'affichage
 */
void start_display(void)
{
    Edbg(("start_display()"));

    glutMainLoop();

    Rdbg(("start_display"));
}

/**
 * Initialise OpenGL
 */
void init_gl(void)
{
    Edbg(("init_gl()"));

    glClearColor(BG_COLOR);

    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    gluLookAt(g_cam_pos.x, g_cam_pos.y, g_cam_pos.z,
              g_ref_point.x, g_ref_point.y, g_ref_point.z,
              g_cam_up.x, g_cam_up.y, g_cam_up.z);

    glMatrixMode(GL_PROJECTION);
    gluPerspective(CAM_THETA, (float)g_width/(float)g_height,
                   NEAR_PLANE, FAR_PLANE);

    glMatrixMode(GL_MODELVIEW);

    Rdbg(("init_gl"));
}

/**
 * Raffraichit l'affichage OpenGL
 */
void refresh_display(void)
{
    Edbg(("refresh_display()"));
    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();

    /* on dessine un carré */
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_LINE);
    glBegin(GL_QUADS);
    {
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(-0.5, -0.5, -NEAR_PLANE);
        glVertex3f( 0.5, -0.5, -NEAR_PLANE);
        glVertex3f( 0.5,  0.5, -NEAR_PLANE);
        glVertex3f(-0.5,  0.5, -NEAR_PLANE);
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(-0.25, -0.25, -NEAR_PLANE);
        glVertex3f( 0.25, -0.25, -NEAR_PLANE);
        glVertex3f( 0.25,  0.25, -NEAR_PLANE);
        glVertex3f(-0.25,  0.25, -NEAR_PLANE);
        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(-0.125, -0.125, -NEAR_PLANE);
        glVertex3f( 0.125, -0.125, -NEAR_PLANE);
        glVertex3f( 0.125,  0.125, -NEAR_PLANE);
        glVertex3f(-0.125,  0.125, -NEAR_PLANE);
    }
    glEnd();

    glFlush();
    Rdbg(("refresh_display"));
}

/**
 * Redimensionne l'affichage OpenGL
 * @param new_width nouvelle largeur de la fenêtre
 * @param new_height nouvelle hauteur de la fenêtre
 */
void reshape_display(GLint new_width, 
                     GLint new_height)
{
    Edbg(("reshape_display(new_width=%d, new_height=%d)", new_width, 
          new_height));

    glViewport(0, 0, new_width, new_height);

    g_width = new_width;
    g_height = new_height;

    Rdbg(("reshape_display"));
}

/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
