/*
 * File:     $RCSfile: commands.c,v $
 * Author:   Jean-François LE BERRE (leberrej@iro.umontreal.ca)
 *               from University of Montreal
 * Date:     $Date: 2004/04/13 19:07:20 $
 * Version:  $Revision: 1.1 $
 * ID:       $Id: commands.c,v 1.1 2004/04/13 19:07:20 arutha Exp $
 * Comments:
 */

#include "dbg.h"
#include "commands.h"

/**
 * Exécute les commandes contenues dans le fichier "file_name"
 * @param file_name nom du fichier contenant les commandes
 * @return différent de 0 si échec
 */
int
execute_commands(file_name)
    const char *file_name;
{
    /* debug trace start */
    Edbg(("execute_commands(file_name=%s)", file_name));

    /* debug trace end */
    Rdbg(("execute_commands 0"));
    return 0;
}

/* use 4 spaces as a tab please */
/* vim: set ts=4 sw=4 et cino=t0(0: */
