/*
 * File:     $Source: /home/xubuntu/berlios_backup/github/tmp-cvs/fastereo/Repository/fastereo/fastereo.c,v $
 * Author:   Jean-François LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/04/13 18:59:04 $
 * Version:  $Revision: 1.3 $
 * ID:       $Id: fastereo.c,v 1.3 2004/04/13 18:59:04 arutha Exp $
 * Comments:
 */

#include <stdlib.h>
#include <unistd.h>
#include "dbg.h"
#include "fastereo.h"

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
main(argc, argv)
    int argc;
    char *argv[];
{
    /* debug trace start */
    Edbg(("main(argc=%d, argv=%s)", argc, argv[0]));
    Initdbg((&argc, argv));

    int c;
    stereo_mode_t mode = INTENSITIES;
    action_t action = SIMPLE;
    int ret;

    /* on récupère les options */
    while( (c = getopt(argc, argv, "hz")) >= 0) 
    {
        switch(c) 
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
    if((argc-optind) != 1)
    {
        print_usage();
        Rdbg(("main return EXIT_FAILURE"));
        return EXIT_FAILURE;
    }

    /* on charge les commandes */
    ret = execute_commands(argv[optind]);

    /* debug trace end */
    Rdbg(("main EXIT_SUCCESS"));
    return EXIT_SUCCESS;
}

/* use 4 spaces as a tab please */
/* vim: set ts=4 sw=4 et cino=t0(0: */
