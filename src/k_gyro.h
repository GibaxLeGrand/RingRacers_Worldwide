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

// Each profile's settings, by its name: "GIBAX=1:30" -- the mode (0 Off,
// 1 On, 2 Inverted) and the tilt, in degrees, for a full turn. Kept beside
// the profiles, whose file stock 2.4 reads too.
extern consvar_t cv_profilegyro;

// The profile editor's two lines, "This Profile only".
extern consvar_t cv_dummyprofilegyrosteer;
extern consvar_t cv_dummyprofilegyrorange;

#define GYRO_ACCEL 0
#define GYRO_GYRO 1

/** A sample of a controller's sensor: device as the game numbers it (1 + its
  * joystick instance), GYRO_ACCEL in m/s^2 or GYRO_GYRO in rad/s, on SDL's
  * axes (+X right, +Y up, +Z toward the player), at a time in microseconds. */
void K_GyroSample(INT32 device, INT32 sensor, const float data[3], UINT64 microseconds);

/** The device went away: what it last sent is forgotten. */
void K_GyroForget(INT32 device);

/** The steering a device's tilt asks for, as a stick's x axis: -JOYAXISRANGE
  * (left) to JOYAXISRANGE (right), by the settings of the profile of this
  * machine's player localplayer; 0 with them off, or from a device that sent
  * no motion lately. */
INT32 K_GyroSteerAxis(INT32 device, INT32 localplayer);

/** The profile editor's lines set to a profile's settings (NULL: a new one). */
void K_GyroProfileToMenu(const char *profile);

/** The profile editor's lines kept as a profile's settings. */
void K_GyroProfileFromMenu(const char *profile);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __K_GYRO__
