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

#include "openal.h"
#include "log.h"
#include "parser.h"

#define MAX_SOUNDS 50

int main(int argc, char **argv)
{
	struct sound *sounds[MAX_SOUNDS];
	int i;
	int busy = 1;
	int count = 0;

	if (argc <= 1) {
		printf("Usage: ambiance sound1 [sound2...]\n");
		return 1;
	}
	if (init_al()) {
		perror("init_al failed");
		return 1;
	}
	log("OpenAL correctly setup.");

	memset(sounds, 0, sizeof(sounds));

	for (i = 1; i < argc && i < MAX_SOUNDS; ++i) {
		log("Loading file %s…", argv[i]);
		sounds[count] = parse_line(argv[i]);
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

	return 0;
}
