/*
 * File:     $RCSfile: geom.h,v $
 * Author:   Jean-François LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/04/19 18:59:30 $
 * Version:  $Revision: 1.7 $
 * ID:       $Id: geom.h,v 1.7 2004/04/19 18:59:30 arutha Exp $
 * Comments:
 */
/**
 * @file: geom.h
 * @brief Fonctions géométriques.
 */

#ifndef _GEOM_H_
#define _GEOM_H_

/**
 * @brief Un vecteur en 2 dimensions
 */
typedef struct {
    float x; /**< coordonnée en X */
    float y; /**< coordonnée en Y */
} Vector2d_t;

/**
 * @brief Un vecteur en 3 dimensions
 */
typedef struct {
    float x; /**< coordonnée en X */
    float y; /**< coordonnée en Y */
    float z; /**< coordonnée en Z */
} Vector3d_t;

/**
 * @brief Un point en 2 dimensions
 */
typedef struct {
    float x; /**< coordonnée en X */
    float y; /**< coordonnée en Y */
    float w; /**< coordonnée homogène */
} Point2d_t;

/**
 * @brief Un point en 3 dimensions
 */
typedef struct {
    float x; /**< coordonnée en X */
    float y; /**< coordonnée en Y */
    float z; /**< coordonnée en Z */
    float w; /**< coordonnée homogène */
} Point3d_t;

/**
 * @brief Un point en 2 dimensions (int)
 */
typedef struct {
    int x; /**< coordonnée en X */
    int y; /**< coordonnée en Y */
} Point2di_t;

/**
 * @brief Un point en 3 dimensions (int)
 */
typedef struct {
    int x; /**< coordonnée en X */
    int y; /**< coordonnée en Y */
    int z; /**< coordonnée en Z */
} Point3di_t;

/**
 * Couleur RGBA
 */
typedef float Color_t[4];
/**
 * Couleur RGBA (unsigned char)
 */
typedef unsigned char Colori_t[4];

void load_identity(float m[4][4]);
int inv_matrix4(float min[4][4], float mout[4][4]);
void normalize2d(Vector2d_t *v);
void normalize3d(Vector3d_t *v);
void h_p2d(Point2d_t *p);
void h_p3d(Point3d_t *p);
Point3d_t m3d_dot_p3d(float m[4][4], Point3d_t p);

#endif /* _GEOM_H_ */

/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
