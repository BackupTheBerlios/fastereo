/*
 * File:     commands.h
 * Author:   Jean-François LE BERRE (leberrej@iro.umontreal.ca)
 * Date:     12 april 2004
 * Version:  0.1
 * Comments:
 */

#ifndef _COMMANDS_H_
#define _COMMANDS_H_

/**
 * Defines modes
 */
typedef enum {
    INTENSITIES = 1,
    DEPTH_MAPS = 2
} stereo_mode_t;

/**
 * Defines possible actions
 */
typedef enum {
    SIMPLE = 1,
    SEQUENCE = 2,
    OPENGL = 3
} action_t;

int simple_image(const stereo_mode_t mode,
                 const char *start_im,
                 const char *start_dm,
                 const char *end_im,
                 const char *end_dm,
                 const int nb_labels,
                 const float distance,
                 const float interpol,
                 const char *result_im,
                 const char *result_dm);
int merge_depth_maps();

#endif /* _COMMANDS_H_ */

/* use 4 spaces as a tab please */
/* vim: set ts=4 sw=4 et cino=t0(0: */
