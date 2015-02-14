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

#include <string.h>
#include <unistd.h>

#include <basedir.h>
#include <basedir_fs.h>

#include "openal.h"
#include "log.h"
#include "parser.h"

#define MAX_SOUNDS 50

xdgHandle xdg;

int main(int argc, char **argv)
{
	struct sound *sounds[MAX_SOUNDS];
	int i;
	int busy = 1;
	int count = 0;
	FILE *config;

	if (argc > 1) {
		config = fopen(argv[1], "r");
		if (!config) {
			fprintf(stderr, "%s: ", argv[1]);
			perror("");
			return 1;
		}
	} else {
		if (!xdgInitHandle(&xdg)) {
			perror("xdgInitHandle failed");
			return 1;
		}

		config = xdgConfigOpen("ambiance/ambiance.conf", "r", &xdg);
		if (!config) {
			fprintf(stderr, "Cannot find a configuration file. \n\n"
				"Please setup one either in :\n\n"
				"\t* /etc/xdg/ambiance/ambiance or\n"
				"\t* $XDG_CONFIG_DIR/ambiance/ambiance.conf "
				"(which typically is located in ~/.config/ambiance/ambiance.conf)\n");
			return 1;
		}
		xdgWipeHandle(&xdg);
	}

	if (init_al()) {
		perror("init_al failed");
		return 1;
	}
	log("OpenAL correctly setup.");

	memset(sounds, 0, sizeof(sounds));

	for (i = 1; i < MAX_SOUNDS; ++i) {
		sounds[count] = parse(config);
		if (sounds[count])
			++count;
	}

	play_sounds(sounds, count);

	float angle = 0;

	while (count) {
		set_orientation(angle);
		busy = 1;
		while (busy) {
			busy = 0;

			for (i = 0; i < count; ++i) {
				if (!queue_next(sounds[i]))
					busy = 1;
			}
		}
		usleep(20000);
		angle += 0.01;
	}

	log("Destroying…");
	for (i = 0; i < count; ++i)
		free_sound(sounds[i]);

	destroy_al();

	fclose(config);

	return 0;
}
