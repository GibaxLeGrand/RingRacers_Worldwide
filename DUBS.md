# Character dubs

A character can have more than one voice -- Sonic in Japanese, say -- and
each pilot chooses theirs. Nothing of it touches the race: a voice is not
part of the game's state.

## Choosing

- **At character select**, a character that has dubs gets one more step
  before the colours: *Default*, the game's voice, then each dub loaded, each
  heard as it comes up. The choice is kept, for that character, with the
  profile you race with (`pilotdubs`).
- **Everyone else**: *Options > Sound > Voice Language* (`voicelanguage`,
  `Default` by default) is the voice heard for bots, replays and pilots who
  chose none.
- **Checking**: the console command `dublist` lists the dubs loaded, your
  choices, and what the other pilots' machines said.

## Installing a pack

Put it in the `dubs` folder, next to `addons`:

- Windows: the game's folder;
- Linux, the tarball: `~/.ringracers/dubs`;
- Flatpak: `~/.var/app/io.github.ringracers_worldwide.RingRacersWorldwide/.ringracers/dubs`.

The game makes the folder at its first start. A release's assets zip carries
the packs made for this fork, with their credits (`dubs/CREDITS.txt`).

The packs load at start-up, on this machine alone: a dub pack never enters a
server's file list, is never sent to anyone, and does not make the game count
as modified, so you can join any server with your dubs loaded, a stock one
included. A pack holding anything but sounds and `DUBDEF` lumps is left out,
with a warning in the console.

## Online

In WORLDWIDE mode, your choice -- the character's and the dub's names, never
the sounds -- reaches the other players: they hear your kart in your dub if
they have the same pack, and in the character's own voice if they don't. On a
stock server nothing is sent: your kart speaks with your dub, the others with
your *Voice Language*. Not yet tried between two machines.

## Making a pack

A WAD or PK3 of sounds, with one `DUBDEF` lump per character and dub, written
as an S_SKIN's sound lines:

    skin = sonic
    name = Japanese
    DSKWIN = DSSNJWIN
    DSKLOSE = DSSNJLOS

- The twelve voice lines are `DSKWIN`, `DSKLOSE`, `DSKHURT1`, `DSKHURT2`,
  `DSKATTK1`, `DSKATTK2`, `DSKBOST1`, `DSKBOST2`, `DSKSLOW`, `DSKHITEM`,
  `DSKGLOAT` and `DSKTALK`. A line the dub leaves out stays the character's
  own.
- A sound lump's name is `DS` and six characters at most, as for any sound.
- A dub's name is a single word, without spaces.
- Up to 128 dubs, under 32 different names.

## Dubs for addon characters

A `DUBDEF` can name a character that is not loaded yet: the dub waits in
memory, under the character's name, and applies as soon as the character is
loaded -- by `addfile`, or downloaded from a server -- its step at character
select included. Two things to get right:

- `skin =` is the addon's internal name, the `name` of its S_SKIN, not the
  one shown in the menus (case does not matter). A dub whose name matches no
  character stays unused; `dublist` still shows it.
- Give the dub's sound lumps names of their own: the game plays the most
  recently loaded lump of a name, so an addon loaded after the pack with a
  lump of the same name would be heard instead.

Read in the code; not yet tried with an addon character.
