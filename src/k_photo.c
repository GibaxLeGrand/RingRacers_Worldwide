// DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) 2025 by Kart Krew.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  k_photo.c
/// \brief Photo mode (WORLDWIDE.md 8.145)

#include "doomdef.h"
#include "k_photo.h"
#include "command.h"
#include "d_player.h"
#include "doomstat.h"
#include "g_demo.h"
#include "g_game.h"
#include "p_local.h"
#include "r_main.h"

static boolean g_photo;
static INT32 g_photohud;    // cv_showhud before, put back on leaving

boolean K_PhotoModeAvailable(void)
{
	if (netgame || demo.playback || gamestate != GS_LEVEL || r_splitscreen > 0)
		return false;

	return (players[displayplayers[0]].mo != NULL);
}

boolean K_PhotoModeActive(void)
{
	return g_photo;
}

void K_PhotoModeEnter(void)
{
	if (g_photo || K_PhotoModeAvailable() == false)
		return;

	g_photo = true;
	g_photohud = cv_showhud.value;
	CV_SetValue(&cv_showhud, 0);

	// The camera let go where it is: the free camera of replays and spectators.
	if (camera[0].freecam == false)
		P_ToggleDemoCamera(0);
}

void K_PhotoModeExit(void)
{
	if (g_photo == false)
		return;

	g_photo = false;

	if (camera[0].freecam)
		P_ToggleDemoCamera(0);

	CV_SetValue(&cv_showhud, g_photohud);

	// The chase camera back behind the kart, not where the photo was taken.
	if (gamestate == GS_LEVEL && players[displayplayers[0]].mo != NULL)
		P_ResetCamera(&players[displayplayers[0]], &camera[0]);
}

void K_PhotoModeTicker(void)
{
	if (g_photo == false)
		return;

	if (netgame || gamestate != GS_LEVEL || camera[0].freecam == false)
		K_PhotoModeExit();
}
