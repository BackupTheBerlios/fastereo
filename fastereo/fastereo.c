/*
 * File:     $RCSfile: fastereo.c,v $
 * Author:   Jean-François LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/04/15 05:21:22 $
 * Version:  $Revision: 1.7 $
 * ID:       $Id: fastereo.c,v 1.7 2004/04/15 05:21:22 arutha Exp $
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
#include "display.h"

/**
 * Appelée lors de la fermeture du programme.
 * S'occupe du nettoyage...
 */
void terminate_program(void)
{
    Edbg(("terminate_program()"));

    destroy_cameras();
    
    Rdbg(("terminate_program"));
}

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
    char display_opengl = FALSE;
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
                display_opengl = TRUE;
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

    /* affichage */
    if (TRUE == display_opengl)
    {
        /* on utilise atexit car glut ne dit pas quand il quitte le programme
         * (ce gros malin...) 
         * cf 3.070 http://users.frii.com/martz/oglfaq/glut.htm */
        atexit(terminate_program);

        init_display(&argc, argv);
        start_display();
    }

    /* nettoyage */
    destroy_cameras();

    /* debug trace end */
    Rdbg(("main EXIT_SUCCESS"));
    return EXIT_SUCCESS;
}

/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
