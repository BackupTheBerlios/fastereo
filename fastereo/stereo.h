/*
 * File:     $RCSfile: stereo.h,v $
 * Author:   Jean-François LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/04/19 19:03:34 $
 * Version:  $Revision: 1.1 $
 * ID:       $Id: stereo.h,v 1.1 2004/04/19 19:03:34 arutha Exp $
 */
/**
 * @file stereo.h
 * @brief Fonctions liées à la stéréoscopie
 */

#ifndef _STEREO_H_
#define _STEREO_H_

int interpol(float position, char *image, char *depth_map);

#endif /* _CAMERAS_H_ */

/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
