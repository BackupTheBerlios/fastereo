/*
 * File:     $RCSfile: commands.h,v $
 * Author:   Jean-François LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/05/03 14:15:15 $
 * Version:  $Revision: 1.6 $
 * ID:       $Id: commands.h,v 1.6 2004/05/03 14:15:15 arutha Exp $
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
int com_interpol(const char *command);
int com_sequence(const char *command);

#endif /* _COMMANDS_H_ */

/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
