// DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) 2025 by Kart Krew.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  k_gyro.h
/// \brief Steering by tilting the controller (WORLDWIDE.md section 9)
///
/// A controller with motion sensors -- a Steam Deck, a DualSense, a Switch Pro
/// -- turned like a wheel steers as its stick does. Its accelerometer gives
/// where gravity is, so the tilt is absolute: no calibration, no drift; its
/// gyroscope, when it has one, smooths it (a complementary filter), so the
/// rumble does not shake the steering. It is an input, as a stick is: it adds
/// to the stick in the ticcmd, and nothing else changes.

#ifndef __K_GYRO__
#define __K_GYRO__

#include "doomtype.h"
#include "command.h"

#ifdef __cplusplus
extern "C" {
#endif

extern consvar_t cv_gyrosteer;  // Off / On
extern consvar_t cv_gyrorange;  // the tilt, in degrees, for a full turn

#define GYRO_ACCEL 0
#define GYRO_GYRO 1

/** A sample of a controller's sensor: device as the game numbers it (1 + its
  * joystick instance), GYRO_ACCEL in m/s^2 or GYRO_GYRO in rad/s, on SDL's
  * axes (+X right, +Y up, +Z toward the player), at a time in microseconds. */
void K_GyroSample(INT32 device, INT32 sensor, const float data[3], UINT64 microseconds);

/** The device went away: what it last sent is forgotten. */
void K_GyroForget(INT32 device);

/** The steering a device's tilt asks for, as a stick's x axis: -JOYAXISRANGE
  * (left) to JOYAXISRANGE (right); 0 with gyrosteer off, or from a device that
  * sent no motion lately. */
INT32 K_GyroSteerAxis(INT32 device);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __K_GYRO__
