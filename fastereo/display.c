/*
 * File:     $RCSfile: display.c,v $
 * Author:   Jean-Fran�ois LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/05/03 14:15:15 $
 * Version:  $Revision: 1.7 $
 * ID:       $Id: display.c,v 1.7 2004/05/03 14:15:15 arutha Exp $
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

/** Variable globale repr�sentant la largeur de la fen�tre */
unsigned int g_width = WIN_SIZE_X;
/** Variable globale repr�sentant la hauteur de la fen�tre */
unsigned int g_height = WIN_SIZE_Y;
/** Variable indiquant le zoom � effectuer sur la fen�tre, par rapport aux
 * images charg�es */
float g_zoomim = ZOOM;
/** taille d'un pixel: en fonction du zoom appliqu� */
GLfloat g_point_size = 1.0;

/** objet display list pour contenir la sc�ne statique */
GLuint g_scene;

/** bool�en qui indique quand il faut sortir de la boucle SDL */
char g_done = FALSE;
/** objet repr�sentant la fen�tre SDL */
SDL_Surface *g_screen = NULL;

/** Indique si l'animation est activ�e */
char g_animate = FALSE;
/** Nombre de keyframes dans l'animation, plus la valeur est petite, plus
 * l'animation est rapide */
int g_anim_nb_keyframes = ANIM_NB_KEYFRAMES;
/** Keyframe actif dans l'animation */
int g_anim_keyframe = 0;
/** Position de la premi�re cam�ra pour l'animation */
float g_anim_first_pos;
/** Position de la derni�re cam�ra pour l'animation */
float g_anim_last_pos;

/** Indique si on est en plein �cran */
char g_fullscreen = FALSE;
/** Indique si les teintes des images sont activ�es */
char g_shading = TRUE;
/** Indique le mode s�lectionn�: triangles ou carr�s */
char g_mode = 0;
/** Indique si on affiche les cartes de profondeurs � la place des image */
char g_depthmaps = FALSE;

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
        fprintf(stderr, "Impossible de cr�er la fen�tre SDL: %s\n", 
                SDL_GetError());
        exit(3);
    }
    SDL_WM_SetCaption(WIN_TITLE, NULL);
    SDL_ShowCursor(SDL_DISABLE);

    init_gl();

    /* on r�cup�re les positions extr�mes de la cam�ra pour l'animation */
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
                    /* �v�nements du clavier */
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

    glEnable (GL_POINT_SMOOTH);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint (GL_POINT_SMOOTH_HINT, GL_DONT_CARE);
    glPointSize(1.0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* on construit la sc�ne statique */
    construct_scene(FALSE);

    Rdbg(("init_gl"));
}

/**
 * Construit la sc�ne statique
 * @param destroyDL d�truire la display list avant de la construire � nouveau
 */
void 
construct_scene(char destroyDL)
{
    Edbg(("construct_scene(destroyDL=%s)", destroyDL?"TRUE":"FALSE"));

    Camera_t *pcam = g_cameras.root;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);

    glPointSize(g_point_size);

    /* on parcourt toutes les cam�ras */
    while (NULL != pcam)
    {
        /* printf("id: %d\n", pcam->id); */

        /* on supprime la display list pr�c�dente */
        if (TRUE == destroyDL)
        {
            glDeleteLists(pcam->idGL, 1);
        }

        /* on compile l'affichage dans la display list */
        pcam->idGL = glGenLists(1);
        glNewList(pcam->idGL, GL_COMPILE);
        display_cam(pcam);
        glEndList();

        /* on passe � la cam�ra suivante */
        pcam = pcam->next;
    }

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    Rdbg(("construct_scene"));
}

/**
 * Affiche l'image 3D d'une cam�ra
 * @param pcam pointeur sur la cam�ra
 */
void 
display_cam(Camera_t *pcam)
{
    Edbg(("display_cam()"));

    int i, j;
    int width;
    int height;
    float d;
    int nb_colors;
    imginfo *labels = &(pcam->labels);
    GLfloat *pshade = pcam->shade;
    unsigned int ind;
    GLfloat *vertices = NULL;
    GLfloat *colors = NULL;
    int size;

    width = pcam->ii.XSize;
    height = pcam->ii.YSize;
    nb_colors = pcam->ii.ZSize;
    Dprintf((1,"cam�ra n�%d\n", pcam->id));
    Dprintf((1,"width: %d\n", width));
    Dprintf((1,"height: %d\n", height));
    Dprintf((1,"shade: (%.3f, %.3f, %.3f, %.3f)\n", 
             pcam->shade[0], pcam->shade[1], pcam->shade[2], pcam->shade[3]));
    Dprintf((1,"g_nb_labels: %d\n", g_nb_labels));

    /* on r�serve la m�moire */
    size = pcam->ii.XSize * pcam->ii.YSize;
    vertices = (GLfloat *) malloc(3*size*sizeof(GLfloat));
    colors = (GLfloat *) malloc(3*size*sizeof(GLfloat));

    /* on remplit les vertex arrays */
    /* on se d�place sur les y */
    for (j=0; j<height; j++)
    {
        /* on se d�place sur les x */
        for (i=0; i<width; i++)
        {
            ind = (j*width + i)*3;
            d = (labels->Data[j*labels->XSize + i]) / (float)(g_nb_labels-1);
            img_get_color(&(colors[ind]), pcam, i , j , 0.0, g_depthmaps);
            if (g_shading)
            {
                colors[ind] *= pshade[0];
                colors[ind+1] *= pshade[1];
                colors[ind+2] *= pshade[2];
            }
            vertices[ind] = i;
            vertices[ind+1] = j;
            vertices[ind+2] = d;
        }
    }

    glColorPointer(3, GL_FLOAT, 0, colors);
    glVertexPointer(3, GL_FLOAT, 0, vertices);

    switch (g_mode)
    {
        case 0:
            glBegin(GL_POINTS);
            /* on se d�place sur les y */
            for (j=0; j<height; j++)
            {
                /* on se d�place sur les x */
                for (i=0; i<width; i++)
                {
                    glArrayElement(j*width+i);
                }
            }
            glEnd();
            break;
        case 1:
            glBegin(GL_QUADS);
            /* on se d�place sur les y */
            for (j=0; j<height; j++)
            {
                /* on se d�place sur les x */
                for (i=0; i<width; i++)
                {
                    ind = (j*width + i)*3;
                    d = vertices[ind+2];
                    glColor3fv(&(colors[ind]));
                    glVertex3f(i-0.5, j-0.5, d);
                    glVertex3f(i+0.5, j-0.5, d);
                    glVertex3f(i+0.5, j+0.5, d);
                    glVertex3f(i-0.5, j+0.5, d);
                }
            }
            glEnd();
            break;
        case 2:
            glBegin(GL_TRIANGLES);
            /* on se d�place sur les y */
            for (j=0; j<height-1; j++)
            {
                /* on se d�place sur les x */
                for (i=0; i<width-1; i++)
                {
                    glArrayElement(j*width + i);
                    glArrayElement(j*width + (i+1));
                    glArrayElement((j+1)*width + (i+1));

                    glArrayElement(j*width + i);
                    glArrayElement((j+1)*width + (i+1));
                    glArrayElement((j+1)*width + i);
                }
            }
            glEnd();
            break;
    }

    /* nettoyage */
    free(vertices);
    free(colors);

    Rdbg(("display_cam"));
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

    glPointSize(g_point_size);

    /* on parcourt les cam�ras */
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

            /* on affiche la cam�ra */
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glCallList(pcam->idGL);
        }

        /* cam�ra suivante */
        pcam = pcam->next;
    }

    SDL_GL_SwapBuffers();
    /* SDL_Flip(g_screen); */
}

/**
 * Redimensionne l'affichage OpenGL
 * @param event pointeur sur l'objet des �v�nements SDL
 */
void 
reshape_display(SDL_Event *event)
{
    Edbg(("reshape_display(event=%p)", event));

    float ratio = (float)g_width / (float) g_height;
    Dprintf((1,"ratio: %f\n", ratio));

    g_height = event->resize.h;
    g_width = g_height * ratio;

    g_point_size = (float)g_width / (float)(g_cameras.root->ii.XSize);
    Dprintf((1,"g_point_size: %f\n", g_point_size));

    g_screen = SDL_SetVideoMode(g_width, g_height, 0, DISPLAY_MODE);
    glViewport(0, 0, g_width, g_height);

    Rdbg(("reshape_display"));
}

/**
 * D�truire l'affichage: nettoyage
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
 * @param event pointeur sur l'objet des �v�nements SDL
 */
void 
mouse_event(SDL_Event *event)
{
    Uint8 state;

    /* on r�cup�re l'�tat de la souris */
    state = SDL_GetMouseState(NULL, NULL);

    /* si bouton droit enfonc� */
    if (state & SDL_BUTTON(3))
    {
    }
    /* si bouton gauche enfonc� */
    else if (state & SDL_BUTTON(1))
    {
    }
}

/**
 * Analyse l'activit� du clavier
 * @param event pointeur sur l'objet des �v�nements SDL
 */
void 
keyboard_event(SDL_Event *event)
{
    Camera_t *cam;

    /* quitter l'application si touche escape enfonc�e */
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
            printf("Arr�t de l'animation!\n");
        }
        else
        {
            g_animate = TRUE;
            /* g_anim_keyframe = 0; */
            printf("Animation...\n");
        }
    }
    else if (SDLK_s == event->key.keysym.sym)
    {
        if (TRUE == g_shading)
        {
            g_shading = FALSE;
            printf("Sc�ne sans les teintes... ");
            construct_scene(TRUE);
            printf("[done]\n");
        }
        else
        {
            g_shading = TRUE;
            printf("Sc�ne avec les teintes... ");
            construct_scene(TRUE);
            printf("[done]\n");
        }
    }
    else if (SDLK_m == event->key.keysym.sym)
    {
        g_mode = (g_mode+1)%3;
        switch (g_mode)
        {
            case 0:
                glEnable (GL_POINT_SMOOTH);
                glEnable (GL_BLEND);
                printf("Sc�ne avec les points... ");
                break;
            case 1:
                glDisable (GL_POINT_SMOOTH);
                glDisable (GL_BLEND);
                printf("Sc�ne avec les carr�s... ");
                break;
            case 2:
                glDisable (GL_POINT_SMOOTH);
                glDisable (GL_BLEND);
                printf("Sc�ne avec les triangles... ");
                break;
        }
        construct_scene(TRUE);
        printf("[done]\n");
    }
    else if (SDLK_d == event->key.keysym.sym)
    {
        if (TRUE == g_depthmaps)
        {
            g_depthmaps = FALSE;
            printf("Sc�ne avec les images... ");
            construct_scene(TRUE);
            printf("[done]\n");
        }
        else
        {
            g_depthmaps = TRUE;
            printf("Sc�ne avec les cartes de profondeurs... ");
            construct_scene(TRUE);
            printf("[done]\n");
        }
    }
    else if (SDLK_F1 == event->key.keysym.sym)
    {
        cam = get_camera(1);
        if (NULL == cam)
        {
            fprintf(stderr, "La cam�ra n�1 n'existe pas!\n");
        }
        else
        {
            cam->display = !(cam->display);
            if (TRUE == cam->display)
            {
                printf("Affichage de la cam�ra n�1\n");
            }
            else
            {
                printf("Cam�ra n�1 cach�e!\n");
            }
        }
    }
    else if (SDLK_F2 == event->key.keysym.sym)
    {
        cam = get_camera(2);
        if (NULL == cam)
        {
            fprintf(stderr, "La cam�ra n�2 n'existe pas!\n");
        }
        else
        {
            cam->display = !(cam->display);
            if (TRUE == cam->display)
            {
                printf("Affichage de la cam�ra n�2\n");
            }
            else
            {
                printf("Cam�ra n�2 cach�e!\n");
            }
        }
    }
    else if (SDLK_F3 == event->key.keysym.sym)
    {
        cam = get_camera(3);
        if (NULL == cam)
        {
            fprintf(stderr, "La cam�ra n�3 n'existe pas!\n");
        }
        else
        {
            cam->display = !(cam->display);
            if (TRUE == cam->display)
            {
                printf("Affichage de la cam�ra n�3\n");
            }
            else
            {
                printf("Cam�ra n�3 cach�e!\n");
            }
        }
    }
    else if (SDLK_F4 == event->key.keysym.sym)
    {
        cam = get_camera(4);
        if (NULL == cam)
        {
            fprintf(stderr, "La cam�ra n�4 n'existe pas!\n");
        }
        else
        {
            cam->display = !(cam->display);
            if (TRUE == cam->display)
            {
                printf("Affichage de la cam�ra n�4\n");
            }
            else
            {
                printf("Cam�ra n�4 cach�e!\n");
            }
        }
    }
    else if (SDLK_F5 == event->key.keysym.sym)
    {
        cam = get_camera(5);
        if (NULL == cam)
        {
            fprintf(stderr, "La cam�ra n�5 n'existe pas!\n");
        }
        else
        {
            cam->display = !(cam->display);
            if (TRUE == cam->display)
            {
                printf("Affichage de la cam�ra n�5\n");
            }
            else
            {
                printf("Cam�ra n�5 cach�e!\n");
            }
        }
    }
    else if (SDLK_F6 == event->key.keysym.sym)
    {
        cam = get_camera(6);
        if (NULL == cam)
        {
            fprintf(stderr, "La cam�ra n�6 n'existe pas!\n");
        }
        else
        {
            cam->display = !(cam->display);
            if (TRUE == cam->display)
            {
                printf("Affichage de la cam�ra n�6\n");
            }
            else
            {
                printf("Cam�ra n�6 cach�e!\n");
            }
        }
    }
    else if (SDLK_F7 == event->key.keysym.sym)
    {
        cam = get_camera(7);
        if (NULL == cam)
        {
            fprintf(stderr, "La cam�ra n�7 n'existe pas!\n");
        }
        else
        {
            cam->display = !(cam->display);
            if (TRUE == cam->display)
            {
                printf("Affichage de la cam�ra n�7\n");
            }
            else
            {
                printf("Cam�ra n�7 cach�e!\n");
            }
        }
    }
    else if (SDLK_F8 == event->key.keysym.sym)
    {
        cam = get_camera(8);
        if (NULL == cam)
        {
            fprintf(stderr, "La cam�ra n�8 n'existe pas!\n");
        }
        else
        {
            cam->display = !(cam->display);
            if (TRUE == cam->display)
            {
                printf("Affichage de la cam�ra n�8\n");
            }
            else
            {
                printf("Cam�ra n�8 cach�e!\n");
            }
        }
    }
    else if (SDLK_F9 == event->key.keysym.sym)
    {
        cam = get_camera(9);
        if (NULL == cam)
        {
            fprintf(stderr, "La cam�ra n�9 n'existe pas!\n");
        }
        else
        {
            cam->display = !(cam->display);
            if (TRUE == cam->display)
            {
                printf("Affichage de la cam�ra n�9\n");
            }
            else
            {
                printf("Cam�ra n�9 cach�e!\n");
            }
        }
    }
    else if (SDLK_F10 == event->key.keysym.sym)
    {
        cam = get_camera(10);
        if (NULL == cam)
        {
            fprintf(stderr, "La cam�ra n�10 n'existe pas!\n");
        }
        else
        {
            cam->display = !(cam->display);
            if (TRUE == cam->display)
            {
                printf("Affichage de la cam�ra n�10\n");
            }
            else
            {
                printf("Cam�ra n�10 cach�e!\n");
            }
        }
    }
    else if (SDLK_F11 == event->key.keysym.sym)
    {
        cam = get_camera(11);
        if (NULL == cam)
        {
            fprintf(stderr, "La cam�ra n�11 n'existe pas!\n");
        }
        else
        {
            cam->display = !(cam->display);
            if (TRUE == cam->display)
            {
                printf("Affichage de la cam�ra n�11\n");
            }
            else
            {
                printf("Cam�ra n�11 cach�e!\n");
            }
        }
    }
    else if (SDLK_F12 == event->key.keysym.sym)
    {
        cam = get_camera(12);
        if (NULL == cam)
        {
            fprintf(stderr, "La cam�ra n�12 n'existe pas!\n");
        }
        else
        {
            cam->display = !(cam->display);
            if (TRUE == cam->display)
            {
                printf("Affichage de la cam�ra n�12\n");
            }
            else
            {
                printf("Cam�ra n�12 cach�e!\n");
            }
        }
    }
}

/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
