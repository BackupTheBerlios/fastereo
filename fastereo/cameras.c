/*
 * File:     $RCSfile: cameras.c,v $
 * Author:   Jean-François LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/04/13 23:59:49 $
 * Version:  $Revision: 1.1 $
 * ID:       $Id: cameras.c,v 1.1 2004/04/13 23:59:49 arutha Exp $
 * Comments:
 */
/**
 * @file cameras.c
 * @brief Gestion des caméras.
 */

#include <stdlib.h>
#include "dbg.h"
#include "utils.h"
#include "cameras.h"
#include "geom.h"
#include "img.h"

/** variable globale représentant toutes les caméras chargées */
Cameras_t g_cameras;

/**
 * Initialise l'objet qui représente les caméras.
 */
void
init_cameras(void)
{
    g_cameras.nb = 0;
    g_cameras.root = NULL;
    g_cameras.last = NULL;
}

/**
 * Libère la mémoire occupée par la caméra et celles qui suivent dans la liste.
 * Cette fonction s'appelle récursivement.
 */
void 
destroy_camera(Camera_t *camera)
{
    Edbg(("destroy_camera(camera=p)", camera));

    if (NULL != camera)
    {
        if (NULL != camera->next)
        {
            destroy_camera(camera->next);
        }
        FreeImage(&(camera->ii));
        free(camera);
    }

    Rdbg(("destroy_camera"));
}

/**
 * Libère la mémoire occupée par les caméras chargées, puis initialise l'objet
 * représentant les caméras.
 */
void 
destroy_cameras(void)
{
    Edbg(("destroy_cameras()"));

    destroy_camera(g_cameras.root);

    init_cameras();

    Rdbg(("destroy_cameras"));
}

/**
 * Charge des caméras à partir d'un fichier *.cameras
 * @param file_name nom du fichier contenant les définitions des caméras
 * @return différent de 0 si échec
 */
int
load_cameras(const char *file_name)
{
    Edbg(("load_cameras(file_name='%s')", file_name));

    Camera_t *cam = NULL;
    FILE *fd = NULL;
    char buffer[MAX_LLINE_CAMERAS];
    int ret;
    int id;
    float position;
    char image[MAX_LNAME];
    char depth_map[MAX_LNAME];

    /* initialisation */
    g_cameras.root = NULL;
    g_cameras.nb = 0;

    /* ouverture du fichier */
    fd = fopen(file_name, "r");
    if (NULL == fd)
    {
        perror("fopen");
        fprintf(stderr, "Impossible d'ouvrir le fichier %s!\n", file_name);
        Rdbg(("load_cameras RETURN_FAILED"));
        return RETURN_FAILED;
    }

    /* on parcours toutes les lignes du fichier */
    while (fgets(buffer, MAX_LLINE_CAMERAS, fd) != NULL)
    {
        strip_eol(buffer);

        if (!is_comment(buffer))
        {
            Dprintf((1,"buffer: %s\n", buffer));
            /* commande 'cameras' */
            if (strncmp(buffer, "camera ", 7) == 0)
            {
                id = -1;
                position = 9e90; /* position ne peut pas être trop grand */
                image[0] = '\0';
                depth_map[0] = '\0';

                /* on extrait les données */
                ret = sscanf(buffer, "camera %d %f %s %s",
                             &id, &position, image, depth_map);

                if ((ret < 3) || (id < 0) || (position > 8e90) 
                    || (image[0] == '\0'))
                {
                    fprintf(stderr, "Commande non reconnue!! %s\n", buffer);
                }
                else
                {
                    /* on ajoute la caméra */
                    cam = add_camera(id, position, image, (ret>3)?depth_map:NULL);
                    if (NULL == cam)
                    {
                        fprintf(stderr, 
                                "Impossible d'ajouter la caméra '%s'!!\n", 
                                buffer);
                        Rdbg(("load_cameras RETURN_FAILED"));
                        return RETURN_FAILED;
                    }
                    Dprintf((1,"Caméra ajoutée!\n"));
                }
            }
            else
            {
                fprintf(stderr, "Commande non reconnue!! %s\n", buffer);
            }
        }
    }

    /* on ferme le fichier */
    fclose(fd);

    Rdbg(("load_cameras RETURN_SUCCESS"));
    return RETURN_SUCCESS;
}

/**
 * Ajoute une caméra dans la liste
 * @param id identifiant de la caméra
 * @param position position de la caméra
 * @param image nom du fichier de l'image associée
 * @param depth_map nom du fichier de la carte de profondeur associée 
 *                  [NULL si on n'en a pas]
 */
Camera_t *
add_camera(const int id, 
           const float position, 
           const char *image, 
           const char *depth_map)
{
    Edbg(("add_camera(id=%d, position=%.3f, image='%s', depth_map='%s')", 
          id, position, image, depth_map));

    Camera_t *cam;

    if (NULL == g_cameras.root)
    {
        /* si la liste est vide */
        cam = g_cameras.root = (Camera_t *) malloc(sizeof(Camera_t));
    }
    else
    {
        cam = g_cameras.last->next = (Camera_t *) malloc(sizeof(Camera_t));
    }
    if(NULL == cam)
    {
        /* le malloc n'a pas marché */
        Rdbg(("add_camera NULL"));
        return NULL;
    }

    strncpy(cam->img, image, MAX_LNAME);
    if (NULL != depth_map)
    {
        strncpy(cam->dm, depth_map, MAX_LNAME);
    }
    else
    {
        cam->dm[0] = '\0';
    }
    InitImgInfo(&(cam->ii), 256, 256);
    cam->next = NULL;
    cam->id = id;
    load_identity(cam->m);
    cam->m[0][3] = position;
    inv_matrix4(cam->m, cam->mi);

    Rdbg(("add_camera cam=%p", cam));
    return cam;
}

/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
