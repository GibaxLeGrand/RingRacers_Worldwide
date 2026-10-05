// DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) 2025 by Kart Krew.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  k_dubs.h
/// \brief Character voice dubs, chosen by each pilot (WORLDWIDE.md 8.142, 9.7)
///
/// A dub is another set of a character's voice lines -- the twelve sounds an
/// S_SKIN maps (DSKWIN, DSKLOSE, DSKHURT1...) -- under a name, "Japanese" say.
/// Each pilot chooses, at character select, for the profile they race with
/// (pilotdubs): their kart speaks with it, two Sonics in two voices. A bot,
/// a pilot who chose nothing, a replay: the listener's choice in the sound
/// options (voicelanguage). A dub this machine does not have, or a line it
/// lacks, is the character's own.
///
/// Offline and in splitscreen, the pilots are this machine's. Online, a
/// pilot's choice reaches the others in WORLDWIDE mode only, as XD_PILOTDUB;
/// with a stock server nothing goes over the network.
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

// Each profile's choice for each character, made at character select and
// saved: "GIBAX/sonic=Japanese,GUEST/sonic=Default". A pilot's comes before
// voicelanguage.
extern consvar_t cv_pilotdubs;

// The menu's list of voicelanguage: "Default" and every dub name loaded.
extern consvar_t cv_dummyvoicelanguage;
extern CV_PossibleValue_t dublanguage_cons_t[];

/** Reads every DUBDEF lump of a file. */
void K_LoadDubDefs(UINT16 wadnum);

/** Reads the DUBDEFs of every file loaded at start-up, and registers
  * XD_PILOTDUB. */
void K_InitDubDefs(void);

/** The sound a kart's voice line plays: its pilot's dub, if it has that line
  * for that character, or the character's own. No pilot (NULL), a bot, a
  * replay: the listener's voicelanguage. */
sfxenum_t K_DubPilotSound(const player_t *player, const skin_t *skin, INT32 skinsound);

/** The same with no pilot: the listener's choice (menus, dialogues). */
sfxenum_t K_DubSkinSound(const skin_t *skin, INT32 skinsound);

/** How many dubs a character has; its choices are these and Default. */
INT32 K_DubCount(const skin_t *skin);

/** A character's choice n: 0 is "Default", then its dubs as loaded. */
const char *K_DubName(const skin_t *skin, INT32 n);

/** The choice a profile's pilot speaks with for a character, as an n of
  * K_DubName: theirs, or the listener's voicelanguage. */
INT32 K_DubChosenBy(const char *profile, const skin_t *skin);

/** Choice n made a profile's own for a character, and saved (pilotdubs). */
void K_DubChooseFor(const char *profile, const skin_t *skin, INT32 n);

/** A character's voice line in choice n, to hear it before choosing. */
sfxenum_t K_DubPreview(const skin_t *skin, INT32 n, INT32 skinsound);

/** The menu's list set to the saved choice, when the menu opens. */
void K_DubMenuSync(void);

/** The menu's choice made the saved one. */
void K_DubMenuChanged(void);

/** Once a frame, outside the tics: in WORLDWIDE mode, tells the others what
  * this machine's pilots speak with -- when it changes, and again when
  * somebody joins, who has heard nothing yet. */
void K_DubNetUpdate(void);

/** Called by a client when it joins: whether the server runs WORLDWIDE mode,
  * the only one XD_PILOTDUB may be sent to. False when it leaves. */
void K_DubServerWorldwide(boolean yes);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __K_DUBS__
