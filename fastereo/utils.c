/*
 * File:     $RCSfile: utils.c,v $
 * Author:   Jean-François LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/04/13 23:59:49 $
 * Version:  $Revision: 1.1 $
 * ID:       $Id: utils.c,v 1.1 2004/04/13 23:59:49 arutha Exp $
 * Comments:
 */
/**
 * @file: utils.c
 * @brief Fonctions utiles.
 */

#include "dbg.h"
#include "utils.h"

/**
 * Indique si une ligne est un commentaire
 * @param s la ligne qu'il faut tester
 * @return 1 si c'est un commentaire, 0 sinon
 */
int
is_comment(const char *s)
{
    Edbg(("is_comment(s='%s')", s));

    const char *p = s;

    while((*p) == ' ') 
    {
        p++;
    }

    if(((*p) == '#') || ((*p) == '\0')) 
    {
        Rdbg(("is_comment TRUE"));
        return TRUE;
    }

    Rdbg(("is_comment FALSE"));
    return FALSE;
}

/**
 * Supprime le retour à la ligne
 * @param buffer la ligne que l'on doit traiter
 * @return un pointeur sur la chaîne de caractères
 */
char *
strip_eol(char *buffer)
{
    Edbg(("strip_eol(buffer)", buffer));

    if(buffer == NULL)
    {
        Rdbg(("strip_eol NULL"));
        return NULL;
    }

    int l = strlen(buffer);

    if((l-1 >= 0) && (buffer[l-1] == '\n'))
        buffer[l-1] = 0;
    if((l-2 >= 0) && (buffer[l-2] == '\r'))
        buffer[l-2] = 0;

    Rdbg(("strip_eol buffer='%s'", buffer));
    return buffer;
}

/* use 4 spaces as a tab please */
/* vim: set ts=4 sts=4 sw=4 et cino=t0(0: */
