/*
 * File:     $RCSfile: commands.h,v $
 * Author:   Jean-François LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/04/14 05:39:33 $
 * Version:  $Revision: 1.4 $
 * ID:       $Id: commands.h,v 1.4 2004/04/14 05:39:33 arutha Exp $
 */
/**
 * @file: commands.h
 * @brief Gestion des commandes.
 */

#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include "cameras.h"

/** 
 * La taille maximale que peut avoir une ligne dans un fichier *.commands
 */
#define MAX_LLINE_COMMANDS 1024

/**
 * Définit les modes
 */
typedef enum 
{
    INTENSITIES = 1,
    DEPTH_MAPS = 2
} Stereo_mode_t;

/**
 * Définit les actions possibles
 */
typedef enum 
{
    SIMPLE = 1,
    SEQUENCE = 2,
    OPENGL = 3
} Action_t;

int execute_commands(const char *file_name);
int com_cameras(const char *command);

// int simple_image(const stereo_mode_t mode,
//                  const char *start_im,
//                  const char *start_dm,
//                  const char *end_im,
//                  const char *end_dm,
//                  const int nb_labels,
//                  const float distance,
//                  const float interpol,
//                  const char *result_im,
//                  const char *result_dm);
// int merge_depth_maps();

#endif /* _COMMANDS_H_ */

/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
