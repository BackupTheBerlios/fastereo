/*
 * File:     $RCSfile: geom.h,v $
 * Author:   Jean-Fran�ois LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/04/16 17:31:23 $
 * Version:  $Revision: 1.6 $
 * ID:       $Id: geom.h,v 1.6 2004/04/16 17:31:23 arutha Exp $
 * Comments:
 */
/**
 * @file: geom.h
 * @brief Fonctions g�om�triques.
 */

#ifndef _GEOM_H_
#define _GEOM_H_

/**
 * @brief Un vecteur en 2 dimensions
 */
typedef struct {
    float x; /**< coordonn�e en X */
    float y; /**< coordonn�e en Y */
} Vector2d_t;

/**
 * @brief Un vecteur en 3 dimensions
 */
typedef struct {
    float x; /**< coordonn�e en X */
    float y; /**< coordonn�e en Y */
    float z; /**< coordonn�e en Z */
} Vector3d_t;

/**
 * @brief Un point en 2 dimensions
 */
typedef struct {
    float x; /**< coordonn�e en X */
    float y; /**< coordonn�e en Y */
    float w; /**< coordonn�e homog�ne */
} Point2d_t;

/**
 * @brief Un point en 3 dimensions
 */
typedef struct {
    float x; /**< coordonn�e en X */
    float y; /**< coordonn�e en Y */
    float z; /**< coordonn�e en Z */
    float w; /**< coordonn�e homog�ne */
} Point3d_t;

/**
 * Couleur RGBA
 */
typedef float Color_t[4];

void load_identity(float m[4][4]);
int inv_matrix4(float min[4][4], float mout[4][4]);
void normalize2d(Vector2d_t *v);
void normalize3d(Vector3d_t *v);
void deproj_point(Point3d_t *point, Point3d_t *eye, float step_z, float label);

#endif /* _GEOM_H_ */

/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
