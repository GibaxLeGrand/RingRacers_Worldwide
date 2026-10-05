// DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) 2025 by Kart Krew.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  k_dubs.c
/// \brief Character voice dubs, chosen by the one listening (WORLDWIDE.md 8.142)

#include "doomdef.h"
#include "k_dubs.h"
#include "command.h"
#include "console.h"
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
					*c = '_'; // voicedubs keeps "skin=Name" pairs between commas
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
  * has; and the one chosen (voicelanguage). */
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
}

void K_InitDubDefs(void)
{
	UINT16 i;

	COM_AddCommand("dublist", Command_DubList_f);

	for (i = 0; i < numwadfiles; i++)
		K_LoadDubDefs(i);
}

/** A character's own choice in voicedubs ("skin=Name,skin=Name"), into
  * out; false if it has none. */
static boolean K_DubChoiceOf(const char *skinname, char *out, size_t outlen)
{
	const char *s = cv_voicedubs.string;
	const size_t len = strlen(skinname);

	while (s != NULL && *s != '\0')
	{
		const char *comma = strchr(s, ',');
		const char *end = (comma != NULL) ? comma : s + strlen(s);
		const char *eq = strchr(s, '=');

		if (eq != NULL && eq < end && (size_t)(eq - s) == len && !strnicmp(s, skinname, len))
		{
			size_t n = (size_t)(end - (eq + 1));

			if (n >= outlen)
				n = outlen - 1;
			memcpy(out, eq + 1, n);
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

/** The name a character speaks with: its own choice, else voicelanguage. */
static const char *K_DubNameFor(const skin_t *skin, char *buf, size_t buflen)
{
	if (K_DubChoiceOf(skin->name, buf, buflen))
		return buf;

	return cv_voicelanguage.string;
}

sfxenum_t K_DubSkinSound(const skin_t *skin, INT32 skinsound)
{
	char buf[DUBNAMESIZE+1];
	const char *name;
	const dub_t *dub;

	if (g_numdubs == 0)
		return skin->soundsid[skinsound];

	name = K_DubNameFor(skin, buf, sizeof buf);
	if (name == NULL || !stricmp(name, "Default"))
		return skin->soundsid[skinsound];

	dub = K_DubFind(skin->name, name);
	if (dub != NULL && dub->sound[skinsound] != sfx_None)
		return dub->sound[skinsound];

	return skin->soundsid[skinsound];
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

INT32 K_DubChosen(const skin_t *skin)
{
	char buf[DUBNAMESIZE+1];
	const char *name = K_DubNameFor(skin, buf, sizeof buf);
	const INT32 count = K_DubCount(skin);
	INT32 n;

	for (n = 1; n <= count; n++)
	{
		if (name != NULL && !stricmp(K_DubName(skin, n), name))
			return n;
	}

	return 0;
}

void K_DubChoose(const skin_t *skin, INT32 n)
{
	char out[1024] = "";
	const char *s = cv_voicedubs.string;
	const size_t len = strlen(skin->name);

	// The others' choices kept, this one's replaced.
	while (s != NULL && *s != '\0')
	{
		const char *comma = strchr(s, ',');
		const size_t entry = (comma != NULL) ? (size_t)(comma - s) : strlen(s);

		if (!(entry > len && s[len] == '=' && !strnicmp(s, skin->name, len))
			&& strlen(out) + entry + 2 < sizeof out)
		{
			if (out[0] != '\0')
				strlcat(out, ",", sizeof out);
			strncat(out, s, entry);
		}

		s = (comma != NULL) ? comma + 1 : NULL;
	}

	if (strlen(out) + len + DUBNAMESIZE + 3 < sizeof out)
	{
		if (out[0] != '\0')
			strlcat(out, ",", sizeof out);
		strlcat(out, va("%s=%s", skin->name, K_DubName(skin, n)), sizeof out);
	}

	CV_Set(&cv_voicedubs, out);
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
