// DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) 2025 by Kart Krew.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  k_gyro.c
/// \brief Steering by tilting the controller (WORLDWIDE.md 8.144)

#include <math.h>

#include "doomdef.h"
#include "k_gyro.h"
#include "i_joy.h" // JOYAXISRANGE
#include "i_time.h"

#define MAXGYRODEVICES 16
#define GYRO_DEADZONE 2.0f          // degrees of tilt read as none
#define GYRO_BLEND_WITH 0.02f       // a sample's pull toward gravity, with a gyroscope
#define GYRO_BLEND_WITHOUT 0.25f    // ... and without one: the accelerometer alone, smoothed
#define GYRO_STALE (TICRATE/2)      // a device silent this long steers no more
#define RAD2DEG (180.0f / 3.14159265f)

typedef struct
{
	INT32 device;           // 0: a free slot
	boolean haveroll;
	boolean hasgyro;
	float roll;             // degrees; + is clockwise as the player sees it, a right turn
	UINT64 lastgyro;        // microseconds
	tic_t lastsample;
} gyrostate_t;

static gyrostate_t g_gyro[MAXGYRODEVICES];

static gyrostate_t *K_GyroState(INT32 device, boolean create)
{
	gyrostate_t *spare = NULL;
	INT32 i;

	for (i = 0; i < MAXGYRODEVICES; i++)
	{
		if (g_gyro[i].device == device)
			return &g_gyro[i];

		if (spare == NULL && g_gyro[i].device == 0)
			spare = &g_gyro[i];
	}

	if (create == false || spare == NULL)
		return NULL;

	memset(spare, 0, sizeof *spare);
	spare->device = device;
	return spare;
}

static float K_GyroWrap(float degrees)
{
	while (degrees > 180.0f)
		degrees -= 360.0f;
	while (degrees < -180.0f)
		degrees += 360.0f;
	return degrees;
}

void K_GyroSample(INT32 device, INT32 sensor, const float data[3], UINT64 microseconds)
{
	gyrostate_t *s;

	if (device <= 0)
		return;

	s = K_GyroState(device, true);
	if (s == NULL)
		return;

	s->lastsample = I_GetTime();

	if (sensor == GYRO_ACCEL)
	{
		// Gravity in the plane of the controller's face: turned clockwise by
		// an angle, the controller sees it turn the other way, (-sin, cos).
		const float plane = sqrtf(data[0]*data[0] + data[1]*data[1]);
		float roll;

		if (plane < 1.0f)
			return; // lying on its back: no tilt to read

		roll = atan2f(-data[0], data[1]) * RAD2DEG;

		if (s->haveroll == false)
		{
			s->roll = roll;
			s->haveroll = true;
		}
		else
		{
			// Pulled toward gravity's angle: slowly with a gyroscope, which
			// carries the quick turns and leaves the rumble out; faster
			// without one, the accelerometer smoothed.
			const float blend = s->hasgyro ? GYRO_BLEND_WITH : GYRO_BLEND_WITHOUT;
			s->roll = K_GyroWrap(s->roll + blend * K_GyroWrap(roll - s->roll));
		}
	}
	else if (sensor == GYRO_GYRO)
	{
		s->hasgyro = true;

		if (s->haveroll && s->lastgyro != 0 && microseconds > s->lastgyro)
		{
			const float dt = (float)(microseconds - s->lastgyro) / 1000000.0f;

			// A clockwise turn is negative about +Z, toward the player.
			if (dt < 0.1f)
				s->roll = K_GyroWrap(s->roll - data[2] * dt * RAD2DEG);
		}

		s->lastgyro = microseconds;
	}
}

void K_GyroForget(INT32 device)
{
	gyrostate_t *s = K_GyroState(device, false);

	if (s != NULL)
		memset(s, 0, sizeof *s);
}

INT32 K_GyroSteerAxis(INT32 device)
{
	const gyrostate_t *s;
	float range, tilt, amount;

	if (cv_gyrosteer.value == 0 || device <= 0)
		return 0;

	s = K_GyroState(device, false);
	if (s == NULL || s->haveroll == false || I_GetTime() - s->lastsample > GYRO_STALE)
		return 0;

	tilt = (cv_gyrosteer.value == 2) ? -s->roll : s->roll; // 2: Inverted
	amount = fabsf(tilt);
	if (amount <= GYRO_DEADZONE)
		return 0;

	range = (float)cv_gyrorange.value;
	amount = (amount - GYRO_DEADZONE) / (range - GYRO_DEADZONE);
	if (amount > 1.0f)
		amount = 1.0f;

	return (INT32)((tilt > 0.0f ? amount : -amount) * JOYAXISRANGE);
}
