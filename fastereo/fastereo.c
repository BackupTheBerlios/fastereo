/*
 * File:     $RCSfile: fastereo.c,v $
 * Author:   Jean-François LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/05/03 14:15:15 $
 * Version:  $Revision: 1.13 $
 * ID:       $Id: fastereo.c,v 1.13 2004/05/03 14:15:15 arutha Exp $
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
 * S'occupe du nettoyage...
 */
void terminate_program()
{
    Edbg(("terminate_program()"));

    destroy_cameras();
    destroy_display();
    
    Rdbg(("terminate_program"));

    exit(0);
}

/**
 * Appelée lors Ctrl-C.
 * S'occupe du nettoyage...
 */
void terminate_program_catch(int n)
{
    terminate_program();
}

/**
 * Prints usage
 */
void
print_usage(void)
{
    Edbg(("print_usage()"));
    printf("Usage: fastereo <file.commands>\n"
           "\n"
           "Options:\n"
           "  -h\t\t\tAffiche cet écran\n"
           "  -g\t\t\tAffichage OpenGL\n"
           "  -a <nb_keyframes>\tNombre de keyframes dans l'animation en OpenGL"
           " (plus la valeur est petite, plus le mouvement est rapide)\n"
           "  -z <zoom>\t\tZoom à appliquer aux images\n"
           "\n"
           "Touches au clavier:\n"
           "  ESC\t\t\tQuitter l'application\n"
           "  Z\t\t\tPasser en plein écran ou revenir en mode fenêtré\n"
           "  A\t\t\tAnime la scène\n"
           "  S\t\t\tActive/Désactive les teintes\n"
           "  D\t\t\tAfficher ou non les cartes de profondeurs à la place des images\n"
           "  M\t\t\tChanger de mode d'affichage: carrés ou triangles\n"
           "  F1, F2, etc\t\tAfficher ou non les images 1, 2, etc.\n"
           );
    Rdbg(("print_usage"));
}

/**
 * Fonction principale
 * @param argc nombre d'arguments
 * @param argv tableau d'arguments
 * @return program exit code
 */
int
main(int argc, char *argv[])
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
                g_zoomim = strtod(optarg, (char **)NULL);
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
	act.sa_handler = terminate_program_catch;
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
        atexit(terminate_program);

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
