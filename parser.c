/***************************************************************************
 *   Copyright (C) 2015 by Guillaume Clement <gclement@baobob.org>         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>

#include "openal.h"
#include "parser.h"
#include "log.h"

static struct sound *directional(char *buf)
{
	fprintf(stderr, "Directional sources are not implemented yet.\n");
	(void)buf;

	return NULL;
}

static struct sound *omnidirectional(char *buf)
{
	int ret, pos, len;
	float x, y, z;
	char *file;
	struct sound *s;

	ret = sscanf(buf, "%f %f %f %n", &x, &y, &z, &pos);
	if (ret != 3) {
		fprintf(stderr, "Invalid omnidirectional source: %s\n",
			buf);
		return NULL;
	}

	file = buf + pos;
	len = strlen(file);
	if (len == 0) {
		fprintf(stderr, "No file was provided: %s\n", buf);
		return NULL;
	}

	if (file[len - 1] == '\n')
		file[strlen(file) - 1] = 0;

	s = load_sound(file);

	if (!s)
		return NULL;

	set_source_position(s, x, y, z);

	log("Loaded omnidirectional sound %s at x=%f, y=%f, z=%f…", file, x, y, z);

	return s;
}

struct sound *parse_line(char *buf)
{
	int pos, ret;
	char type;

	ret = sscanf(buf, " %c %n", &type, &pos);

	if (ret != 1)
		return NULL;

	switch(type) {
	case '#':
		return NULL;
	case 'O':
	case 'o':
		return omnidirectional(buf + pos);
	case 'D':
	case 'd':
		return directional(buf + pos);
	default:
		pos = strlen(buf);
		if (buf[pos-1] == '\n')
			buf[pos-1] = 0;
		return load_sound(buf);
	}
	return NULL;
}

struct sound *parse(FILE *f)
{
	char buf[4096];

	if (fgets(buf, sizeof(buf), f) == NULL)
		return NULL;
	return parse_line(buf);
}
