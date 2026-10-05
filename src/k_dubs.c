// DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) 2025 by Kart Krew.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  k_dubs.c
/// \brief Character voice dubs, chosen by each pilot (WORLDWIDE.md 8.142, 9.7)

#include "doomdef.h"
#include "k_dubs.h"
#include "byteptr.h"
#include "command.h"
#include "console.h"
#include "d_clisrv.h"
#include "d_netcmd.h"
#include "doomstat.h"
#include "g_game.h"
#include "k_profiles.h"
#include "k_rollback.h" // K_WorldwideServer
#include "i_system.h"
#include "r_skins.h"
#include "sounds.h"
#include "w_wad.h"
#include "z_zone.h"

#define MAXDUBS 128
#define DUBNAMESIZE 31
#define MAXDUBNAMES 32

typedef struct
{
	char skin[SKINNAMESIZE+1];
	char name[DUBNAMESIZE+1];
	sfxenum_t sound[NUMSKINSOUNDS]; // sfx_None: the character's own line
} dub_t;

static dub_t g_dubs[MAXDUBS];
static INT32 g_numdubs;

// What each remote pilot's machine said its kart speaks with (XD_PILOTDUB),
// by player slot -- for a character, so that a choice never reaches another
// character the slot picks later.
typedef struct
{
	char skin[SKINNAMESIZE+1];
	char name[DUBNAMESIZE+1]; // "": no choice, the listener's voicelanguage
} pilotdub_t;

static pilotdub_t g_heard[MAXPLAYERS];

static void Got_PilotDub(const UINT8 **cp, INT32 playernum);

// "Default", then each dub name once, in the order loaded: the menu's list.
// Its strings are the dubs' own, which live as long as the game.
CV_PossibleValue_t dublanguage_cons_t[MAXDUBNAMES + 2] = {{0, "Default"}, {0, NULL}};
static INT32 g_numdubnames = 1;

static void K_AddDubName(const char *name)
{
	INT32 i;

	for (i = 0; i < g_numdubnames; i++)
	{
		if (!stricmp(dublanguage_cons_t[i].strvalue, name))
			return;
	}

	if (g_numdubnames > MAXDUBNAMES)
	{
		CONS_Alert(CONS_WARNING, "Dubs: more than %d names, '%s' left out of the menu\n", MAXDUBNAMES, name);
		return;
	}

	dublanguage_cons_t[g_numdubnames].value = g_numdubnames;
	dublanguage_cons_t[g_numdubnames].strvalue = name;
	g_numdubnames++;
	dublanguage_cons_t[g_numdubnames].value = 0;
	dublanguage_cons_t[g_numdubnames].strvalue = NULL;
}

/** The voice line an S_SKIN key names -- DSKWIN or sfx_kwin -- as a slot of
  * skinsound_t, or -1. The sound it is mapped to comes with it. */
static INT32 K_DubSlot(const char *key, sfxenum_t *base)
{
	sfxenum_t i;
	size_t adjust;

	if ((key[0] == 'D' || key[0] == 'd') && (key[1] == 'S' || key[1] == 's'))
		adjust = 2;
	else if (!strnicmp(key, "sfx_", 4))
		adjust = 4;
	else
		return -1;

	for (i = 0; i < sfx_skinsoundslot0; i++)
	{
		if (S_sfx[i].name == NULL || S_sfx[i].skinsound == -1)
			continue;

		if (!stricmp(S_sfx[i].name, key + adjust))
		{
			*base = i;
			return S_sfx[i].skinsound;
		}
	}

	return -1;
}

static void K_ParseDubDef(UINT16 wadnum, UINT16 lump)
{
	const size_t size = W_LumpLengthPwad(wadnum, lump);
	const char *lumpdata = W_CacheLumpNumPwad(wadnum, lump, PU_CACHE);
	char *text = malloc(size + 1);
	char *lines[NUMSKINSOUNDS];
	sfxenum_t bases[NUMSKINSOUNDS];
	sfxenum_t base = sfx_None;
	char skin[SKINNAMESIZE+1] = "";
	char name[DUBNAMESIZE+1] = "";
	char *stoken, *value;
	dub_t *dub = NULL;
	INT32 i, slot, given = 0;

	if (text == NULL)
		I_Error("K_ParseDubDef: no more free memory\n");

	memcpy(text, lumpdata, size);
	text[size] = '\0';
	memset(lines, 0, sizeof lines);
	memset(bases, 0, sizeof bases);

	// As an S_SKIN is read (R_AddSkins): "key = value" lines, // and # comments.
	stoken = strtok(text, "\r\n= ");
	while (stoken)
	{
		if ((stoken[0] == '/' && stoken[1] == '/') || stoken[0] == '#')
		{
			stoken = strtok(NULL, "\r\n");
			goto next_token;
		}

		value = strtok(NULL, "\r\n= ");
		if (value == NULL)
		{
			CONS_Alert(CONS_WARNING, "DUBDEF in %s: '%s' has no value\n", wadfiles[wadnum]->filename, stoken);
			break;
		}

		if (!stricmp(stoken, "skin"))
		{
			strlcpy(skin, value, sizeof skin);
			strlwr(skin);
		}
		else if (!stricmp(stoken, "name"))
		{
			char *c;

			strlcpy(name, value, sizeof name);
			for (c = name; *c != '\0'; c++)
			{
				if (*c == ',')
					*c = '_'; // pilotdubs keeps "PROFILE/skin=Name" entries between commas
			}
		}
		else if ((slot = K_DubSlot(stoken, &base)) != -1)
		{
			bases[slot] = base;
			lines[slot] = value;
		}
		else
			CONS_Alert(CONS_WARNING, "DUBDEF in %s: '%s' is not a voice line\n", wadfiles[wadnum]->filename, stoken);

next_token:
		stoken = strtok(NULL, "\r\n= ");
	}

	if (skin[0] == '\0' || name[0] == '\0' || !stricmp(name, "Default"))
	{
		CONS_Alert(CONS_WARNING, "DUBDEF in %s needs a skin and a name other than Default\n", wadfiles[wadnum]->filename);
		free(text);
		return;
	}

	// The same dub again, from a later file: its lines replace the earlier ones.
	for (i = 0; i < g_numdubs; i++)
	{
		if (!stricmp(g_dubs[i].skin, skin) && !stricmp(g_dubs[i].name, name))
		{
			dub = &g_dubs[i];
			break;
		}
	}

	if (dub == NULL)
	{
		if (g_numdubs >= MAXDUBS)
		{
			CONS_Alert(CONS_WARNING, "Dubs: more than %d, %s's %s left out\n", MAXDUBS, skin, name);
			free(text);
			return;
		}

		dub = &g_dubs[g_numdubs++];
		memset(dub, 0, sizeof *dub);
		strlcpy(dub->skin, skin, sizeof dub->skin);
		strlcpy(dub->name, name, sizeof dub->name);
	}

	for (slot = 0; slot < NUMSKINSOUNDS; slot++)
	{
		const char *sound = lines[slot];

		if (sound == NULL)
			continue;

		if ((sound[0] == 'D' || sound[0] == 'd') && (sound[1] == 'S' || sound[1] == 's'))
			sound += 2;
		else if (!strnicmp(sound, "sfx_", 4))
			sound += 4;

		dub->sound[slot] = S_AddSoundFx(sound, S_sfx[bases[slot]].singularity, S_sfx[bases[slot]].pitch, true);
		given++;
	}

	K_AddDubName(dub->name);
	CONS_Printf("Dub '%s' for %s: %d voice lines (%s)\n", dub->name, dub->skin, given, wadfiles[wadnum]->filename);
	free(text);
}

void K_LoadDubDefs(UINT16 wadnum)
{
	UINT16 lump = 0;

	while ((lump = W_CheckNumForNamePwad("DUBDEF", wadnum, lump)) != INT16_MAX)
	{
		K_ParseDubDef(wadnum, lump);
		lump++;
	}
}

/** Console command: dublist
  *
  * The dubs loaded, by name and character, with how many voice lines each
  * has; the one chosen (voicelanguage); each profile's (pilotdubs), and
  * what the other pilots' machines said (XD_PILOTDUB). */
static void Command_DubList_f(void)
{
	INT32 i, slot, given;

	CONS_Printf("voicelanguage: %s -- %d dubs loaded\n", cv_voicelanguage.string, g_numdubs);

	for (i = 0; i < g_numdubs; i++)
	{
		for (given = 0, slot = 0; slot < NUMSKINSOUNDS; slot++)
			given += (g_dubs[i].sound[slot] != sfx_None);

		CONS_Printf("  %s: %s, %d of %d voice lines\n", g_dubs[i].name, g_dubs[i].skin, given, NUMSKINSOUNDS);
	}

	CONS_Printf("pilotdubs: %s\n", cv_pilotdubs.string);

	for (i = 0; i < MAXPLAYERS; i++)
	{
		if (playeringame[i] && g_heard[i].skin[0] != '\0')
		{
			CONS_Printf("  heard from %s: %s=%s\n", player_names[i], g_heard[i].skin,
				(g_heard[i].name[0] != '\0') ? g_heard[i].name : "(no choice)");
		}
	}
}

void K_InitDubDefs(void)
{
	UINT16 i;

	COM_AddCommand("dublist", Command_DubList_f);
	RegisterNetXCmd(XD_PILOTDUB, Got_PilotDub);

	for (i = 0; i < numwadfiles; i++)
		K_LoadDubDefs(i);
}

/** A profile's choice for a character in pilotdubs ("GIBAX/sonic=Japanese,
  * ..."), into out; false if it has none. */
static boolean K_PilotChoiceOf(const char *profile, const char *skinname, char *out, size_t outlen)
{
	const char *s = cv_pilotdubs.string;
	const size_t plen = strlen(profile);
	const size_t slen = strlen(skinname);

	while (s != NULL && *s != '\0')
	{
		const char *comma = strchr(s, ',');
		const char *end = (comma != NULL) ? comma : s + strlen(s);

		if ((size_t)(end - s) > plen + slen + 2
			&& s[plen] == '/' && !strnicmp(s, profile, plen)
			&& s[plen + 1 + slen] == '=' && !strnicmp(s + plen + 1, skinname, slen))
		{
			const char *value = s + plen + slen + 2;
			size_t n = (size_t)(end - value);

			if (n >= outlen)
				n = outlen - 1;
			memcpy(out, value, n);
			out[n] = '\0';
			return true;
		}

		s = (comma != NULL) ? comma + 1 : NULL;
	}

	return false;
}

static const dub_t *K_DubFind(const char *skinname, const char *name)
{
	INT32 i;

	for (i = 0; i < g_numdubs; i++)
	{
		if (!stricmp(g_dubs[i].skin, skinname) && !stricmp(g_dubs[i].name, name))
			return &g_dubs[i];
	}

	return NULL;
}

/** The name a kart speaks with: its pilot's choice -- this machine's profile
  * for one of its own pilots, what its machine said for another -- else the
  * listener's voicelanguage. */
static const char *K_DubNameOf(const player_t *player, const skin_t *skin, char *buf, size_t buflen)
{
	if (player != NULL && player >= players && player < players + MAXPLAYERS && !player->bot)
	{
		profile_t *pr = PR_GetPlayerProfile((player_t *)player); // NULL for a replay

		if (pr != NULL)
		{
			if (K_PilotChoiceOf(pr->profilename, skin->name, buf, buflen))
				return buf;
		}
		else if (netgame)
		{
			const pilotdub_t *heard = &g_heard[player - players];

			if (heard->name[0] != '\0' && !stricmp(heard->skin, skin->name))
				return heard->name;
		}
	}

	return cv_voicelanguage.string;
}

sfxenum_t K_DubPilotSound(const player_t *player, const skin_t *skin, INT32 skinsound)
{
	char buf[DUBNAMESIZE+1];
	const char *name;
	const dub_t *dub;

	if (g_numdubs == 0)
		return skin->soundsid[skinsound];

	name = K_DubNameOf(player, skin, buf, sizeof buf);
	if (name == NULL || !stricmp(name, "Default"))
		return skin->soundsid[skinsound];

	dub = K_DubFind(skin->name, name);
	if (dub != NULL && dub->sound[skinsound] != sfx_None)
		return dub->sound[skinsound];

	return skin->soundsid[skinsound];
}

sfxenum_t K_DubSkinSound(const skin_t *skin, INT32 skinsound)
{
	return K_DubPilotSound(NULL, skin, skinsound);
}

INT32 K_DubCount(const skin_t *skin)
{
	INT32 i, count = 0;

	for (i = 0; i < g_numdubs; i++)
		count += !stricmp(g_dubs[i].skin, skin->name);

	return count;
}

static const dub_t *K_DubNth(const skin_t *skin, INT32 n)
{
	INT32 i;

	for (i = 0; i < g_numdubs; i++)
	{
		if (stricmp(g_dubs[i].skin, skin->name))
			continue;

		if (--n == 0)
			return &g_dubs[i];
	}

	return NULL;
}

const char *K_DubName(const skin_t *skin, INT32 n)
{
	const dub_t *dub = (n > 0) ? K_DubNth(skin, n) : NULL;

	return (dub != NULL) ? dub->name : "Default";
}

INT32 K_DubChosenBy(const char *profile, const skin_t *skin)
{
	char buf[DUBNAMESIZE+1];
	const char *name = K_PilotChoiceOf(profile, skin->name, buf, sizeof buf) ? buf : cv_voicelanguage.string;
	const INT32 count = K_DubCount(skin);
	INT32 n;

	for (n = 1; n <= count; n++)
	{
		if (name != NULL && !stricmp(K_DubName(skin, n), name))
			return n;
	}

	return 0;
}

void K_DubChooseFor(const char *profile, const skin_t *skin, INT32 n)
{
	char key[PROFILENAMELEN + SKINNAMESIZE + 3];
	char out[2048] = "";
	const char *s = cv_pilotdubs.string;
	size_t klen;

	snprintf(key, sizeof key, "%s/%s=", profile, skin->name);
	klen = strlen(key);

	// The other choices kept, this profile's for this character replaced.
	while (s != NULL && *s != '\0')
	{
		const char *comma = strchr(s, ',');
		const size_t entry = (comma != NULL) ? (size_t)(comma - s) : strlen(s);

		if (!(entry >= klen && !strnicmp(s, key, klen))
			&& strlen(out) + entry + 2 < sizeof out)
		{
			if (out[0] != '\0')
				strlcat(out, ",", sizeof out);
			strncat(out, s, entry);
		}

		s = (comma != NULL) ? comma + 1 : NULL;
	}

	if (strlen(out) + klen + DUBNAMESIZE + 2 < sizeof out)
	{
		if (out[0] != '\0')
			strlcat(out, ",", sizeof out);
		strlcat(out, key, sizeof out);
		strlcat(out, K_DubName(skin, n), sizeof out);
	}

	CV_Set(&cv_pilotdubs, out);
}

sfxenum_t K_DubPreview(const skin_t *skin, INT32 n, INT32 skinsound)
{
	const dub_t *dub = (n > 0) ? K_DubNth(skin, n) : NULL;

	if (dub != NULL && dub->sound[skinsound] != sfx_None)
		return dub->sound[skinsound];

	return skin->soundsid[skinsound];
}

void K_DubMenuSync(void)
{
	INT32 i;

	for (i = 0; i < g_numdubnames; i++)
	{
		if (!stricmp(dublanguage_cons_t[i].strvalue, cv_voicelanguage.string))
		{
			CV_StealthSetValue(&cv_dummyvoicelanguage, dublanguage_cons_t[i].value);
			return;
		}
	}

	// A choice whose dub is not loaded: shown as Default, kept as it is.
	CV_StealthSetValue(&cv_dummyvoicelanguage, 0);
}

void K_DubMenuChanged(void)
{
	CV_Set(&cv_voicelanguage, cv_dummyvoicelanguage.string);
}

// ----------------------------------------------------------------------------
// Pilots' dubs over the network, WORLDWIDE mode only (WORLDWIDE.md 9.7)
// ----------------------------------------------------------------------------
//
// A pilot's machine knows their choice from their profile; the others learn it
// from XD_PILOTDUB: the character and the dub's name, "" for no choice. Only a
// server in WORLDWIDE mode, and its clients, ever see one -- a stock server
// would kick the sender over a net command it does not know, which is also why
// WORLDWIDE_PROTOCOL went up with it. It never touches the game: a name, kept
// by player slot.
//
// Sent once a frame, outside the tics, so that a tic the prediction runs again
// sends nothing: when a pilot's character or choice changes, and again for
// every pilot of this machine when anybody joins -- the newcomer has heard
// nothing, and a late join brings no history of net commands.

static pilotdub_t g_said[MAXSPLITSCREENPLAYERS]; // this machine's pilots, as last sent
static boolean g_saidonce[MAXSPLITSCREENPLAYERS];
static boolean g_ingame[MAXPLAYERS];              // playeringame, as last seen
static boolean g_serverww;                        // the server joined runs WORLDWIDE mode

void K_DubServerWorldwide(boolean yes)
{
	g_serverww = yes;
}

static boolean K_DubsOnline(void)
{
	if (!netgame || dedicated)
		return false;

	if (server)
		return K_WorldwideServer();

	return (g_serverww && addedtogame);
}

static void Got_PilotDub(const UINT8 **cp, INT32 playernum)
{
	char skin[SKINNAMESIZE+1];
	char name[DUBNAMESIZE+1];

	READSTRINGN(*cp, skin, SKINNAMESIZE);
	READSTRINGN(*cp, name, DUBNAMESIZE);

	if (playernum < 0 || playernum >= MAXPLAYERS)
		return;

	strlcpy(g_heard[playernum].skin, skin, sizeof g_heard[playernum].skin);
	strlcpy(g_heard[playernum].name, name, sizeof g_heard[playernum].name);
}

void K_DubNetUpdate(void)
{
	boolean joined = false;
	INT32 i;

	// A slot left: what it said goes with it. A slot filled: say ours again.
	for (i = 0; i < MAXPLAYERS; i++)
	{
		if (playeringame[i] == g_ingame[i])
			continue;

		if (playeringame[i])
			joined = true;
		else
			memset(&g_heard[i], 0, sizeof g_heard[i]);

		g_ingame[i] = playeringame[i];
	}

	if (!K_DubsOnline())
	{
		memset(g_saidonce, 0, sizeof g_saidonce);
		if (!netgame)
			memset(g_heard, 0, sizeof g_heard);
		return;
	}

	for (i = 0; i <= splitscreen; i++)
	{
		const INT32 pnum = g_localplayers[i];
		const profile_t *pr = PR_GetLocalPlayerProfile(i);
		const skin_t *skin;
		char name[DUBNAMESIZE+1] = "";
		UINT8 buf[SKINNAMESIZE + DUBNAMESIZE + 2];
		UINT8 *p = buf;

		if (pnum < 0 || pnum >= MAXPLAYERS || !playeringame[pnum] || players[pnum].skin >= numskins)
			continue;

		skin = skins[players[pnum].skin];

		if (pr == NULL || !K_PilotChoiceOf(pr->profilename, skin->name, name, sizeof name))
			name[0] = '\0';

		if (g_saidonce[i] && !joined
			&& !stricmp(g_said[i].skin, skin->name) && !strcmp(g_said[i].name, name))
			continue;

		WRITESTRINGN(p, skin->name, SKINNAMESIZE);
		WRITESTRINGN(p, name, DUBNAMESIZE);
		SendNetXCmdForPlayer(i, XD_PILOTDUB, buf, p - buf);

		strlcpy(g_said[i].skin, skin->name, sizeof g_said[i].skin);
		strlcpy(g_said[i].name, name, sizeof g_said[i].name);
		g_saidonce[i] = true;
	}
}
