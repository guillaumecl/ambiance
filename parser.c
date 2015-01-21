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
