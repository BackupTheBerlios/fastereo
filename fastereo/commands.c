/*
 * File:     $RCSfile: commands.c,v $
 * Author:   Jean-François LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/05/03 14:15:15 $
 * Version:  $Revision: 1.6 $
 * ID:       $Id: commands.c,v 1.6 2004/05/03 14:15:15 arutha Exp $
 * Comments:
 */
/**
 * @file: commands.c
 * @brief Gestion des commandes.
 */

#include <stdio.h>
#include <stdlib.h>
#include "dbg.h"
#include "utils.h"
#include "commands.h"
#include "stereo.h"

/**
 * Exécute les commandes contenues dans le fichier file_name.
 * @param file_name nom du fichier contenant les commandes.
 * @return RETURN_SUCCESS si succès, RETURN_FAILED sinon
 */
int
execute_commands(const char *file_name)
{
    /* debug trace start */
    Edbg(("execute_commands(file_name='%s')", file_name));

    FILE *fd = NULL;
    char buffer[MAX_LLINE_COMMANDS];

    /* initialisation */
    init_cameras();

    /* ouverture du fichier */
    fd = fopen(file_name, "r");
    if (NULL == fd)
    {
        perror("fopen");
        fprintf(stderr, "Impossible d'ouvrir le fichier %s!\n", file_name);
        Rdbg(("execute_commands RETURN_FAILED"));
        return RETURN_FAILED;
    }

    /* on parcours toutes les lignes du fichier */
    while (fgets(buffer, MAX_LLINE_COMMANDS, fd) != NULL)
    {
        strip_eol(buffer);

        if (!is_comment(buffer))
        {
            /* printf("=== Commande: %s\n", buffer); */

            /* commande 'cameras' */
            if (strncmp(buffer, "cameras ", 8) == 0)
            {
                if (com_cameras(buffer)) 
                {
                    Rdbg(("execute_commands RETURN_FAILED"));
                    return RETURN_FAILED;
                }
            }
            else if (strncmp(buffer, "interpol ", 9) == 0)
            {
                if (com_interpol(buffer))
                {
                    Rdbg(("execute_commands RETURN_FAILED"));
                    return RETURN_FAILED;
                }
            }
            else if (strncmp(buffer, "sequence ", 9) == 0)
            {
                if (com_sequence(buffer))
                {
                    Rdbg(("execute_commands RETURN_FAILED"));
                    return RETURN_FAILED;
                }
            }
            else
            {
                fprintf(stderr, "Commande non reconnue!! %s\n", buffer);
            }

            /* printf("\n"); */
        }
    }


    fclose(fd);

    /* debug trace end */
    Rdbg(("execute_commands RETURN_SUCCESS"));
    return RETURN_SUCCESS;
}

/**
 * Analyse et extrait les informations de la commande 'cameras'
 * @param command buffer contenant la commande
 * @return RETURN_SUCCESS si succès, RETURN_FAILED sinon
 */
int
com_cameras(const char *command)
{
    Edbg(("com_cameras(command='%s')", command));

    int l = strlen(command);

    if(l <= 8) {
        fprintf(stderr, "Commande camera incomplète!\n");
        return -1;
    } else {
        if(load_cameras(&(command[8]))) 
        {
            Rdbg(("com_cameras RETURN_FAILED"));
            return RETURN_FAILED;
        }
    }

    /* printf("%d caméra chargées\n", g_cameras->nb); */

    Rdbg(("com_cameras RETURN_SUCCESS"));
    return RETURN_SUCCESS;
}

/**
 * Crée une image interpolée à partir des autres caméras
 * [PAS FINI]
 * @param command buffer contenant la commande
 * @return RETURN_SUCCESS si succès, RETURN_FAILED sinon
 */
int
com_interpol(const char *command)
{
    Edbg(("com_interpol(command='%s')", command));

    int ret;
    char image_fn[MAX_LNAME];
    char depth_map_fn[MAX_LNAME];
    unsigned char *image = NULL;
    unsigned char *depth_map = NULL;
    int width;
    int height;
    float position = 0.0;

    image_fn[0] = '\0';
    depth_map_fn[0] = '\0';
    
    ret = sscanf(command, "interpol %f %s %s",
                 &position, image_fn, depth_map_fn);

    if (ret < 2)
    {
        fprintf(stderr, "Commande non reconnue!! %s\n", command);
        Rdbg(("com_interpol RETURN_FAILED"));
        return RETURN_FAILED;
    }

    width = g_cameras.root->ii.XSize;
    height = g_cameras.root->ii.YSize;
    image = (unsigned char *) malloc(sizeof(unsigned char)*width*height);
    if ('\0' != depth_map[0])
    {
        depth_map = (unsigned char *) malloc(sizeof(unsigned char)*width*height);
    }

    if (RETURN_FAILED == interpol(position, image, depth_map))
    {
        fprintf(stderr, "L'interpolation a échouée!!\n");
        Rdbg(("com_interpol RETURN_FAILED"));
        return RETURN_FAILED;
    }

    free(image);
    if(depth_map) free(depth_map);

    Rdbg(("com_interpol RETURN_SUCCESS"));
    return RETURN_SUCCESS;
}

/**
 * Crée une séquence d'images interpolées à partir des autres caméras
 * [PAS FINI]
 * @param command buffer contenant la commande
 * @return RETURN_SUCCESS si succès, RETURN_FAILED sinon
 */
int
com_sequence(const char *command)
{
    Edbg(("com_sequence(command='%s')", command));

    Rdbg(("com_sequence RETURN_SUCCESS"));
    return RETURN_SUCCESS;
}

/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
