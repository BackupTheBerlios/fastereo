/*
 * File:     $RCSfile: display.c,v $
 * Author:   Jean-François LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/04/19 18:59:29 $
 * Version:  $Revision: 1.4 $
 * ID:       $Id: display.c,v 1.4 2004/04/19 18:59:29 arutha Exp $
 * Comments:
 */
/**
 * @file display.c
 * @brief Gestion de l'affichage OpenGL.
 */

#include <stdlib.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "SDL.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "dbg.h"
#include "utils.h"
#include "display.h"
#include "geom.h"
#include "cameras.h"

/** Variable globale représentant la position de la caméra */
Point3d_t g_eye_pos = EYE_POS;
/** Variable globale représentant le point de référence */
Point3d_t g_ref_point = REF_POINT;
/** Variable globale représentant le vecteur UP de la caméra */
Vector3d_t g_eye_up = EYE_UP;
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
char g_done = FALSE;
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

/** Indique si l'animation est activée */
char g_animate = FALSE;
/** Nombre de keyframes dans l'animation, plus la valeur est petite, plus
 * l'animation est rapide */
int g_anim_nb_keyframes = ANIM_NB_KEYFRAMES;
/** Keyframe actif dans l'animation */
int g_anim_keyframe = 0;
float g_anim_first_pos;
float g_anim_last_pos;

/** Indique si on est en plein écran */
char g_fullscreen = FALSE;
/** Couleur blanche */
GLfloat g_white_color[] = {1.0, 1.0, 1.0, 1.0};
/** Couleur rouge */
GLfloat g_red_color[] = {1.0, 0.0, 0.0, 1.0};

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

    /* on récupère les positions extrèmes de la caméra pour l'animation */
    g_anim_first_pos = g_cameras.root->position;
    g_anim_last_pos = g_cameras.last->position;

    Dprintf((1,"g_animate: %s\n", g_animate?"TRUE":"FALSE"));
    Dprintf((1,"g_anim_nb_keyframes: %d\n", g_anim_nb_keyframes));
    Dprintf((1,"g_anim_keyframe: %d\n", g_anim_keyframe));
    Dprintf((1,"g_anim_first_pos: %f\n", g_anim_first_pos));
    Dprintf((1,"g_anim_last_pos: %f\n", g_anim_last_pos));

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
                    else if (SDLK_a == event.key.keysym.sym)
                    {
                        if (TRUE == g_animate)
                        {
                            g_animate = FALSE;
                            Dprintf((1,"Animate finished!\n"));
                        }
                        else
                        {
                            g_animate = TRUE;
                            g_anim_keyframe = 0;
                            Dprintf((1,"Animate...\n"));
                        }
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

    float hscreen = 1.0;
    float wscreen = (float)g_width / (float)g_height;
    float hh = hscreen/2.0;
    float hw = wscreen/2.0;

    glViewport(0, 0, g_width, g_height);
    glClearColor(BG_COLOR);
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_SMOOTH);

    Dprintf((1,"g_eye_pos (%.3f, %.3f, %.3f)\n", 
             g_eye_pos.x, g_eye_pos.y, g_eye_pos.z));
    Dprintf((1,"g_ref_point (%.3f, %.3f, %.3f)\n", 
             g_ref_point.x, g_ref_point.y, g_ref_point.z));
    Dprintf((1,"g_eye_up (%.3f, %.3f, %.3f)\n", 
             g_eye_up.x, g_eye_up.y, g_eye_up.z));

    /* glMatrixMode(GL_MODELVIEW); */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    /* gluPerspective(EYE_THETA, (float)g_width/(float)g_height, */
    /*                NEAR_PLANE, FAR_PLANE);                    */
    glFrustum(-hw, hw, -hh, hh, NEAR_PLANE, FAR_PLANE);
    gluLookAt(g_eye_pos.x, g_eye_pos.y, g_eye_pos.z,
              g_ref_point.x, g_ref_point.y, g_ref_point.z,
              g_eye_up.x, g_eye_up.y, g_eye_up.z);

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

    float hscreen = 1.0;
    float wscreen = (float)g_width / (float)g_height;
    float step_x = wscreen / (g_cameras.root->ii.XSize);
    float step_y = hscreen / (g_cameras.root->ii.YSize);
    Camera_t *pcam = g_cameras.root;

    Dprintf((1,"step_x: %.6f\n", step_x));
    Dprintf((1,"step_y: %.6f\n", step_y));
    Dprintf((1,"hscreen: %.6f\twscreen: %.6f\n", hscreen, wscreen));

    g_scene = glGenLists(1);
    glLoadIdentity();

    glNewList(g_scene, GL_COMPILE);
    {
        glBegin(GL_TRIANGLES);
        
        /* on parcourt toutes les caméras */
        while (NULL != pcam)
        {
            /* on affiche l'image associée à la caméra courante */
            display_cam(pcam, hscreen, wscreen, step_x, step_y);

            /* on passe à la caméra suivante */
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
 * Affiche l'image 3D d'une caméra
 */
void 
display_cam(Camera_t *pcam,
            float hscreen,
            float wscreen,
            float step_x,
            float step_y)
{
    Edbg(("display_cam()"));

    int i, j;
    int width;
    int height;
    Point3d_t p1, p2, p3, p4, p5;
    Color_t color_p1, color_p2, color_p3, color_p4, color_p5;
    int nb_colors;
    float step_z;
    Point3d_t point;
    unsigned char *labels = pcam->labels.Data;
    Point3d_t eye = g_eye_pos;
    Color_t *ptint = &(pcam->tint);

    /* on calcule la position de la caméra */
    compute_cam_position(pcam);

    point.z = eye.z - NEAR_PLANE;
    eye.x += pcam->position;

    point.y = -hscreen/2;
    width = pcam->ii.XSize;
    height = pcam->ii.YSize;
    nb_colors = pcam->ii.ZSize;
    /* step_z = pcam->range / pcam->nb_labels; */
    step_z = STEP_Z;
    Dprintf((1,"caméra n°%d\n", pcam->id));
    Dprintf((1,"width: %d\n", width));
    Dprintf((1,"height: %d\n", height));
    Dprintf((1,"step_z: %.3f\n", step_z));
    Dprintf((1,"position: %.9f\n", eye.x));
    Dprintf((1,"tint: (%.3f, %.3f, %.3f, %.3f)\n", 
             pcam->tint[0], pcam->tint[1], pcam->tint[2], pcam->tint[3]));

    /* on se déplace sur les y */
    for (i=0; i<height-1; i++)
    {
        /* on va au début de la ligne suivante */
        point.x = eye.x - wscreen/2;

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
            p5 = point; p5.x += step_x/2.0; p5.y += step_y/2.0;

            /* on place les points à la distance correspondant à leur
             * étiquette */
            deprojGL_point(&p1, &eye, step_z, labels[i*width+j]);
            deprojGL_point(&p2, &eye, step_z, labels[i*width+(j+1)]);
            deprojGL_point(&p3, &eye, step_z, labels[(i+1)*width+j]);
            deprojGL_point(&p4, &eye, step_z, labels[(i+1)*width+(j+1)]);
            deprojGL_point(&p5, &eye, step_z, 
                         InterpoleImg(j+0.5, i+0.5, 0, &(pcam->labels)));

            /* on récupère les couleurs des points */
            img_get_color(&color_p1, pcam, i, j, 0.0);
            img_get_color(&color_p2, pcam, i, j+1, 0.0);
            img_get_color(&color_p3, pcam, i+1, j, 0.0);
            img_get_color(&color_p4, pcam, i+1, j+1, 0.0);
            img_get_color(&color_p5, pcam, i, j, 0.5);

            set_color(&color_p5, ptint);
            glVertex3f(p5.x, p5.y, p5.z);
            set_color(&color_p1, ptint);
            glVertex3f(p1.x, p1.y, p1.z);
            set_color(&color_p2, ptint);
            glVertex3f(p2.x, p2.y, p2.z);

            set_color(&color_p5, ptint);
            glVertex3f(p5.x, p5.y, p5.z);
            set_color(&color_p2, ptint);
            glVertex3f(p2.x, p2.y, p2.z);
            set_color(&color_p4, ptint);
            glVertex3f(p4.x, p4.y, p4.z);

            set_color(&color_p5, ptint);
            glVertex3f(p5.x, p5.y, p5.z);
            set_color(&color_p4, ptint);
            glVertex3f(p4.x, p4.y, p4.z);
            set_color(&color_p3, ptint);
            glVertex3f(p3.x, p3.y, p3.z);

            set_color(&color_p5, ptint);
            glVertex3f(p5.x, p5.y, p5.z);
            set_color(&color_p3, ptint);
            glVertex3f(p3.x, p3.y, p3.z);
            set_color(&color_p1, ptint);
            glVertex3f(p1.x, p1.y, p1.z);

            /* on passe au point suivant sur la ligne */
            point.x += step_x;
        }

        /* on passe à la ligne suivante */
        point.y += step_y;
    }

    Rdbg(("display_cam"));
}

/**
 * Applique une couleur
 */
void set_color(Color_t *color, Color_t *tint)
{
    GLfloat c[4];

    c[0] = (*color)[0] * (*tint)[0];
    c[1] = (*color)[1] * (*tint)[1];
    c[2] = (*color)[2] * (*tint)[2];
    c[3] = (*color)[3] * (*tint)[3];

    glColor4fv(c);
}


/**
 * Raffraichit l'affichage OpenGL
 */
void 
refresh_display(void)
{
    float cam_pos = 0;;

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    /* animation */
    if (TRUE == g_animate)
    {
        cam_pos = (float)g_anim_keyframe/(float)g_anim_nb_keyframes 
            * (g_anim_last_pos - g_anim_first_pos);
        g_anim_keyframe = (g_anim_keyframe+1) % (g_anim_nb_keyframes+1);
    }

    glLoadIdentity();

    glTranslatef(0, 0, -g_zoom);
    glRotatef(g_rot_x, 1.0, 0.0, 0.0);
    glRotatef(g_rot_y, 0.0, 1.0, 0.0);
    glTranslatef(0, 0, -g_translate_z);
    glTranslatef(-cam_pos, 0, 0);
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
    /* si bouton gauche enfoncé tout seul, alors translation sur l'axe des Z */
    else if (state & SDL_BUTTON(1))
    {
        g_translate_z += (event->motion.xrel)/2.0;
    }
}

/**
 * Calcule la position de la caméra à partir des informations présentes dans le
 * fichier *.cameras
 */
int
compute_cam_position(Camera_t *cam)
{
    Edbg(("compute_cam_position(cam=%p", cam));

    Camera_t *root = g_cameras.root;
    Point3d_t p1, p2, p3, pr;
    float hscreen = 1.0;
    float wscreen = (float)g_width / (float)g_height;
    float step_x = wscreen / (g_cameras.root->ii.XSize);
    float step_y = hscreen / (g_cameras.root->ii.YSize);
    unsigned char *labels = root->labels.Data;
    /* float step_z = cam->range / cam->nb_labels; */
    float step_z = STEP_Z;
    int width = root->ii.XSize;
    Vector3d_t vdir;
    float t;
    char label;
    float depth;
    Point2d_t middle;
    Point2d_t middle_proj;
    Point3d_t ptw;

    if (cam == root)
    {
        /* root->position = 0.130208343; */
        root->position = 0.0;
        Rdbg(("compute_cam_position RETURN_SUCCESS"));
        return RETURN_SUCCESS;
    }

    Dprintf((1,"hscreen: %f\n", hscreen));
    Dprintf((1,"wscreen: %f\n", wscreen));
    Dprintf((1,"step_x: %f\n", step_x));
    Dprintf((1,"step_y: %f\n", step_y));
    Dprintf((1,"step_z: %f\n", step_z));
    Dprintf((1,"width: %d\n", width));

    middle.x = (int)(root->ii.XSize / 2.0);
    middle.y = (int)(root->ii.YSize / 2.0);
    middle.w = 1.0;
    label = labels[(int)(middle.y*width+middle.x)];

    Dprintf((1,"middle: (%f,%f,%f)\n", middle.x, middle.y, middle.w));
    Dprintf((1,"label: %d\n", label));

    /* on retrouve la profondeur qui correspond à cette étiquette */
    depth = label2depth(root, label);

    Dprintf((1,"depth: %f\n", depth));

    /* on retrouve le point dans le monde qui correspond */
    ptw = deproj(root, middle, depth);

    Dprintf((1,"ptw: (%f,%f,%f,%f)\n", ptw.x, ptw.y, ptw.z, ptw.w));

    /* on projette ce point dans la nouvelle caméra */
    middle_proj = proj(cam, ptw);
    Dprintf((1,"middle_proj: (%f,%f)\n", middle_proj.x, middle_proj.y, 
             middle_proj.w));

    p1.z = g_eye_pos.z - NEAR_PLANE;
    p1.x = g_eye_pos.x - wscreen/2.0;
    p1.y = g_eye_pos.y - hscreen/2.0;
    p1.x += middle.x * step_x;
    p1.y += middle.y * step_y;
    p2 = p1;
    p2.x -= (middle_proj.x - middle.x) * step_x;
    p3 = p1;
    deprojGL_point(&p3, &g_eye_pos, step_z, label);
    vdir.x = p2.x - p3.x;
    vdir.y = p2.y - p3.y;
    vdir.z = p2.z - p3.z;
    normalize3d(&vdir);

    /* on cherche l'intersection avec le plan y=0 */
    if (vdir.y > vdir.z)
    {
        t = - p3.y / vdir.y;
        Dprintf((1,"vdir.y > vdir.z\n"));
        Dprintf((1,"%f > %f\n", vdir.y, vdir.z));
    }
    else
    {
        t = - p3.z / vdir.z;
        Dprintf((1,"vdir.y <= vdir.z\n"));
        Dprintf((1,"%f <= %f\n", vdir.y, vdir.z));
    }

    pr.x = p3.x + vdir.x * t;
    pr.y = p3.y + vdir.y * t;
    pr.z = p3.z + vdir.z * t;

    Dprintf((1,"p1: (%.3f, %.3f, %.3f)\n", p1.x, p1.y, p1.z));
    Dprintf((1,"p2: (%.3f, %.3f, %.3f)\n", p2.x, p2.y, p2.z));
    Dprintf((1,"p3: (%.3f, %.3f, %.3f)\n", p3.x, p3.y, p3.z));
    Dprintf((1,"vdir: (%.3f, %.3f, %.3f)\n", vdir.x, vdir.y, vdir.z));
    Dprintf((1,"pr: (%.9f, %.9f, %.9f)\n", pr.x, pr.y, pr.z));

    cam->position = pr.x;

    Rdbg(("compute_cam_position RETURN_SUCCESS"));
    return RETURN_SUCCESS;
}

/**
 * Déprojette un point 3D dans la scène OpenGL
 */
void
deprojGL_point(Point3d_t *point, Point3d_t *eye, float step_z, float label)
{
    Vector3d_t vdir;

    vdir.x = point->x - eye->x;
    vdir.y = point->y - eye->y;
    vdir.z = point->z - eye->z;
    normalize3d(&vdir);

    point->x += label * step_z * vdir.x;
    point->y += label * step_z * vdir.y;
    point->z += label * step_z * vdir.z;
}

/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
