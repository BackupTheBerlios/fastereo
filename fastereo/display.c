/*
 * File:     $RCSfile: display.c,v $
 * Author:   Jean-François LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/04/15 12:45:51 $
 * Version:  $Revision: 1.2 $
 * ID:       $Id: display.c,v 1.2 2004/04/15 12:45:51 arutha Exp $
 * Comments:
 */
/**
 * @file display.c
 * @brief Gestion de l'affichage OpenGL.
 */

#include <stdlib.h>
#include "SDL.h"
#include "dbg.h"
#include "utils.h"
#include "display.h"
#include "geom.h"
#include "cameras.h"

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
/** Variable indiquant le zoom à effectuer sur la fenêtre, par rapport aux
 * images chargées */
float g_zoomim = ZOOM;

/** objet display list pour contenir la scène statique */
GLuint g_scene;

/** booléen qui indique quand il faut sortir de la boucle SDL */
char g_done = 0;
/** objet représentant la fenêtre SDL */
SDL_Surface *g_screen = NULL;

/** Rotation en X */
GLfloat g_rot_x = 0.0;
/** Rotation en Y */
GLfloat g_rot_y = 0.0;
/** Zoom */
GLfloat g_zoom = 0.0;
/** Translation sur l'axe Z */
GLfloat g_translate_z = 0.0;

/** Variable indiquant si on est en plein écran */
char g_fullscreen = 0;

/**
 * Initialise l'affichage
 */
void 
init_display()
{
    Edbg(("init_display()"));

    g_width = g_zoomim * g_cameras.root->ii.XSize;
    g_height = g_zoomim * g_cameras.root->ii.YSize;
    Dprintf((1,"g_width: %d\n", g_width));
    Dprintf((1,"g_height: %d\n", g_height));

    if((SDL_Init(SDL_INIT_VIDEO)) < 0)
    {
        fprintf(stderr, "Impossible d'initialiser SDL: %s\n", SDL_GetError());
        exit(2);
    }
    g_screen = SDL_SetVideoMode(g_width, g_height, 0, DISPLAY_MODE);
    if(g_screen == NULL)
    {
        fprintf(stderr, "Impossible de créer la fenêtre SDL: %s\n", 
                SDL_GetError());
        exit(3);
    }
    SDL_WM_SetCaption(WIN_TITLE, NULL);

    init_gl();

    g_rot_x = 0.0;
    g_rot_y = 0.0;
    g_zoom = 0.0;
    g_translate_z = 0.0;

    Rdbg(("init_display"));
}

/**
 * Boucle principale pour l'affichage
 */
void 
start_display(void)
{
    Edbg(("start_display()"));

    while (!g_done)
    {
        refresh_display();

        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    /* quitter l'application */
                    g_done = 1;
                    break;
                case SDL_MOUSEMOTION:
                    /* mouvements de la souris */
                    mouse_event(&event);
                    break;
                case SDL_KEYDOWN:
                    /* quitter l'application si touche escape enfoncée */
                    if (SDLK_ESCAPE == event.key.keysym.sym)
                    {
                        g_done = 1;
                    }
                    else if (SDLK_r == event.key.keysym.sym)
                    {
                        g_rot_x = 0.0;
                        g_rot_y = 0.0;
                        g_zoom = 0.0;
                        g_translate_z = 0.0;
                    }
                    else if (SDLK_z == event.key.keysym.sym)
                    {
                        Uint32 flags = DISPLAY_MODE;
                        if(g_fullscreen)
                        {
                            g_fullscreen = 0;
                        }
                        else
                        {
                            flags |= SDL_FULLSCREEN;
                            g_fullscreen = 1;
                        }
                        g_screen = SDL_SetVideoMode(g_width, g_height, 0,
                                                    flags);
                    }
                    break;
                case SDL_VIDEORESIZE:
                    reshape_display(&event);
                    break;
            }
        }
    }

    Rdbg(("start_display"));
}

/**
 * Initialise OpenGL
 */
void 
init_gl(void)
{
    Edbg(("init_gl()"));

    glViewport(0, 0, g_width, g_height);
    glClearColor(BG_COLOR);
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_SMOOTH);

    Dprintf((1,"g_cam_pos (%.3f, %.3f, %.3f)\n", 
             g_cam_pos.x, g_cam_pos.y, g_cam_pos.z));
    Dprintf((1,"g_ref_point (%.3f, %.3f, %.3f)\n", 
             g_ref_point.x, g_ref_point.y, g_ref_point.z));
    Dprintf((1,"g_cam_up (%.3f, %.3f, %.3f)\n", 
             g_cam_up.x, g_cam_up.y, g_cam_up.z));

    /* glMatrixMode(GL_MODELVIEW); */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(CAM_THETA, (float)g_width/(float)g_height,
                   NEAR_PLANE, FAR_PLANE);
    gluLookAt(g_cam_pos.x, g_cam_pos.y, g_cam_pos.z,
              g_ref_point.x, g_ref_point.y, g_ref_point.z,
              g_cam_up.x, g_cam_up.y, g_cam_up.z);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* on construit la scène statique */
    construct_scene();

    Rdbg(("init_gl"));
}

/**
 * Construit la scène statique
 */
void 
construct_scene(void)
{
    Edbg(("construct_scene()"));

    float hs = 1.0;
    float ws = (float)g_width / (float)g_height;
    float step_x = ws / (g_width/g_zoomim-1);
    float step_y = hs / (g_height/g_zoomim-1);
    float step_z;
    Camera_t *pcam = g_cameras.root;
    Point3d_t point;
    int i, j;
    int width;
    int height;
    Point3d_t p1, p2, p3, p4, p5;
    float color_p1, color_p2, color_p3, color_p4, color_p5;
    int nb_colors;
    Vector3d_t vdir;
    float label;

    Dprintf((1,"step_x: %.6f\n", step_x));
    Dprintf((1,"step_y: %.6f\n", step_y));
    Dprintf((1,"hs: %.6f\tws: %.6f\n", hs, ws));

    point.z = 0;

    g_scene = glGenLists(1);
    glLoadIdentity();

    glNewList(g_scene, GL_COMPILE);
    {
        glBegin(GL_TRIANGLES);
        while (NULL != pcam)
        {
            point.y = -hs/2;
            width = pcam->ii.XSize;
            height = pcam->ii.YSize;
            nb_colors = pcam->ii.ZSize;
            step_z = pcam->range / pcam->nb_labels;
            Dprintf((1,"caméra n°%d\n", pcam->id));
            Dprintf((1,"width: %d\n", width));
            Dprintf((1,"height: %d\n", height));
            Dprintf((1,"step_z: %.3f\n", step_z));

            /* on se déplace sur les y */
            for (i=0; i<height-1; i++)
            {
                /* on va au début de la ligne suivante */
                point.x = pcam->position - ws/2;

                /* on se déplace sur les x */
                for (j=0; j<width-1; j++)
                {
                    /* Dprintf((1,"i: %d\tj: %d\n", i, j));     */
                    /* Dprintf((1,"point (%.3f, %.3f, %.3f)\n", */
                    /*          point.x, point.y, point.z));    */

                    /* on calcule les points */
                    p1 = point;
                    p2 = point; p2.x += step_x;
                    p3 = point; p3.y += step_y;
                    p4 = point; p4.x += step_x; p4.y += step_y;
                    p5 = point; p5.x += step_x/2; p5.y += step_y/2;

                    /* on place les points à la distance correspondant à leur
                     * étiquette */
                    /* point 1 */
                    vdir.x = p1.x;
                    vdir.y = p1.y;
                    vdir.z = p1.z - NEAR_PLANE;
                    normalize3d(&vdir);
                    label = pcam->labels.Data[i*width+j];
                    /* Dprintf((1,"p1 label = %.3f\n", label)); */
                    p1.x += label * step_z * vdir.x;
                    p1.y += label * step_z * vdir.y;
                    p1.z += label * step_z * vdir.z;
                    /* point 2 */
                    vdir.x = p2.x;
                    vdir.y = p2.y;
                    vdir.z = p2.z - NEAR_PLANE;
                    normalize3d(&vdir);
                    label = pcam->labels.Data[i*width+(j+1)];
                    /* Dprintf((1,"p2 label = %.3f\n", label)); */
                    p2.x += label * step_z * vdir.x;
                    p2.y += label * step_z * vdir.y;
                    p2.z += label * step_z * vdir.z;
                    /* point 3 */
                    vdir.x = p3.x;
                    vdir.y = p3.y;
                    vdir.z = p3.z - NEAR_PLANE;
                    normalize3d(&vdir);
                    label = pcam->labels.Data[(i+1)*width+j];
                    /* Dprintf((1,"p3 label = %.3f\n", label)); */
                    p3.x += label * step_z * vdir.x;
                    p3.y += label * step_z * vdir.y;
                    p3.z += label * step_z * vdir.z;
                    /* point 4 */
                    vdir.x = p4.x;
                    vdir.y = p4.y;
                    vdir.z = p4.z - NEAR_PLANE;
                    normalize3d(&vdir);
                    label = pcam->labels.Data[(i+1)*width+(j+1)];
                    /* Dprintf((1,"p4 label = %.3f\n", label)); */
                    p4.x += label * step_z * vdir.x;
                    p4.y += label * step_z * vdir.y;
                    p4.z += label * step_z * vdir.z;
                    /* point 5 */
                    vdir.x = p5.x;
                    vdir.y = p5.y;
                    vdir.z = p5.z - NEAR_PLANE;
                    normalize3d(&vdir);
                    label = InterpoleImg(j+0.5, i+0.5, 0, &(pcam->labels));
                    /* Dprintf((1,"p5 label = %.3f\n", label)); */
                    p5.x += label * step_z * vdir.x;
                    p5.y += label * step_z * vdir.y;
                    p5.z += label * step_z * vdir.z;

                    /* on récupère les couleurs des points */
                    color_p1 = pcam->ii.Data[(i*width+j)*nb_colors];
                    color_p2 = pcam->ii.Data[(i*width+(j+1))*nb_colors];
                    color_p3 = pcam->ii.Data[((i+1)*width+j)*nb_colors];
                    color_p4 = pcam->ii.Data[((i+1)*width+(j+1))*nb_colors];
                    color_p5 = InterpoleImg(j+0.5, i+0.5, 0, &(pcam->ii));
                    color_p1 /= 255.0;
                    color_p2 /= 255.0;
                    color_p3 /= 255.0;
                    color_p4 /= 255.0;
                    color_p5 /= 255.0;

                    /* Dprintf((1,"p1 (%.6f, %.6f, %.6f)\n", p1.x, p1.y, p1.z)); */
                    /* Dprintf((1,"color_p1: %.1f\n", color_p1));                */
                    /* Dprintf((1,"p2 (%.6f, %.6f, %.6f)\n", p2.x, p2.y, p2.z)); */
                    /* Dprintf((1,"color_p2: %.1f\n", color_p2));                */
                    /* Dprintf((1,"p3 (%.6f, %.6f, %.6f)\n", p3.x, p3.y, p3.z)); */
                    /* Dprintf((1,"color_p3: %.1f\n", color_p3));                */
                    /* Dprintf((1,"p4 (%.6f, %.6f, %.6f)\n", p4.x, p4.y, p4.z)); */
                    /* Dprintf((1,"color_p4: %.1f\n", color_p4));                */
                    /* Dprintf((1,"p5 (%.6f, %.6f, %.6f)\n", p5.x, p5.y, p5.z)); */
                    /* Dprintf((1,"color_p5: %.1f\n", color_p5));                */

                    glColor3f(color_p5, color_p5, color_p5);
                    glVertex3f(p5.x, p5.y, p5.z);
                    glColor3f(color_p1, color_p1, color_p1);
                    glVertex3f(p1.x, p1.y, p1.z);
                    glColor3f(color_p2, color_p2, color_p2);
                    glVertex3f(p2.x, p2.y, p2.z);

                    glColor3f(color_p5, color_p5, color_p5);
                    glVertex3f(p5.x, p5.y, p5.z);
                    glColor3f(color_p2, color_p2, color_p2);
                    glVertex3f(p2.x, p2.y, p2.z);
                    glColor3f(color_p4, color_p4, color_p4);
                    glVertex3f(p4.x, p4.y, p4.z);

                    glColor3f(color_p5, color_p5, color_p5);
                    glVertex3f(p5.x, p5.y, p5.z);
                    glColor3f(color_p4, color_p4, color_p4);
                    glVertex3f(p4.x, p4.y, p4.z);
                    glColor3f(color_p3, color_p3, color_p3);
                    glVertex3f(p3.x, p3.y, p3.z);

                    glColor3f(color_p5, color_p5, color_p5);
                    glVertex3f(p5.x, p5.y, p5.z);
                    glColor3f(color_p3, color_p3, color_p3);
                    glVertex3f(p3.x, p3.y, p3.z);
                    glColor3f(color_p1, color_p1, color_p1);
                    glVertex3f(p1.x, p1.y, p1.z);

                    /* glColor3f(color_p1, color_p1, color_p1); */
                    /* glVertex3f(p1.x, p1.y, p1.z);            */
                    /* glColor3f(color_p2, color_p2, color_p2); */
                    /* glVertex3f(p2.x, p2.y, p2.z);            */
                    /* glColor3f(color_p4, color_p4, color_p4); */
                    /* glVertex3f(p4.x, p4.y, p4.z);            */

                    /* glColor3f(color_p1, color_p1, color_p1); */
                    /* glVertex3f(p1.x, p1.y, p1.z);            */
                    /* glColor3f(color_p4, color_p4, color_p4); */
                    /* glVertex3f(p4.x, p4.y, p4.z);            */
                    /* glColor3f(color_p3, color_p3, color_p3); */
                    /* glVertex3f(p3.x, p3.y, p3.z);            */

                    point.x += step_x;
                }

                point.y += step_y;
            }
            pcam = pcam->next;
        }
        glEnd();

        /* on dessine un carré */
        /* glPolygonMode(GL_FRONT, GL_FILL);            */
        /* glPolygonMode(GL_BACK, GL_LINE);             */
        /* glBegin(GL_QUADS);                           */
        /* {                                            */
        /*     glColor3f(1.0, 0.0, 0.0);                */
        /*     glVertex3f(-0.5, -0.5, 0.0);     */
        /*     glVertex3f( 0.5, -0.5, 0.0);     */
        /*     glVertex3f( 0.5,  0.5, 0.0);     */
        /*     glVertex3f(-0.5,  0.5, 0.0);     */
        /*     glColor3f(0.0, 1.0, 0.0);                */
        /*     glVertex3f(-0.25, -0.25, 0.0);   */
        /*     glVertex3f( 0.25, -0.25, 0.0);   */
        /*     glVertex3f( 0.25,  0.25, 0.0);   */
        /*     glVertex3f(-0.25,  0.25, 0.0);   */
        /*     glColor3f(0.0, 0.0, 1.0);                */
        /*     glVertex3f(-0.125, -0.125, 0.0); */
        /*     glVertex3f( 0.125, -0.125, 0.0); */
        /*     glVertex3f( 0.125,  0.125, 0.0); */
        /*     glVertex3f(-0.125,  0.125, 0.0); */
        /* }                                            */
        /* glEnd();                                     */
    }
    glEndList();

    Rdbg(("construct_scene"));
}

/**
 * Raffraichit l'affichage OpenGL
 */
void 
refresh_display(void)
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glLoadIdentity();

    glTranslatef(0, 0, -g_zoom);
    glRotatef(g_rot_x, 1.0, 0.0, 0.0);
    glRotatef(g_rot_y, 0.0, 1.0, 0.0);
    glTranslatef(0, 0, -g_translate_z);
    glCallList(g_scene);

    SDL_GL_SwapBuffers();
    /* SDL_Flip(g_screen); */
}

/**
 * Redimensionne l'affichage OpenGL
 * @param event pointeur sur l'objet des évènements SDL
 */
void 
reshape_display(SDL_Event *event)
{
    Edbg(("reshape_display(event=%p)", event));

    float ratio = (float)g_width / (float) g_height;

    g_height = event->resize.h;
    g_width = g_height * ratio;

    g_screen = SDL_SetVideoMode(g_width, g_height, 0, DISPLAY_MODE);
    glViewport(0, 0, g_width, g_height);

    Rdbg(("reshape_display"));
}

/**
 * Détruire l'affichage: nettoyage
 */
void 
destroy_display(void)
{
    Edbg(("destroy_display()"));

    g_done = 1;
    glDeleteLists(g_scene, 1);
    SDL_Quit();

    Rdbg(("destroy_display"));
}

/**
 * Analyse les mouvements de la souris
 * @param event pointeur sur l'objet des évènements SDL
 */
void 
mouse_event(SDL_Event *event)
{
    Uint8 state;

    /* on récupère l'état de la souris */
    state = SDL_GetMouseState(NULL, NULL);

    /* bouton droit enfoncé? */
    if (state & SDL_BUTTON(3))
    {
        /* bouton gauche enfoncé = zoom */
        if (state & SDL_BUTTON(1))
        {
            g_zoom += (event->motion.xrel)/2.0;
        }
        /* sinon rotation */
        else
        {
            g_rot_x = g_rot_x + event->motion.yrel;
            g_rot_y = g_rot_y + event->motion.xrel;
        }
    }
    else if (state & SDL_BUTTON(1))
    {
        g_translate_z += (event->motion.xrel)/2.0;
    }
}

/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
