/*
 * File:     fastereo.c
 * Author:   Jean-François LE BERRE (leberrej@iro.umontreal.ca)
 * Date:     12 april 2004
 * Version:  0.1
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
    printf("usage: fastereo <start_im> <start_dm> <end_im> <end_dm> <nb_labels>"
           " <distance> <interpol> <result_img>\n"
           "\n"
           "options:\n"
           "  -i\t\tfast stereo merging intensities\n"
           "  -m\t\tfast stereo merging depth maps\n"
           "  -x\t\tx coordinate or moving vector\n"
           "  -y\t\ty coordinate or moving vector\n"
           "  -s <nb>\tgenerate a sequence of images\n"
           "  -z\t\tdisplay in OpenGL\n");
    Rdbg(("print_usage"));
}

/**
 * Main function
 * @param argc number of arguments
 * @param argv arrays of arguments
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
    int seq_nb = 0;

    /* on récupère les options */
    while( (c = getopt(argc, argv, "hims:z")) >= 0) 
    {
        switch(c) 
        {
            case 'h':
                /* print help */
                print_usage();
                Rdbg(("main return 0"));
                return 0;
            case 'i':
                /* fast stereo merging intensities */
                mode = INTENSITIES;
                break;
            case 'm':
                /* fast stereo merging intensities */
                mode = DEPTH_MAPS;
                break;
            case 's':
                /* create a sequence of image */
                action = SEQUENCE;
                seq_nb = strtol(optarg, (char **)NULL, 10);
                break;
            case 'z':
                /* display in OpenGL */
                action = OPENGL;
                mode = DEPTH_MAPS;
                break;
        }
    }

    /* debug trace end */
    Rdbg(("main return 0"));
    return 0;
}

/* use 4 spaces as a tab please */
/* vim: set ts=4 sw=4 et cino=t0(0: */
