/*
 * File:     $RCSfile: fastereo.c,v $
 * Author:   Jean-François LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/04/13 23:59:49 $
 * Version:  $Revision: 1.5 $
 * ID:       $Id: fastereo.c,v 1.5 2004/04/13 23:59:49 arutha Exp $
 * Comments:
 */
/**
 * @file fastereo.c
 * @brief Fichier principal
 */

#include <stdlib.h>
#include <unistd.h>
#include "dbg.h"
#include "utils.h"
#include "fastereo.h"
#include "commands.h"


/**
 * Prints usage
 */
void
print_usage(void)
{
    Edbg(("print_usage()"));
    printf("usage: fastereo <file.commands>\n"
           "\n"
           "options:\n"
           "  -z\tdisplay in OpenGL\n");
    Rdbg(("print_usage"));
}

/**
 * Fonction principale
 * @param argc nombre d'arguments
 * @param argv tableau d'arguments
 * @return program exit code
 */
int
main(int argc, 
     char *argv[])
{
    /* debug trace start */
    Edbg(("main(argc=%d, argv=%s)", argc, argv[0]));
    Initdbg((&argc, argv));

    int c;
    Stereo_mode_t mode = INTENSITIES;
    Action_t action = SIMPLE;
    int ret;

    /* on récupère les options */
    while ( (c = getopt(argc, argv, "hz")) >= 0) 
    {
        switch (c) 
        {
            case 'h':
                /* print help */
                print_usage();
                Rdbg(("main return EXIT_SUCCESS"));
                return EXIT_SUCCESS;
            case 'z':
                /* display in OpenGL */
                action = OPENGL;
                mode = DEPTH_MAPS;
                break;
        }
    }

    /* il faut le nom du fichier de commandes dans les arguments */
    if ((argc-optind) != 1)
    {
        print_usage();
        Rdbg(("main return EXIT_FAILURE"));
        return EXIT_FAILURE;
    }

    /* on charge les commandes */
    ret = execute_commands(argv[optind]);
    if (ret != RETURN_SUCCESS)
    {
        destroy_cameras();
        return EXIT_FAILURE;
    }

    /* debug trace end */
    Rdbg(("main EXIT_SUCCESS"));
    return EXIT_SUCCESS;
}

/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
