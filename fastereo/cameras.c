/*
 * File:     $RCSfile: cameras.c,v $
 * Author:   Jean-François LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/04/26 20:24:40 $
 * Version:  $Revision: 1.7 $
 * ID:       $Id: cameras.c,v 1.7 2004/04/26 20:24:40 arutha Exp $
 * Comments:
 */
/**
 * @file cameras.c
 * @brief Gestion des caméras.
 */

#include <stdlib.h>
#include <stdio.h>
#include "dbg.h"
#include "utils.h"
#include "cameras.h"
#include "geom.h"
#include "img.h"
#include "display.h"

/** variable globale représentant toutes les caméras chargées */
Cameras_t g_cameras;
/** Nombre d'étiquettes dans les cartes de profondeurs */
int g_nb_labels = -1;
/** Profondeur minimale */
float g_dmin = -1;
/** Profondeur maximale */
float g_dmax = -1;

/**
 * Initialise l'objet qui représente les caméras.
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
 * Libère la mémoire occupée par la caméra et celles qui suivent dans la liste.
 * Cette fonction s'appelle récursivement.
 */
void 
destroy_camera(Camera_t *camera)
{
    Edbg(("destroy_camera(camera=%p)", camera));

    if (NULL != camera)
    {
        Dprintf((1,"destruction de la caméra n°%d\n", camera->id));
        if (NULL != camera->next)
        {
            destroy_camera(camera->next);
        }
        FreeImage(&(camera->ii));
        FreeImage(&(camera->labels));
        glDeleteLists(camera->idGL, 1);
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
    int nb_labels;
    Color_t shade;
    float dmin, dmax;

    /* initialisation */
    g_cameras.root = NULL;
    g_cameras.nb = 0;
    shade[3] = 1.0;

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

                /* on extrait les données */
                ret = sscanf(buffer, "camera %d %f %s %s",
                             &id, &position, image, depth_map);

                Dprintf((1,"Données extraites:\n"));
                Dprintf((1,"id: %d\n", id));
                Dprintf((1,"position: %.3f\n", position));
                Dprintf((1,"image: %s\n", image));
                Dprintf((1,"depth_map: %s\n", depth_map));

                if ((ret < 3) || (id < 0) || (image[0] == '\0') 
                    || (nb_labels > 255))
                {
                    fprintf(stderr, "Commande non reconnue!! %s\n", buffer);
                }
                else
                {
                    /* on ajoute la caméra */
                    if(ret > 3)
                    {
                        cam = add_camera(id, position, image, depth_map);
                    }
                    else
                    {
                        cam = add_camera(id, position, image, NULL);
                    }

                    /* erreur? */
                    if (NULL == cam)
                    {
                        fprintf(stderr, 
                                "Impossible d'ajouter la caméra '%s'!!\n", 
                                buffer);
                        destroy_cameras();
                        Rdbg(("load_cameras RETURN_FAILED"));
                        return RETURN_FAILED;
                    }
                    Dprintf((1,"Caméra ajoutée!\n"));
                }
            }
            /* commande 'depth_maps' */
            else if (strncmp(buffer, "depthmaps ", 10) == 0)
            {
                dmin = dmax = nb_labels = -1;

                /* on extrait les données */
                ret = sscanf(buffer, "depthmaps %d %f %f",
                             &nb_labels, &dmin, &dmax);

                Dprintf((1,"Données extraites:\n"));
                Dprintf((1,"nb_labels: %d\n", nb_labels));
                Dprintf((1,"dmin: %.3f\n", dmin));
                Dprintf((1,"dmax: %.3f\n", dmax));

                if ((ret < 3) || (nb_labels < 0) || (dmin < 0) || (dmax < 0))
                {
                    fprintf(stderr, "Commande non reconnue!! %s\n", buffer);
                }
                else
                {
                    if (dmin > dmax)
                    {
                        g_dmax = dmin;
                        g_dmin = dmax;
                    }
                    else
                    {
                        g_dmin = dmin;
                        g_dmax = dmax;
                    }
                    g_nb_labels = nb_labels;
                }
            }
            /* commande 'shade' */
            else if (strncmp(buffer, "shade ", 6) == 0)
            {
                /* on extrait les données */
                ret = sscanf(buffer, "shade %d %f %f %f",
                             &id, &(shade[0]), &(shade[1]), &(shade[2]));

                if (ret < 4)
                {
                    fprintf(stderr, "Commande non reconnue!! %s\n", buffer);
                }
                else
                {
                    Dprintf((1,"Données extraites:\n"));
                    Dprintf((1,"id: %d\n", id));
                    Dprintf((1,"R: %.3f\n", shade[0]));
                    Dprintf((1,"G: %.3f\n", shade[1]));
                    Dprintf((1,"B: %.3f\n", shade[2]));

                    cam = get_camera (id);

                    /* si la caméra n'a pas été trouvée */
                    if (NULL == cam)
                    {
                        fprintf(stderr, "Impossible de trouver la caméra n°%d!!\n",
                                id);
                    }
                    else
                    {
                        cam->shade[0] = shade[0];
                        cam->shade[1] = shade[1];
                        cam->shade[2] = shade[2];
                        cam->shade[3] = 1.0;

                        /* g_enable_shade = TRUE; */
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
 * Ajoute une caméra dans la liste
 * @param id identifiant de la caméra
 * @param position position de la caméra
 * @param image nom du fichier de l'image associée
 * @param depth_map nom du fichier de la carte de profondeurs associée 
 *                  [NULL si on n'en a pas]
 * @return un pointeur sur la nouvelle caméra
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
    Camera_t *prev = NULL;

    if (g_nb_labels < 0)
    {
        fprintf(stderr, "Il faut utiliser la commande depth_maps avant la"
                " commande camera!!\n");
        Rdbg(("add_camera NULL"));
        return NULL;
    }

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
        /* le malloc n'a pas marché */
        Rdbg(("add_camera NULL"));
        return NULL;
    }

    cam->id = id;

    /* on charge l'image */
    if (LoadImage((char *)image, &(cam->ii)))
    {
        /* si le chargement de l'image a échoué, on détruit la caméra */
        fprintf(stderr, "Impossible de charger l'image %s de la caméra %d!\n",
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
        if (load_depth_map(cam, depth_map, g_nb_labels))
        {
            /* si le chargement de la carte de profondeurs a échoué, on détruit 
             * la caméra */
            fprintf(stderr, "Impossible de charger la carte de profondeurs %s"
                    " de la caméra %d!\n", depth_map, cam->id);
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
    }

    cam->display = TRUE;
    cam->next = NULL;
    cam->prev = prev;
    load_identity(cam->m);
    cam->position = position;
    cam->m[0][3] = position;
    inv_matrix4(cam->m, cam->mi);
    cam->mGL[0] = 2.0/cam->ii.XSize;
    cam->mGL[1] = 0;
    cam->mGL[2] = 0;
    cam->mGL[3] = 0;
    cam->mGL[4] = 0;
    cam->mGL[5] = 2.0/cam->ii.YSize;
    cam->mGL[6] = 0;
    cam->mGL[7] = 0;
    cam->mGL[8] = -(g_dmax-g_dmin)*2.0*position / cam->ii.XSize;
    cam->mGL[9] = 0;
    cam->mGL[10] = 0.9;
    cam->mGL[11] = 0;
    cam->mGL[12] = -1;
    cam->mGL[13] = -1;
    cam->mGL[14] = 0;
    cam->mGL[15] = 1;

    /* on met la teinte à blanc par défaut */
    cam->shade[0] = 1.0;
    cam->shade[1] = 1.0;
    cam->shade[2] = 1.0;
    cam->shade[3] = 1.0;

    /* on augmente le nombre de caméras */
    g_cameras.nb++;
    /* on met à jour le pointeur sur la dernière caméra */
    g_cameras.last = cam;

    Rdbg(("add_camera cam=%p", cam));
    return cam;
}

/**
 * Charge une carte de profondeurs à partir d'une image.
 * @param cam caméra pour laquelle il faut charger la carte de profondeurs
 * @param file_name nom du fichier contenant l'image de la carte de profondeurs
 * @param nb_labels nombre d'étiquettes dans cette carte de profondeurs
 * @return RETURN_SUCCESS si succès, RETURN_FAILED sinon
 */
int 
load_depth_map(Camera_t *cam, const char *file_name, int nb_labels)
{
    Edbg(("load_depth_map(cam=%p, file_name='%s', nb_labels=%d)",
          cam, file_name, nb_labels));

    int i;
    int L;
    float d;

    /* on charge l'image */
    if (LoadImage((char *)file_name, &(cam->labels)))
    {
        Dprintf((1,"LoadImage failed!\n"));
        Rdbg(("load_depth_map RETURN_FAILED"));
        return RETURN_FAILED;
    }

    /* on convertit les intensités en étiquettes */
    L = cam->labels.XSize * cam->labels.YSize;
    d = (nb_labels-1.0)/255;
    for (i=0; i<L; i++)
    {
        cam->labels.Data[i] = (int)((cam->labels.Data[i]-0.5)*d + 0.5);
    }

    Rdbg(("load_depth_map RETURN_SUCCESS"));
    return RETURN_SUCCESS;
}

/**
 * Renvoie la caméra dont l'identifiant est id
 * @param id identifiant de la caméra
 * @return pointeur sur la caméra, NULL si non trouvée
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
            (*color)[k] = InterpoleImg(i+interpol, j+interpol, k, &(pcam->ii));
            (*color)[k] /= 255.0;
            moy += (*color)[k];
        }
    }
    else
    {
        for (k=0; k<nb_colors; k++)
        {
            (*color)[k] = pcam->ii.Data[(j*width+i)*nb_colors+k];
            (*color)[k] /= 255.0;
            moy += (*color)[k];
        }
    }

    moy /= nb_colors;

    /* on remplit les autres cases avec la moyenne des premières composantes */
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
 * Donne la profondeur 1/z qui correspond à une étiquette.
 */
float label2depth(Camera_t *pcam, int label)
{
/*    Edbg(("label2depth(pcam=%p, label=%d)", pcam, label));*/

    float depth = (float)(g_nb_labels-1-label)/(float)(g_nb_labels-1)
        * (g_dmax - g_dmin) + g_dmin;

/*    Rdbg(("label2depth depth=%f", depth));*/
    return depth;
}

/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
