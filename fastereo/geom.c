/*
 * File:     $RCSfile: geom.c,v $
 * Author:   Jean-François LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/04/13 23:59:49 $
 * Version:  $Revision: 1.1 $
 * ID:       $Id: geom.c,v 1.1 2004/04/13 23:59:49 arutha Exp $
 * Comments:
 */
/**
 * @file geom.c
 * @brief Fonctions géométriques.
 */

#include "dbg.h"
#include "utils.h"

/**
 * Charge la matrice identité 4x4.
 * @param m matrice 4x4 dans laquelle on souhaite charger la matrice identité
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
 * @param mout matrice 4x4 qui contiendra le résultat
 */
int 
inv_matrix4(float min[4][4], float mout[4][4])
{
    Edbg(("inv_matrix4(min, mout)"));
    int ii, jj;
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

    for(ii = 0; ii < 4; ii++)
    {
        for(jj = 0; jj < 4; jj++)
        {
            mout[ii][jj] /= det;
        }
    }

    Rdbg(("inv_matrix4 RETURN_SUCCESS"));
    return RETURN_SUCCESS;
}

/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
