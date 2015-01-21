#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>

struct sound;

struct sound *parse_line(char *buf);
struct sound *parse(FILE *f);


#endif
