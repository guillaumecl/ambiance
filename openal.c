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

#include <math.h>

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

	unsigned int cur_buf;
};

int init_al()
{
	ALCcontext* ctx;
	ALCdevice* dev;

	dev = alcOpenDevice(NULL);
	if (!dev)
		return 1;

	ctx = alcCreateContext(dev, NULL);
	if (!ctx)
		return 1;

	if (!alcMakeContextCurrent(ctx))
		return 1;

	return 0;
}

int set_position(float x, float y, float z)
{
	alListener3f(AL_POSITION, x, y, z);
	return alGetError();
}

int set_orientation(float angle)
{
	float direction_vect[6];
	direction_vect[0] = sin(angle); //sin(angle)
	direction_vect[1] = 0;
	direction_vect[2] = cos(angle); //cos(angle)
	direction_vect[3] = 0;
	direction_vect[4] = 1;
	direction_vect[5] = 0;

	alListenerfv(AL_ORIENTATION, direction_vect);
	return alGetError();
}

void destroy_al()
{
	ALCcontext *ctx = alcGetCurrentContext();
	ALCdevice *dev = alcGetContextsDevice(ctx);

	alcMakeContextCurrent(NULL);
	alcDestroyContext(ctx);
	alcCloseDevice(dev);
}


int read_more(struct sound *s, int buffer)
{
	unsigned int read;
	ALshort samples[4096];

	read = sf_read_short(s->file, samples, sizeof(samples) / sizeof(*samples));
	log("sf_read_short on source %d returns %d", s->source, read);

	if (read == 0)
		return 1;

	alBufferData(buffer, s->format, samples,
		read * sizeof(*samples), s->sample_rate);
	log("alBufferData on buffer %d source %d returns %d",
		buffer, s->source, alGetError());

	return alGetError() != AL_NO_ERROR;
}

int queue_next(struct sound *s)
{
	ALuint buffer;

	if (!s)
		return 1;

	alSourceUnqueueBuffers(s->source, 1, &buffer);
	if (alGetError() != AL_NO_ERROR)
		return 1;

	if (read_more(s, buffer))
		return 1;

	alSourceQueueBuffers(s->source, 1, &buffer);

	return alGetError() != AL_NO_ERROR;
}

struct sound *load_sound(const char *name)
{
	int i;
	struct sound *s = calloc(1, sizeof(struct sound));
	if (s == NULL)
		return NULL;

	s->file = sf_open(name, SFM_READ, &s->file_infos);

	s->nb_samples = s->file_infos.channels * s->file_infos.frames;
	s->sample_rate = s->file_infos.samplerate;

	log("Sample count: %u", s->nb_samples);

	if (s->file_infos.channels == 1)
		s->format = AL_FORMAT_MONO16;
	else if (s->file_infos.channels == 2) {
		s->format = AL_FORMAT_STEREO16;
		fprintf(stderr, "Warning: %s is stereo, 3D effects are not applied for this source.\n",
			name);
	}
	else
		return NULL;

	alGenSources(1, &s->source);
	if (alGetError() != AL_NO_ERROR)
		goto err_free;

	alGenBuffers(BUFFER_COUNT, s->buffers);
	if (alGetError() != AL_NO_ERROR)
		goto err_free;

	for (i = 0; i < BUFFER_COUNT; ++i)
		read_more(s, s->buffers[i]);

	alSourceQueueBuffers(s->source, BUFFER_COUNT, s->buffers);
	if (alGetError() != AL_NO_ERROR)
		goto err_free;
	log("source: %d\n", s->source);

	return s;
err_free:
	free_sound(s);
	fprintf(stderr, "Cannot open file '%s'\n", name);
	return NULL;
}

void play_sounds(struct sound **s, int count)
{
	int i, j;
	ALuint sources[count];

	for (i = 0, j = 0; i < count; ++i) {
		if (s[i]) {
			sources[j] = s[i]->source;
			++j;
		}
	}
	if (j)
		alSourcePlayv(j, sources);
}

void play_sound(struct sound *s)
{
	alSourcePlay(s->source);
}

void pause_sound(struct sound *s)
{
	alSourcePause(s->source);
}

void free_sound(struct sound *s)
{
	if (!s)
		return;

	if (s->file)
		sf_close(s->file);

	alDeleteSources(1, &s->source);
	alDeleteBuffers(BUFFER_COUNT, s->buffers);

	free(s);
}

int set_source_position(struct sound *s, float x, float y, float z)
{
	alSource3f(s->source, AL_POSITION, x, y, z);
	return alGetError();
}
