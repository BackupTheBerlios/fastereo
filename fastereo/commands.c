/*
 * File:     $RCSfile: commands.c,v $
 * Author:   Jean-François LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/04/15 05:21:22 $
 * Version:  $Revision: 1.3 $
 * ID:       $Id: commands.c,v 1.3 2004/04/15 05:21:22 arutha Exp $
 * Comments:
 */
/**
 * @file: commands.c
 * @brief Gestion des commandes.
 */

#include <stdio.h>
#include "dbg.h"
#include "utils.h"
#include "commands.h"

/**
 * Exécute les commandes contenues dans le fichier file_name.
 * @param file_name nom du fichier contenant les commandes.
 * @return différent de 0 si échec.
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
            printf("=== Commande: %s\n", buffer);

            /* commande 'cameras' */
            if (strncmp(buffer, "cameras", 7) == 0)
            {
                if (com_cameras(buffer)) 
                {
                    Rdbg(("execute_commands RETURN_FAILED"));
                    return RETURN_FAILED;
                }
            }
            /* |+ commande proj +|                                          */
            /* else if(strncmp(buffer, "proj", 4) == 0)                     */
            /* {                                                            */
            /*     if(com_proj(buffer, &cameras)) return -1;                */
            /* }                                                            */
            /* |+ commande cout +|                                          */
            /* else if(strncmp(buffer, "cout", 4) == 0)                     */
            /* {                                                            */
            /*     if(com_cout(buffer, &cameras)) return -1;                */
            /* }                                                            */
            /* |+ commande scan +|                                          */
            /* else if(strncmp(buffer, "scan", 4) == 0)                     */
            /* {                                                            */
            /*     if(com_scan(buffer, &cameras)) return -1;                */
            /* }                                                            */
            /* |+ commande inter +|                                         */
            /* else if(strncmp(buffer, "inter", 5) == 0)                    */
            /* {                                                            */
            /*     if(com_inter(buffer, &cameras)) return -1;               */
            /* }                                                            */
            /* |+ commande stereo +|                                        */
            /* else if(strncmp(buffer, "stereo", 6) == 0)                   */
            /* {                                                            */
            /*     if(com_stereo(buffer, &cameras)) return -1;              */
            /* }                                                            */
            else
            {
                fprintf(stderr, "Commande non reconnue!! %s\n", buffer);
            }

            printf("\n");
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
 * @return différent de 0 si échec
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

/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
