/*
 * File:     $RCSfile: cameras.h,v $
 * Author:   Jean-Fran�ois LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/04/15 05:21:22 $
 * Version:  $Revision: 1.3 $
 * ID:       $Id: cameras.h,v 1.3 2004/04/15 05:21:22 arutha Exp $
 */
/**
 * @file cameras.h
 * @brief Gestion des cam�ras.
 */

#ifndef _CAMERAS_H_
#define _CAMERAS_H_

#include "img.h"

/** La taille maximale que peut avoir une ligne dans un fichier *.cameras */
#define MAX_LLINE_CAMERAS 1024
/** La taille maximale que peut avoir un chemin vers un fichier */
#define MAX_LNAME 1024

/**
 * @brief Structure repr�sentant une cam�ra
 */
typedef struct Camera_t 
{
	int id;                  /**< identifiant de la cam�ra */
	float m[4][4];           /**< matrice de la cam�ra */
	float mi[4][4];          /**< matrice inverse */
    // char img[MAX_LNAME];     /**< nom du fichier de l'image */
    // char dm[MAX_LNAME];      /**< nom du fichier de la carte de profondeur */
	imginfo ii;              /**< structure image */
    unsigned char *labels;   /**< carte de profondeur */
    unsigned char nb_labels; /**< nombre d'�tiquettes */
    float range;             /**< �tendue sur laquelle s'�tale les �tiquettes */
	struct Camera_t *next;   /**< pointeur sur la prochaine cam�ra */
	struct Camera_t *prev;   /**< pointeur sur la cam�ra pr�c�dente */
} Camera_t;

/**
 * @brief Structure pour g�rer une liste de cam�ras
 */
typedef struct Cameras_t 
{
	int nb;         /**< nombre de cam�ras */
	Camera_t *root; /**< pointeur sur la premi�re cam�ra de la file */
	Camera_t *last; /**< pointeur sur la derni�re cam�ra de la file */
} Cameras_t;

void init_cameras(void);
int load_cameras(const char *file_name);
void destroy_cameras(void);
void destroy_camera(Camera_t *camera);
Camera_t *add_camera(const int id, 
                     const float position, 
                     const char *image, 
                     const char *depth_map,
                     const unsigned char nb_labels,
                     const float range);
int load_depth_map(Camera_t *cam, 
                   const char *file_name, 
                   unsigned char nb_labels);

extern Cameras_t g_cameras;


#endif /* _CAMERAS_H_ */

/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
