/*
 * File:     $RCSfile: cameras.h,v $
 * Author:   Jean-François LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/04/13 23:59:49 $
 * Version:  $Revision: 1.1 $
 * ID:       $Id: cameras.h,v 1.1 2004/04/13 23:59:49 arutha Exp $
 */
/**
 * @file cameras.h
 * @brief Gestion des caméras.
 */

#ifndef _CAMERAS_H_
#define _CAMERAS_H_

#include "img.h"

/** 
 * La taille maximale que peut avoir une ligne dans un fichier *.cameras
 */
#define MAX_LLINE_CAMERAS 1024
/** La taille maximale que peut avoir un chemin vers un fichier */
#define MAX_LNAME 1024

typedef struct Camera {
	int id;                /**< identifiant de la caméra */
	float m[4][4];         /**< matrice de la caméra */
	float mi[4][4];        /**< matrice inverse */
	char img[MAX_LNAME];   /**< nom du fichier de l'image */
	char dm[MAX_LNAME];    /**< nom du fichier de la carte de profondeur */
	imginfo ii;            /**< structure image */
	struct Camera *next; /**< pointeur sur la prochaine caméra */
} Camera_t;

typedef struct {
	int nb;         /**< nombre de caméras */
	Camera_t *root; /**< première caméra de la file */
	Camera_t *last; /**< pointeur sur la dernière caméra de la file */
} Cameras_t;

void init_cameras(void);
int load_cameras(const char *file_name);
void destroy_cameras(void);
void destroy_camera(Camera_t *camera);
Camera_t *add_camera(const int id, 
                     const float position, 
                     const char *image, 
                     const char *depth_map);

extern Cameras_t g_cameras;


#endif /* _CAMERAS_H_ */

/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
