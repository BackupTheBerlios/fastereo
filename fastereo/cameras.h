/*
 * File:     $RCSfile: cameras.h,v $
 * Author:   Jean-Fran�ois LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/04/30 14:42:55 $
 * Version:  $Revision: 1.8 $
 * ID:       $Id: cameras.h,v 1.8 2004/04/30 14:42:55 arutha Exp $
 */
/**
 * @file cameras.h
 * @brief Gestion des cam�ras.
 */

#ifndef _CAMERAS_H_
#define _CAMERAS_H_

#include <GL/gl.h>
#include "img.h"
#include "geom.h"

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
    float position;          /**< position de la cam�ra */
    Color_t shade;           /**< teinte qu'on doit donner � l'image lors de 
                               l'affichage OpenGL */
	float m[4][4];           /**< matrice de la cam�ra */
	float mi[4][4];          /**< matrice inverse */
    GLfloat mGL[16];         /**< matrice pour la cam�ra OpenGL */
	imginfo ii;              /**< image associ�e � la cam�ra */
    imginfo labels;          /**< carte de profondeurs */
	struct Camera_t *next;   /**< pointeur sur la prochaine cam�ra */
	struct Camera_t *prev;   /**< pointeur sur la cam�ra pr�c�dente */
    GLuint idGL;             /**< identifiant pour la display list OpenGL */
    char display;            /**< bool�en qui indique s'il faut afficher 
                               l'image de cette cam�ra */
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
                     const char *depth_map);
int load_depth_map(Camera_t *cam, 
                   const char *file_name, 
                   int nb_labels);
Camera_t *get_camera(int id);
int img_get_color(Color_t *color, Camera_t *pcam, int i, int j, float interpol, 
                  char dm);
Point2d_t proj(Camera_t *cam, Point3d_t ptw);
Point3d_t deproj(Camera_t *cam, Point2d_t pti, float depth);
float label2depth(Camera_t *pcam, int label);

extern Cameras_t g_cameras;
extern int g_nb_labels;
extern float g_dmin;
extern float g_dmax;


#endif /* _CAMERAS_H_ */

/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
