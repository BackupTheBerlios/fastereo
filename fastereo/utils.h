/*
 * File:     $RCSfile: utils.h,v $
 * Author:   Jean-François LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/04/13 23:59:49 $
 * Version:  $Revision: 1.1 $
 * ID:       $Id: utils.h,v 1.1 2004/04/13 23:59:49 arutha Exp $
 * Comments:
 */
/**
 * @file: utils.h
 * @brief Fonctions utiles.
 */

#ifndef _UTILS_H_
#define _UTILS_H_

#include <string.h>

/** retour de fonction pour dire que tout s'est bien passé */
#define RETURN_SUCCESS 0
/** retour de fonction pour dire qu'il y a eu une erreur */
#define RETURN_FAILED -1
/** condition vraie */
#define TRUE 1
/** condition fausse */
#define FALSE 0

int is_comment(const char *s);
char *strip_eol(char *buffer);

#endif /* _UTILS_H_ */

/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
