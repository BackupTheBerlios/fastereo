/*
 * File:     $RCSfile: fastereo.c,v $
 * Author:   Jean-François LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/04/19 18:59:29 $
 * Version:  $Revision: 1.10 $
 * ID:       $Id: fastereo.c,v 1.10 2004/04/19 18:59:29 arutha Exp $
 * Comments:
 */
/**
 * @file fastereo.c
 * @brief Fichier principal
 */

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "dbg.h"
#include "utils.h"
#include "fastereo.h"
#include "commands.h"
#include "display.h"

/**
 * Appelée lors Ctrl-C.
 * S'occupe du nettoyage...
 */
void terminate_program(int n)
{
    Edbg(("terminate_program()"));

    destroy_cameras();
    destroy_display();
    
    Rdbg(("terminate_program"));

    exit(0);
}

/**
 * Même fonction que terminate_program mais adaptée pour atexit.
 * S'occupe du nettoyage...
 */
void terminate_program_bis()
{
    terminate_program(0);
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
           "  -h\t\t\taffiche cet écran\n"
           "  -g\t\t\tdisplay in OpenGL\n"
           "  -a <nb_keyframes>\tnombre de keyframes dans l'animation en OpenGL"
           " (plus la valeur est petite, plus l'animation est rapide)\n"
           "  -z <zoom>\t\tzoom à appliquer aux images\n");
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
	static struct sigaction act;

    /* on récupère les options */
    while ( (c = getopt(argc, argv, "hga:z:")) >= 0) 
    {
        switch (c) 
        {
            case 'h':
                /* print help */
                print_usage();
                Rdbg(("main return EXIT_SUCCESS"));
                return EXIT_SUCCESS;
            case 'g':
                /* display in OpenGL */
                display_opengl = TRUE;
                break;
            case 'z':
                /* display in OpenGL */
                g_zoom = strtod(optarg, (char **)NULL);
                break;
            case 'a':
                /* nombre de keyframes dans l'animation OpenGL */
                g_anim_nb_keyframes = strtol(optarg, (char **)NULL, 10);;
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

	/* on catche le Ctrl-C */
	act.sa_handler = terminate_program;
	sigfillset(&(act.sa_mask));
	sigaction(SIGINT, &act, NULL);

    /* on charge les commandes */
    ret = execute_commands(argv[optind]);
    if (ret != RETURN_SUCCESS)
    {
        destroy_cameras();
        return EXIT_FAILURE;
    }

    /* on vérifie qu'il y a bien au moins 2 caméras chargées */
    /* if (g_cameras.nb < 2)                                         */
    /* {                                                             */
    /*     fprintf(stderr, "Il faut charger au moins 2 caméras!\n"); */
    /*     destroy_cameras();                                        */
    /*     return EXIT_FAILURE;                                      */
    /* }                                                             */

    /* affichage */
    if (TRUE == display_opengl)
    {
        /* on utilise atexit car glut ne dit pas quand il quitte le programme
         * (ce gros malin...) 
         * cf 3.070 http://users.frii.com/martz/oglfaq/glut.htm
         * update: depuis j'utilise SDL mais je garde quand même atexit... */
        atexit(terminate_program_bis);

        init_display();
        start_display();
        destroy_display();
    }

    /* nettoyage */
    destroy_cameras();

    /* debug trace end */
    Rdbg(("main EXIT_SUCCESS"));
    return EXIT_SUCCESS;
}

/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
