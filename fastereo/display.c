/*
 * File:     $RCSfile: display.c,v $
 * Author:   Jean-François LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/04/26 20:24:40 $
 * Version:  $Revision: 1.5 $
 * ID:       $Id: display.c,v 1.5 2004/04/26 20:24:40 arutha Exp $
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

#include <SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "dbg.h"
#include "utils.h"
#include "display.h"
#include "geom.h"
#include "cameras.h"

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

/** Indique si l'animation est activée */
char g_animate = FALSE;
/** Nombre de keyframes dans l'animation, plus la valeur est petite, plus
 * l'animation est rapide */
int g_anim_nb_keyframes = ANIM_NB_KEYFRAMES;
/** Keyframe actif dans l'animation */
int g_anim_keyframe = 0;
/** Position de la première caméra pour l'animation */
float g_anim_first_pos;
/** Position de la dernière caméra pour l'animation */
float g_anim_last_pos;

/** Indique si on est en plein écran */
char g_fullscreen = FALSE;
/** Indique si les teintes des images sont activées */
char g_shading = TRUE;
/** Indique le mode sélectionné: triangles ou carrés */
char g_mode = 0;

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
    SDL_ShowCursor(SDL_DISABLE);

    init_gl();

    /* on récupère les positions extrèmes de la caméra pour l'animation */
    g_anim_first_pos = -g_cameras.root->position;
    g_anim_last_pos = -g_cameras.last->position;

    Dprintf((1,"g_animate: %s\n", g_animate?"TRUE":"FALSE"));
    Dprintf((1,"g_anim_nb_keyframes: %d\n", g_anim_nb_keyframes));
    Dprintf((1,"g_anim_keyframe: %d\n", g_anim_keyframe));
    Dprintf((1,"g_anim_first_pos: %f\n", g_anim_first_pos));
    Dprintf((1,"g_anim_last_pos: %f\n", g_anim_last_pos));

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
                    /* évènements du clavier */
                    keyboard_event(&event);
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
    /* glEnable(GL_SMOOTH); */

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* on construit la scène statique */
    construct_scene(FALSE);

    Rdbg(("init_gl"));
}

/**
 * Construit la scène statique
 */
void 
construct_scene(char destroyDL)
{
    Edbg(("construct_scene(destroyDL=%s)", destroyDL?"TRUE":"FALSE"));

    Camera_t *pcam = g_cameras.root;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* on parcourt toutes les caméras */
    while (NULL != pcam)
    {
        /* printf("id: %d\n", pcam->id); */
        if (TRUE == destroyDL)
        {
            glDeleteLists(pcam->idGL, 1);
        }
        pcam->idGL = glGenLists(1);
        glNewList(pcam->idGL, GL_COMPILE);
        display_cam(pcam);
        glEndList();

        /* on passe à la caméra suivante */
        pcam = pcam->next;
    }

    Rdbg(("construct_scene"));
}

/**
 * Affiche l'image 3D d'une caméra
 */
void 
display_cam(Camera_t *pcam)
{
    Edbg(("display_cam()"));

    int i, j;
    int width;
    int height;
    float d1, d2, d3, d4, d5;
    Color_t color_p1, color_p2, color_p3, color_p4, color_p5;
    int nb_colors;
    imginfo *labels = &(pcam->labels);
    Color_t *pshade = &(pcam->shade);

    width = pcam->ii.XSize;
    height = pcam->ii.YSize;
    nb_colors = pcam->ii.ZSize;
    /* step_z = pcam->range / pcam->nb_labels; */
    Dprintf((1,"caméra n°%d\n", pcam->id));
    Dprintf((1,"width: %d\n", width));
    Dprintf((1,"height: %d\n", height));
    Dprintf((1,"shade: (%.3f, %.3f, %.3f, %.3f)\n", 
             pcam->shade[0], pcam->shade[1], pcam->shade[2], pcam->shade[3]));
    Dprintf((1,"g_nb_labels: %d\n", g_nb_labels));

    switch (g_mode)
    {
        case 0:
            glBegin(GL_QUADS);
            /* on se déplace sur les y */
            for (j=0; j<height; j++)
            {
                /* on se déplace sur les x */
                for (i=0; i<width; i++)
                {
                    /* Dprintf((1,"i: %d\tj: %d\n", i, j)); */

                    /* on récupère les profondeurs */
                    d1 = (labels->Data[j    *labels->XSize + i    ]) / (float)(g_nb_labels-1);
                    /* Dprintf((1,"d1:%f\n", d1)); */
                    /* Dprintf((1,"label1: %d\n", labels->Data[j*labels->XSize+i])); */

                    /* on récupère les couleurs des points */
                    img_get_color(&color_p1, pcam, i  , j  , 0.0);
                    /* Dprintf((1,"c1: (%f,%f,%f,%f)\n", color_p1[0], color_p1[1], */
                    /*          color_p1[2], color_p1[3]));                        */

                    set_color(&color_p1, pshade);
                    glVertex3f(i-0.5, j-0.5, d1);
                    set_color(&color_p1, pshade);
                    glVertex3f(i+0.5, j-0.5, d1);
                    set_color(&color_p1, pshade);
                    glVertex3f(i+0.5, j+0.5, d1);
                    set_color(&color_p1, pshade);
                    glVertex3f(i-0.5, j+0.5, d1);
                }
            }
            glEnd();
            break;
        case 1:
            glBegin(GL_TRIANGLES);
            /* on se déplace sur les y */
            for (j=0; j<height-1; j++)
            {
                /* on se déplace sur les x */
                for (i=0; i<width-1; i++)
                {
                    /* Dprintf((1,"i: %d\tj: %d\n", i, j)); */

                    /* on récupère les profondeurs */
                    d1 = (labels->Data[j    *labels->XSize + i    ]) / (float)(g_nb_labels-1);
                    d2 = (labels->Data[j    *labels->XSize + (i+1)]) / (float)(g_nb_labels-1);
                    d3 = (labels->Data[(j+1)*labels->XSize + i    ]) / (float)(g_nb_labels-1);
                    d4 = (labels->Data[(j+1)*labels->XSize + (i+1)]) / (float)(g_nb_labels-1);
                    d5 = (InterpoleImg(i+0.5, j+0.5, 0, labels))     / (float)(g_nb_labels-1);
                    /* Dprintf((1,"d1:%f\td2:%f\td3:%f\td4:%f\td5:%f\n", d1, d2, d3, d4, d5)); */
                    /* Dprintf((1,"label1: %d\n", labels->Data[j*labels->XSize+i]));           */
                    /* Dprintf((1,"label2: %d\n", labels->Data[j*labels->XSize+(i+1)]));       */
                    /* Dprintf((1,"label3: %d\n", labels->Data[(j+1)*labels->XSize+i]));       */
                    /* Dprintf((1,"label4: %d\n", labels->Data[(j+1)*labels->XSize+(i+1)]));   */
                    /* Dprintf((1,"label5: %f\n", InterpoleImg(i+0.5, j+0.5, 0, labels)));     */

                    /* on récupère les couleurs des points */
                    img_get_color(&color_p1, pcam, i  , j  , 0.0);
                    img_get_color(&color_p2, pcam, i+1, j  , 0.0);
                    img_get_color(&color_p3, pcam, i  , j+1, 0.0);
                    img_get_color(&color_p4, pcam, i+1, j+1, 0.0);
                    img_get_color(&color_p5, pcam, i  , j  , 0.5);
                    /* Dprintf((1,"c1: (%f,%f,%f,%f)\n", color_p1[0], color_p1[1], */
                    /*          color_p1[2], color_p1[3]));                        */
                    /* Dprintf((1,"c2: (%f,%f,%f,%f)\n", color_p2[0], color_p2[1], */
                    /*          color_p2[2], color_p2[3]));                        */
                    /* Dprintf((1,"c3: (%f,%f,%f,%f)\n", color_p3[0], color_p3[1], */
                    /*          color_p3[2], color_p3[3]));                        */
                    /* Dprintf((1,"c4: (%f,%f,%f,%f)\n", color_p4[0], color_p4[1], */
                    /*          color_p4[2], color_p4[3]));                        */
                    /* Dprintf((1,"c5: (%f,%f,%f,%f)\n", color_p5[0], color_p5[1], */
                    /*          color_p5[2], color_p5[3]));                        */

                    /* set_color(&color_p1, pshade); */
                    /* glVertex3f(i, j, d1);         */
                    /* set_color(&color_p2, pshade); */
                    /* glVertex3f(i+1, j, d2);       */
                    /* set_color(&color_p4, pshade); */
                    /* glVertex3f(i+1, j+1, d4);     */

                    /* set_color(&color_p1, pshade); */
                    /* glVertex3f(i, j, d1);         */
                    /* set_color(&color_p4, pshade); */
                    /* glVertex3f(i+1, j+1, d4);     */
                    /* set_color(&color_p3, pshade); */
                    /* glVertex3f(i, j+1, d3);       */

                    set_color(&color_p5, pshade);
                    glVertex3f(i+0.5, j+0.5, d5);
                    set_color(&color_p1, pshade);
                    glVertex3f(i, j, d1);
                    set_color(&color_p2, pshade);
                    glVertex3f(i+1, j, d2);

                    set_color(&color_p5, pshade);
                    glVertex3f(i+0.5, j+0.5, d5);
                    set_color(&color_p2, pshade);
                    glVertex3f(i+1, j, d2);
                    set_color(&color_p4, pshade);
                    glVertex3f(i+1, j+1, d4);

                    set_color(&color_p5, pshade);
                    glVertex3f(i+0.5, j+0.5, d5);
                    set_color(&color_p4, pshade);
                    glVertex3f(i+1, j+1, d4);
                    set_color(&color_p3, pshade);
                    glVertex3f(i, j+1, d3);

                    set_color(&color_p5, pshade);
                    glVertex3f(i+0.5, j+0.5, d5);
                    set_color(&color_p3, pshade);
                    glVertex3f(i, j+1, d3);
                    set_color(&color_p1, pshade);
                    glVertex3f(i, j, d1);
                }
            }
            glEnd();
            break;
    }

    Rdbg(("display_cam"));
}

/**
 * Applique une couleur
 */
void set_color(Color_t *color, Color_t *shade)
{
    GLfloat c[4];

    if (TRUE == g_shading)
    {
        c[0] = (*color)[0] * (*shade)[0];
        c[1] = (*color)[1] * (*shade)[1];
        c[2] = (*color)[2] * (*shade)[2];
        c[3] = (*color)[3] * (*shade)[3];
    }
    else
    {
        c[0] = (*color)[0];
        c[1] = (*color)[1];
        c[2] = (*color)[2];
        c[3] = (*color)[3];
    }

    glColor4fv(c);
}


/**
 * Raffraichit l'affichage OpenGL
 */
void 
refresh_display(void)
{
    float x;
    Camera_t *pcam = g_cameras.root;

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    /* animation */
    if (TRUE == g_animate)
    {
        g_anim_keyframe = (g_anim_keyframe+1) % (g_anim_nb_keyframes+1);
        x = (g_anim_last_pos-g_anim_first_pos)
            * ((float)g_anim_keyframe/(g_anim_nb_keyframes))
            + g_anim_first_pos;
    }

    /* on parcourt les caméras */
    while (NULL != pcam)
    {
        /* printf("pcam->id: %d\n", pcam->id); */
        if (TRUE == pcam->display)
        {
            /* matrice de projection */
            glMatrixMode(GL_PROJECTION);
            if (TRUE == g_animate)
            {
                pcam->mGL[8] =
                    -(g_dmax-g_dmin)*2.0*(pcam->position+x) / pcam->ii.XSize;
            }
            glLoadMatrixf(pcam->mGL);

            /* on affiche la caméra */
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glCallList(pcam->idGL);
        }

        /* caméra suivante */
        pcam = pcam->next;
    }

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
    Dprintf((1,"ratio: %f\n", ratio));

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
    /* Uint8 state;                                                                */

    /* |+ on récupère l'état de la souris +|                                       */
    /* state = SDL_GetMouseState(NULL, NULL);                                      */

    /* |+ bouton droit enfoncé? +|                                                 */
    /* if (state & SDL_BUTTON(3))                                                  */
    /* {                                                                           */
    /*     |+ bouton gauche enfoncé = zoom +|                                      */
    /*     if (state & SDL_BUTTON(1))                                              */
    /*     {                                                                       */
    /*         g_zoom += (event->motion.xrel)/2.0;                                 */
    /*     }                                                                       */
    /*     |+ sinon rotation +|                                                    */
    /*     else                                                                    */
    /*     {                                                                       */
    /*         g_rot_x = g_rot_x + event->motion.yrel;                             */
    /*         g_rot_y = g_rot_y + event->motion.xrel;                             */
    /*     }                                                                       */
    /* }                                                                           */
    /* |+ si bouton gauche enfoncé tout seul, alors translation sur l'axe des Z +| */
    /* else if (state & SDL_BUTTON(1))                                             */
    /* {                                                                           */
    /*     g_translate_z += (event->motion.xrel)/2.0;                              */
    /* }                                                                           */
}

/**
 * Analyse l'activité du clavier
 * @param event pointeur sur l'objet des évènements SDL
 */
void 
keyboard_event(SDL_Event *event)
{
    Camera_t *cam;

    /* quitter l'application si touche escape enfoncée */
    if (SDLK_ESCAPE == event->key.keysym.sym)
    {
        g_done = 1;
    }
    else if (SDLK_z == event->key.keysym.sym)
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
    else if (SDLK_a == event->key.keysym.sym)
    {
        if (TRUE == g_animate)
        {
            g_animate = FALSE;
            printf("Animate finished!\n");
        }
        else
        {
            g_animate = TRUE;
            g_anim_keyframe = 0;
            printf("Animate...\n");
        }
    }
    else if (SDLK_s == event->key.keysym.sym)
    {
        if (TRUE == g_shading)
        {
            g_shading = FALSE;
            printf("Disabling shading...\n");
            construct_scene(TRUE);
            printf("Shading disabled!\n");
        }
        else
        {
            g_shading = TRUE;
            printf("Enabling shading...\n");
            construct_scene(TRUE);
            printf("Shading enabled!\n");
        }
    }
    else if (SDLK_m == event->key.keysym.sym)
    {
        g_mode = (g_mode+1)%2;
        construct_scene(TRUE);
    }
    else if (SDLK_F1 == event->key.keysym.sym)
    {
        cam = get_camera(1);
        if (NULL == cam)
        {
            fprintf(stderr, "La caméra n°1 n'existe pas!\n");
        }
        else
        {
            cam->display = !(cam->display);
        }
    }
    else if (SDLK_F2 == event->key.keysym.sym)
    {
        cam = get_camera(2);
        if (NULL == cam)
        {
            fprintf(stderr, "La caméra n°2 n'existe pas!\n");
        }
        else
        {
            cam->display = !(cam->display);
        }
    }
    else if (SDLK_F3 == event->key.keysym.sym)
    {
        cam = get_camera(3);
        if (NULL == cam)
        {
            fprintf(stderr, "La caméra n°3 n'existe pas!\n");
        }
        else
        {
            cam->display = !(cam->display);
        }
    }
    else if (SDLK_F4 == event->key.keysym.sym)
    {
        cam = get_camera(4);
        if (NULL == cam)
        {
            fprintf(stderr, "La caméra n°4 n'existe pas!\n");
        }
        else
        {
            cam->display = !(cam->display);
        }
    }
    else if (SDLK_F5 == event->key.keysym.sym)
    {
        cam = get_camera(5);
        if (NULL == cam)
        {
            fprintf(stderr, "La caméra n°5 n'existe pas!\n");
        }
        else
        {
            cam->display = !(cam->display);
        }
    }
    else if (SDLK_F6 == event->key.keysym.sym)
    {
        cam = get_camera(6);
        if (NULL == cam)
        {
            fprintf(stderr, "La caméra n°6 n'existe pas!\n");
        }
        else
        {
            cam->display = !(cam->display);
        }
    }
    else if (SDLK_F7 == event->key.keysym.sym)
    {
        cam = get_camera(7);
        if (NULL == cam)
        {
            fprintf(stderr, "La caméra n°7 n'existe pas!\n");
        }
        else
        {
            cam->display = !(cam->display);
        }
    }
    else if (SDLK_F8 == event->key.keysym.sym)
    {
        cam = get_camera(8);
        if (NULL == cam)
        {
            fprintf(stderr, "La caméra n°8 n'existe pas!\n");
        }
        else
        {
            cam->display = !(cam->display);
        }
    }
    else if (SDLK_F9 == event->key.keysym.sym)
    {
        cam = get_camera(9);
        if (NULL == cam)
        {
            fprintf(stderr, "La caméra n°9 n'existe pas!\n");
        }
        else
        {
            cam->display = !(cam->display);
        }
    }
    else if (SDLK_F10 == event->key.keysym.sym)
    {
        cam = get_camera(10);
        if (NULL == cam)
        {
            fprintf(stderr, "La caméra n°10 n'existe pas!\n");
        }
        else
        {
            cam->display = !(cam->display);
        }
    }
    else if (SDLK_F11 == event->key.keysym.sym)
    {
        cam = get_camera(11);
        if (NULL == cam)
        {
            fprintf(stderr, "La caméra n°11 n'existe pas!\n");
        }
        else
        {
            cam->display = !(cam->display);
        }
    }
    else if (SDLK_F12 == event->key.keysym.sym)
    {
        cam = get_camera(12);
        if (NULL == cam)
        {
            fprintf(stderr, "La caméra n°12 n'existe pas!\n");
        }
        else
        {
            cam->display = !(cam->display);
        }
    }
}

/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
