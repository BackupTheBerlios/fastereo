/*
 * File:     $RCSfile: stereo.c,v $
 * Author:   Jean-Fran�ois LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/05/03 14:15:15 $
 * Version:  $Revision: 1.3 $
 * ID:       $Id: stereo.c,v 1.3 2004/05/03 14:15:15 arutha Exp $
 * Comments:
 */
/**
 * @file stereo.c
 * @brief Fonctions li�es � la st�r�oscopie
 */

#include <stdlib.h>
#include <stdio.h>
#include "dbg.h"
#include "utils.h"
#include "cameras.h"
#include "img.h"

/**
 * Cr�e une image interpol�e � partir des autres cam�ras
 * [PAS FINI]
 * @param position position de l'image interpol�e
 * @param image buffer pour contenir l'image interpol�e
 * @param depth_map buffer pour contenir la carte de profondeurs
 * @return RETURN_SUCCESS si succ�s, RETURN_FAILED sinon
 */
int 
interpol(float position, char *image, char *depth_map)
{
    Edbg(("interpol(position=%.3f, image=%p, depth_map=%p)", 
          position, image, depth_map));

    float *depths;
    float *weigths;
    Camera_t *root = g_cameras.root;
    Camera_t *last = g_cameras.last;
    int width = root->ii.XSize;
    int height = root->ii.YSize;
    int i, j;
    Camera_t *pcam = root;
    unsigned char *im;
    unsigned char *labels;
    unsigned char label;
    float range = g_dmax - g_dmin;
    float depth;
    float campos;
    float newpos;
    float start = root->m[0][3];
    float end = last->m[0][3];
    float distance = end - start;

    /* initialisation */
    depths = (float *) malloc(sizeof(float) * width * height);
    weigths = (float *) malloc(sizeof(float) * width * height);
    for (i=0; i<width*height; i++)
    {
        depths[i] = 0;
        weigths[i] = 0;
    }

    /* on parcourt les cam�ras */
    while (NULL == pcam)
    {
        im = pcam->ii.Data;
        labels = pcam->labels.Data;
        campos = pcam->m[0][3];

        /* on parcourt tous les pixels de l'image */
        for (j=0; j<height; j++)
        {
            for (i=0; i<width; i++)
            {
                /* on r�cup�re la profondeur � partir de l'�tiquette */
                label = labels[j*width+i];
                depth = label2depth(pcam, label);

                /* on calcule la nouvelle position */
                newpos = campos + (depth/range * distance)*(position-campos);
            }
        }

        /* on passe � la cam�ra suivante */
        pcam = pcam->next;
    }

    /* nettoyage */
    free(depths);
    free(weigths);

    Rdbg(("interpol RETURN_SUCCESS"));
    return RETURN_SUCCESS;
}


/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
