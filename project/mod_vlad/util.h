/*
 * util.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au
 */

#ifndef __MODVLAD_UTIL_H
#define __MODVLAD_UTIL_H

/* a version of yyinput that uses apache apr */
int modvlad_apache_yyinput(void *a_stream, char *a_buf, int a_max);

/* a version of yyinput that uses libc fread */
int modvlad_default_yyinput(void *a_stream, char *a_buf, int a_maxsize);

#endif
