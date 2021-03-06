/*
 * File:     $RCSfile: geom.c,v $
 * Author:   Jean-Fran�ois LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/05/03 14:15:15 $
 * Version:  $Revision: 1.6 $
 * ID:       $Id: geom.c,v 1.6 2004/05/03 14:15:15 arutha Exp $
 * Comments:
 */
/**
 * @file geom.c
 * @brief Fonctions g�om�triques.
 */

#include <math.h>
#include "dbg.h"
#include "utils.h"
#include "geom.h"
#include "cameras.h"

/**
 * Charge la matrice identit� 4x4.
 * @param m matrice 4x4 dans laquelle on souhaite charger la matrice identit�
 */
void 
load_identity(float m[4][4])
{
    Edbg(("load_identity(m)"));
    m[0][0] = 1.0; m[0][1] = 0.0; m[0][2] = 0.0; m[0][3] = 0.0;
    m[1][0] = 0.0; m[1][1] = 1.0; m[1][2] = 0.0; m[1][3] = 0.0;
    m[2][0] = 0.0; m[2][1] = 0.0; m[2][2] = 1.0; m[2][3] = 0.0;
    m[3][0] = 0.0; m[3][1] = 0.0; m[3][2] = 0.0; m[3][3] = 1.0;
    Rdbg(("load_identity"));
}

/**
 * Inverse une matrice 4x4
 * @param min matrice 4x4 que l'on souhaite inverser
 * @param mout matrice 4x4 qui contiendra le r�sultat
 * @return RETURN_SUCCESS si succ�s, RETURN_FAILED sinon
 */
int 
inv_matrix4(float min[4][4], float mout[4][4])
{
    Edbg(("inv_matrix4(min, mout)"));
    float det;

    float a = min[0][0]*min[1][1] - min[0][1]*min[1][0];
    float b = min[0][0]*min[1][2] - min[0][2]*min[1][0];
    float c = min[0][0]*min[1][3] - min[0][3]*min[1][0];
    float d = min[0][1]*min[1][2] - min[0][2]*min[1][1];
    float e = min[0][1]*min[1][3] - min[0][3]*min[1][1];
    float f = min[0][2]*min[1][3] - min[0][3]*min[1][2];
    float g = min[2][0]*min[3][1] - min[2][1]*min[3][0];
    float h = min[2][0]*min[3][2] - min[2][2]*min[3][0];
    float i = min[2][0]*min[3][3] - min[2][3]*min[3][0];
    float j = min[2][1]*min[3][2] - min[2][2]*min[3][1];
    float k = min[2][1]*min[3][3] - min[2][3]*min[3][1];
    float l = min[2][2]*min[3][3] - min[2][3]*min[3][2];

    det = a*l-b*k+c*j+d*i-e*h+f*g;
    /* mettre un epsilon ? */
    if(det == 0.0)
    {
        Rdbg(("inv_matrix4 RETURN_FAILED"));
        return RETURN_FAILED;
    }

    mout[0][0] = + min[1][1]*l - min[1][2]*k + min[1][3]*j;
    mout[1][0] = - min[1][0]*l + min[1][2]*i - min[1][3]*h;
    mout[2][0] = + min[1][0]*k - min[1][1]*i + min[1][3]*g;
    mout[3][0] = - min[1][0]*j + min[1][1]*h - min[1][2]*g;
    mout[0][1] = - min[0][1]*l + min[0][2]*k - min[0][3]*j;
    mout[1][1] = + min[0][0]*l - min[0][2]*i + min[0][3]*h;
    mout[2][1] = - min[0][0]*k + min[0][1]*i - min[0][3]*g;
    mout[3][1] = + min[0][0]*j - min[0][1]*h + min[0][2]*g;
    mout[0][2] = + min[3][1]*f - min[3][2]*e + min[3][3]*d;
    mout[1][2] = - min[3][0]*f + min[3][2]*c - min[3][3]*b;
    mout[2][2] = + min[3][0]*e - min[3][1]*c + min[3][3]*a;
    mout[3][2] = - min[3][0]*d + min[3][1]*b - min[3][2]*a;
    mout[0][3] = - min[2][1]*f + min[2][2]*e - min[2][3]*d;
    mout[1][3] = + min[2][0]*f - min[2][2]*c + min[2][3]*b;
    mout[2][3] = - min[2][0]*e + min[2][1]*c - min[2][3]*a;
    mout[3][3] = + min[2][0]*d - min[2][1]*b + min[2][2]*a;

    Rdbg(("inv_matrix4 RETURN_SUCCESS"));
    return RETURN_SUCCESS;
}

/**
 * Normalise un vecteur en 2 dimensions
 * @param v un pointeur sur le vecteur � normaliser
 */
void 
normalize2d(Vector2d_t *v)
{
    if (NULL == v) 
    {
        Rdbg(("normalize2d"));
        return;
    }

    float n = v->x * v->x + v->y * v->y;
    n = sqrt(n);

    v->x /= n;
    v->y /= n;
}

/**
 * Normalise un vecteur en 3 dimensions
 * @param v un pointeur sur le vecteur � normaliser
 */
void 
normalize3d(Vector3d_t *v)
{
    if (NULL == v) 
    {
        Rdbg(("normalize3d"));
        return;
    }

    float n = v->x * v->x + v->y * v->y + v->z * v->z;
    n = sqrt(n);

    v->x /= n;
    v->y /= n;
    v->z /= n;
}

/**
 * Homog�n�ise un point 2D pour le ramener dans le syst�me euclidien.
 * @param p point 2D � homog�n�iser
 */
void h_p2d(Point2d_t *p)
{
	p->x = p->x / p->w;
	p->y = p->y / p->w;
	p->w = 1.0;
}

/**
 * Homog�n�ise un point 3D pour le ramener dans le syst�me euclidien.
 * @param p point 3D � homog�n�iser
 */
void h_p3d(Point3d_t *p)
{
	p->x = p->x / p->w;
	p->y = p->y / p->w;
	p->z = p->z / p->w;
	p->w = 1.0;
}

/**
 * Applique une matrice � un point 3D
 * @param m matrice 4x4
 * @param p point 3D
 * @return nouveau point 3D
 */
Point3d_t m3d_dot_p3d(float m[4][4], Point3d_t p)
{
	Point3d_t res;

	res.x = m[0][0]*p.x + m[0][1]*p.y + m[0][2]*p.z + m[0][3]*p.w;
	res.y = m[1][0]*p.x + m[1][1]*p.y + m[1][2]*p.z + m[1][3]*p.w;
	res.z = m[2][0]*p.x + m[2][1]*p.y + m[2][2]*p.z + m[2][3]*p.w;
	res.w = m[3][0]*p.x + m[3][1]*p.y + m[3][2]*p.z + m[3][3]*p.w;

	return res;
}

/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
