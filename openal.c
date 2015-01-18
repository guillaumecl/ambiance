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
#include <malloc.h>

#include <al.h>
#include <alc.h>

#include <sndfile.h>

#include "log.h"
#include "openal.h"

#define BUFFER_COUNT 5

struct sound
{
	SNDFILE* file;
	SF_INFO file_infos;

	ALsizei nb_samples, sample_rate;
	ALenum format;
	ALuint buffers[BUFFER_COUNT];

	ALuint source;

	unsigned int curbuf;
};

int init_al()
{
	ALCdevice* dev = alcOpenDevice(NULL);
	if (!dev)
		return 1;

	// Création du contexte
	ALCcontext* ctx = alcCreateContext(dev, NULL);
	if (!ctx)
		return 1;

	if (!alcMakeContextCurrent(ctx))
		return 1;

	return 0;
}

void destroy_al()
{
	ALCcontext *ctx = alcGetCurrentContext();
	ALCdevice *dev = alcGetContextsDevice(ctx);

	alcMakeContextCurrent(NULL);
	alcDestroyContext(ctx);
	alcCloseDevice(dev);
}


void read_more(struct sound *s)
{
	unsigned int read;
	ALshort samples[4096];

	read = sf_read_short(s->file, samples, sizeof(samples) / sizeof(*samples));

	if (read == 0)
		return;

	alBufferData(s->buffers[s->curbuf], s->format, samples,
		read * sizeof(ALushort), s->sample_rate);

	s->curbuf = (s->curbuf + 1) % BUFFER_COUNT;
}

struct sound *load_sound(const char *name)
{
	struct sound *s = calloc(1, sizeof(struct sound));
	if (s == NULL)
		return NULL;

	s->file = sf_open(name, SFM_READ, &s->file_infos);

	s->nb_samples = s->file_infos.channels * s->file_infos.frames;
	s->sample_rate = s->file_infos.samplerate;

	log("Sample count: %ud", s->nb_samples);

	if (s->file_infos.channels == 1)
		s->format = AL_FORMAT_MONO16;
	else
		s->format = AL_FORMAT_STEREO16;

	alGenSources(1, &s->source);
	if (alGetError() != AL_NO_ERROR)
		goto err_free;

	alGenBuffers(BUFFER_COUNT, s->buffers);
	if (alGetError() != AL_NO_ERROR)
		goto err_free;

	read_more(s);

	return s;
err_free:
	free_sound(s);
	return NULL;
}

void free_sound(struct sound *s)
{
	if (!s)
		return;

	if (s->file)
		sf_close(s->file);

	alDeleteBuffers(BUFFER_COUNT, s->buffers);
	alDeleteSources(1, &s->source);

	free(s);
}
