// DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) 2025 by Kart Krew.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  k_photo.h
/// \brief Photo mode (WORLDWIDE.md section 9)
///
/// Offline, from the pause menu: the game held where it is, the HUD hidden,
/// and the camera let go -- the free camera replays and spectators have,
/// driven with the same controls. It leaves from the pause menu, or by
/// letting the camera go back (the button that toggles the free camera).
/// Nothing of the game changes: it is held, as the pause menu holds it.

#ifndef __K_PHOTO__
#define __K_PHOTO__

#include "doomtype.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Whether photo mode can start now: offline, in a level, one view. */
boolean K_PhotoModeAvailable(void);

/** Whether it is on. */
boolean K_PhotoModeActive(void);

void K_PhotoModeEnter(void);
void K_PhotoModeExit(void);

/** Each tic: left when its camera was let go, or the level or the game left. */
void K_PhotoModeTicker(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __K_PHOTO__
