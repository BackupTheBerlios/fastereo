/*
 * File:     $RCSfile: cameras.c,v $
 * Author:   Jean-Fran�ois LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/04/19 18:59:29 $
 * Version:  $Revision: 1.6 $
 * ID:       $Id: cameras.c,v 1.6 2004/04/19 18:59:29 arutha Exp $
 * Comments:
 */
/**
 * @file cameras.c
 * @brief Gestion des cam�ras.
 */

#include <stdlib.h>
#include <stdio.h>
#include "dbg.h"
#include "utils.h"
#include "cameras.h"
#include "geom.h"
#include "img.h"
#include "display.h"

/** variable globale repr�sentant toutes les cam�ras charg�es */
Cameras_t g_cameras;

extern char g_enable_tint;

/**
 * Initialise l'objet qui repr�sente les cam�ras.
 */
void
init_cameras(void)
{
    Edbg(("init_cameras()"));

    g_cameras.nb = 0;
    g_cameras.root = NULL;
    g_cameras.last = NULL;

    Rdbg(("init_cameras"));
}

/**
 * Lib�re la m�moire occup�e par la cam�ra et celles qui suivent dans la liste.
 * Cette fonction s'appelle r�cursivement.
 */
void 
destroy_camera(Camera_t *camera)
{
    Edbg(("destroy_camera(camera=%p)", camera));

    if (NULL != camera)
    {
        Dprintf((1,"destruction de la cam�ra n�%d\n", camera->id));
        if (NULL != camera->next)
        {
            destroy_camera(camera->next);
        }
        FreeImage(&(camera->ii));
        FreeImage(&(camera->labels));
        free(camera);
    }

    Rdbg(("destroy_camera"));
}

/**
 * Lib�re la m�moire occup�e par les cam�ras charg�es, puis initialise l'objet
 * repr�sentant les cam�ras.
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
 * Charge des cam�ras � partir d'un fichier *.cameras
 * @param file_name nom du fichier contenant les d�finitions des cam�ras
 * @return diff�rent de 0 si �chec
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
    int nb_labels;
    Color_t tint;
    float dmin, dmax, dtmp;

    /* initialisation */
    g_cameras.root = NULL;
    g_cameras.nb = 0;
    tint[3] = 1.0;

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
                image[0] = '\0';
                depth_map[0] = '\0';
                nb_labels = 0;

                /* on extrait les donn�es */
                ret = sscanf(buffer, "camera %d %f %s %s %d %f %f",
                             &id, &position, image, depth_map, &nb_labels,
                             &dmin, &dmax);

                if ((ret == 7) && (dmin > dmax))
                {
                    dtmp = dmin;
                    dmin = dmax;
                    dmax = dtmp;
                }

                Dprintf((1,"Donn�es extraites:\n"));
                Dprintf((1,"id: %d\n", id));
                Dprintf((1,"position: %.3f\n", position));
                Dprintf((1,"image: %s\n", image));
                Dprintf((1,"depth_map: %s\n", depth_map));
                Dprintf((1,"nb_labels: %d\n", nb_labels));
                Dprintf((1,"dmin: %.3f\n", dmin));
                Dprintf((1,"dmax: %.3f\n", dmax));

                if ((ret < 5) || (id < 0) || (image[0] == '\0') 
                    || (nb_labels > 255))
                {
                    fprintf(stderr, "Commande non reconnue!! %s\n", buffer);
                }
                else
                {
                    /* on ajoute la cam�ra */
                    if(ret > 3)
                    {
                        cam = add_camera(id, position, image, depth_map,
                                         (unsigned char)nb_labels, dmin, dmax);
                    }
                    else
                    {
                        cam = add_camera(id, position, image, NULL, 0, 0, 0);
                    }

                    /* erreur? */
                    if (NULL == cam)
                    {
                        fprintf(stderr, 
                                "Impossible d'ajouter la cam�ra '%s'!!\n", 
                                buffer);
                        destroy_cameras();
                        Rdbg(("load_cameras RETURN_FAILED"));
                        return RETURN_FAILED;
                    }
                    Dprintf((1,"Cam�ra ajout�e!\n"));
                }
            }
            /* commande 'tint' */
            else if (strncmp(buffer, "tint ", 5) == 0)
            {
                /* on extrait les donn�es */
                ret = sscanf(buffer, "tint %d %f %f %f",
                             &id, &(tint[0]), &(tint[1]), &(tint[2]));

                if (ret < 4)
                {
                    fprintf(stderr, "Commande non reconnue!! %s\n", buffer);
                }
                else
                {
                    Dprintf((1,"Donn�es extraites:\n"));
                    Dprintf((1,"id: %d\n", id));
                    Dprintf((1,"R: %.3f\n", tint[0]));
                    Dprintf((1,"G: %.3f\n", tint[1]));
                    Dprintf((1,"B: %.3f\n", tint[2]));

                    cam = get_camera (id);

                    /* si la cam�ra n'a pas �t� trouv�e */
                    if (NULL == cam)
                    {
                        fprintf(stderr, "Impossible de trouver la cam�ra n�%d!!\n",
                                id);
                    }
                    else
                    {
                        cam->tint[0] = tint[0];
                        cam->tint[1] = tint[1];
                        cam->tint[2] = tint[2];
                        cam->tint[3] = 1.0;

                        /* g_enable_tint = TRUE; */
                    }
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
 * Ajoute une cam�ra dans la liste
 * @param id identifiant de la cam�ra
 * @param image nom du fichier de l'image associ�e
 * @param depth_map nom du fichier de la carte de profondeurs associ�e 
 *                  [NULL si on n'en a pas]
 * @param nb_labels nombre d'�tiquettes pr�sentes dans la carte de profondeurs
 * @param dmin profondeur minimale
 * @param dmax profondeur maximale
 * @return un pointeur sur la nouvelle cam�ra
 */
Camera_t *
add_camera(const int id, 
           const float position, 
           const char *image, 
           const char *depth_map,
           const unsigned char nb_labels,
           const float dmin,
           const float dmax)
{
    Edbg(("add_camera(id=%d, position=%.3f, image='%s', depth_map='%s',"
          " nb_labels=%d, dmin=%.3f, dmax=%.3f)", 
          id, position, image, depth_map, nb_labels, dmin, dmax));

    Camera_t *cam;
    Camera_t *prev = NULL;

    if (NULL == g_cameras.root)
    {
        /* si la liste est vide */
        cam = g_cameras.root = (Camera_t *) malloc(sizeof(Camera_t));
    }
    else
    {
        prev = g_cameras.last;
        cam = g_cameras.last->next = (Camera_t *) malloc(sizeof(Camera_t));
    }
    if(NULL == cam)
    {
        /* le malloc n'a pas march� */
        Rdbg(("add_camera NULL"));
        return NULL;
    }

    cam->id = id;

    /* on charge l'image */
    if (LoadImage((char *)image, &(cam->ii)))
    {
        /* si le chargement de l'image a �chou�, on d�truit la cam�ra */
        fprintf(stderr, "Impossible de charger l'image %s de la cam�ra %d!\n",
                image, cam->id);
        destroy_camera(cam);
        if (NULL != prev)
        {
            prev->next = NULL;
        }
        else
        {
            g_cameras.root = NULL;
        }
        Rdbg(("add_camera NULL"));
        return NULL;
    }

    /* on charge la carte de profondeurs si elle existe */
    if (NULL != depth_map)
    {
        if (load_depth_map(cam, depth_map, nb_labels))
        {
            /* si le chargement de la carte de profondeurs a �chou�, on d�truit 
             * la cam�ra */
            fprintf(stderr, "Impossible de charger la carte de profondeurs %s"
                    " de la cam�ra %d!\n", depth_map, cam->id);
            destroy_camera(cam);
            if (NULL != prev)
            {
                prev->next = NULL;
            }
            else
            {
                g_cameras.root = NULL;
            }
            Rdbg(("add_camera NULL"));
            return NULL;
        }
    }
    else
    {
        cam->labels.Data = NULL;
        cam->nb_labels = 0;
    }

    cam->next = NULL;
    cam->prev = prev;
    load_identity(cam->m);
    cam->m[0][3] = position;
    inv_matrix4(cam->m, cam->mi);
    cam->dmin = dmin;
    cam->dmax = dmax;

    /* on met la teinte � blanc par d�faut */
    cam->tint[0] = 1.0;
    cam->tint[1] = 1.0;
    cam->tint[2] = 1.0;
    cam->tint[3] = 1.0;

    /* on augmente le nombre de cam�ras */
    g_cameras.nb++;
    /* on met � jour le pointeur sur la derni�re cam�ra */
    g_cameras.last = cam;

    Rdbg(("add_camera cam=%p", cam));
    return cam;
}

/**
 * Charge une carte de profondeurs � partir d'une image.
 * @param cam cam�ra pour laquelle il faut charger la carte de profondeurs
 * @param file_name nom du fichier contenant l'image de la carte de profondeurs
 * @param nb_labels nombre d'�tiquettes dans cette carte de profondeurs
 * @return RETURN_SUCCESS si succ�s, RETURN_FAILED sinon
 */
int 
load_depth_map(Camera_t *cam, const char *file_name, unsigned char nb_labels)
{
    Edbg(("load_depth_map(cam=%p, file_name='%s', nb_labels=%d)",
          cam, file_name, nb_labels));

    int i;
    int L;
    float d;

    /* on charge l'image */
    if (LoadImage((char *)file_name, &(cam->labels)))
    {
        cam->nb_labels = 0;
        Dprintf((1,"LoadImage failed!\n"));
        Rdbg(("load_depth_map RETURN_FAILED"));
        return RETURN_FAILED;
    }
    cam->nb_labels = nb_labels;

    /* on convertit les intensit�s en �tiquettes */
    L = cam->labels.XSize * cam->labels.YSize;
    d = (nb_labels-1.0)/255;
    for (i=0; i<L; i++)
    {
        cam->labels.Data[i] = (int)(cam->labels.Data[i]*d + 0.5);
    }

    Rdbg(("load_depth_map RETURN_SUCCESS"));
    return RETURN_SUCCESS;
}

/**
 * Renvoie la cam�ra dont l'identifiant est id
 * @param id identifiant de la cam�ra
 * @return pointeur sur la cam�ra, NULL si non trouv�e
 */
Camera_t *
get_camera(int id)
{
    Camera_t *pcam = g_cameras.root;

    while (NULL != pcam)
    {
        if (id == pcam->id)
        {
            return pcam;
        }
        pcam = pcam->next;
    }

    return pcam;
}

/**
 * Donne la couleur d'un pixel dans l'image
 */
int
img_get_color(Color_t *color, Camera_t *pcam, int i, int j, float interpol)
{
    int nb_colors = pcam->ii.ZSize;
    int width = pcam->ii.XSize;
    int k;
    float moy = 0.0;

    if (NULL == pcam) return RETURN_FAILED;
    if (NULL == pcam->ii.Data) return RETURN_FAILED;

    if(interpol > 0.0)
    {
        for (k=0; k<nb_colors; k++)
        {
            (*color)[k] = InterpoleImg(j+interpol, i+interpol, k, &(pcam->ii));
            (*color)[k] /= 255.0;
            moy += (*color)[k];
        }
    }
    else
    {
        for (k=0; k<nb_colors; k++)
        {
            (*color)[k] = pcam->ii.Data[(i*width+j)*nb_colors+k];
            (*color)[k] /= 255.0;
            moy += (*color)[k];
        }
    }

    moy /= nb_colors;

    /* on remplit les autres cases avec la premi�re composante */
    for (;k<3; k++)
    {
        (*color)[k] = moy;
    }
    if(nb_colors < 4) (*color)[k] = 1.0;

    return RETURN_SUCCESS;
}

/**
 * proj
 */
Point2d_t proj(Camera_t *cam, Point3d_t ptw)
{
    Edbg(("proj(cam=%p, ptw=(%.3f,%.3f,%.3f,%.3f))", 
          cam, ptw.x, ptw.y, ptw.z, ptw.w));
    Point2d_t res;
    Point3d_t ptmp;

    ptmp = m3d_dot_p3d(cam->m, ptw);
    res.x = ptmp.x / ptmp.z;
    res.y = ptmp.y / ptmp.z;
    res.w = 1.0;

    Rdbg(("proj res=(%.3f,%.3f,%.3f)", res.x, res.y, res.w));
    return res;
}

/**
 * deproj
 */
Point3d_t deproj(Camera_t *cam, Point2d_t pti, float depth)
{
    Edbg(("deproj(cam=%p, pti=(%.3f,%.3f,%.3f), depth=%.3f)", 
          cam, pti.x, pti.y, pti.w, depth));

    Point3d_t res;
    Point3d_t ptmp;

    ptmp.x = pti.x / pti.w;
    ptmp.y = pti.y / pti.w;
    ptmp.z = 1.0;
    ptmp.w = depth;

    res = m3d_dot_p3d(cam->mi, ptmp);

    h_p3d(&res);

    Rdbg(("deproj res=(%.3f,%.3f,%.3f,%.3f)", res.x, res.y, res.z, res.w));
    return res;
}

/**
 * Donne la profondeur qui correspond � une �tiquette.
 */
float label2depth(Camera_t *pcam, int label)
{
    Edbg(("label2depth(pcam=%p, label=%d)", pcam, label));

    Dprintf((1,"nb_labels: %d\n", pcam->nb_labels));
    Dprintf((1,"dmax: %f\n", pcam->dmax));
    Dprintf((1,"dmin: %f\n", pcam->dmin));

    float depth = (float)(pcam->nb_labels-1-label)/(float)(pcam->nb_labels-1)
        * (pcam->dmax - pcam->dmin) + pcam->dmin;

    Rdbg(("label2depth depth=%f", depth));
    return depth;
}

/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
