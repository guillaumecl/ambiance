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

#ifndef OPENAL_H
#define OPENAL_H

#include <al.h>

struct sound;

int init_al();
void destroy_al();

struct sound *load_sound(const char *file);
void free_sound(struct sound *sound);

void play_sounds(struct sound **sound, int max_sounds);

void play_sound(struct sound *s);
void pause_sound(struct sound *sound);

int queue_next(struct sound *sound);

#endif
