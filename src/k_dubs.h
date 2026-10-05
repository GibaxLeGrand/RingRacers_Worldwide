// DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) 2025 by Kart Krew.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  k_dubs.h
/// \brief Character voice dubs, chosen by the one listening (WORLDWIDE.md 8.142)
///
/// A dub is another set of a character's voice lines -- the twelve sounds an
/// S_SKIN maps (DSKWIN, DSKLOSE, DSKHURT1...) -- under a name, "Japanese" say.
/// Whoever listens chooses: a character's voice at character select, as a
/// profile is chosen (voicedubs), and a name for every other character in the
/// sound options (voicelanguage). A dub's missing lines are the character's
/// own. Nothing goes over the network: it changes what this machine plays,
/// never the game.
///
/// A dub comes in a DUBDEF lump, written as an S_SKIN's sound lines:
///     skin = sonic
///     name = Japanese
///     DSKWIN = DSSNJWIN
/// A file holding only DUBDEFs and sounds is not important (W_VerifyNMUSlumps):
/// it is loaded on this machine alone, even in a netgame, and never enters a
/// server's file list. Those in <home>/dubs are loaded at start-up.

#ifndef __K_DUBS__
#define __K_DUBS__

#include "doomtype.h"
#include "command.h"
#include "sounds.h"
#include "r_skins.h"

#ifdef __cplusplus
extern "C" {
#endif

// The listener's choice: a dub's name, or "Default". Saved, any string, so a
// choice outlives a session that does not load its dub.
extern consvar_t cv_voicelanguage;

// Each character's own choice, made at character select and saved:
// "sonic=Japanese,tails=Default". It comes before voicelanguage.
extern consvar_t cv_voicedubs;

// The menu's list of voicelanguage: "Default" and every dub name loaded.
extern consvar_t cv_dummyvoicelanguage;
extern CV_PossibleValue_t dublanguage_cons_t[];

/** Reads every DUBDEF lump of a file. */
void K_LoadDubDefs(UINT16 wadnum);

/** Reads the DUBDEFs of every file loaded at start-up. */
void K_InitDubDefs(void);

/** The sound a character's voice line plays: the chosen dub's, if it has
  * that line for that character, or the character's own. */
sfxenum_t K_DubSkinSound(const skin_t *skin, INT32 skinsound);

/** How many dubs a character has; its choices are these and Default. */
INT32 K_DubCount(const skin_t *skin);

/** A character's choice n: 0 is "Default", then its dubs as loaded. */
const char *K_DubName(const skin_t *skin, INT32 n);

/** The choice a character speaks with now, as an n of K_DubName. */
INT32 K_DubChosen(const skin_t *skin);

/** Choice n made the character's own, and saved (voicedubs). */
void K_DubChoose(const skin_t *skin, INT32 n);

/** A character's voice line in choice n, to hear it before choosing. */
sfxenum_t K_DubPreview(const skin_t *skin, INT32 n, INT32 skinsound);

/** The menu's list set to the saved choice, when the menu opens. */
void K_DubMenuSync(void);

/** The menu's choice made the saved one. */
void K_DubMenuChanged(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __K_DUBS__
