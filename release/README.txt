RING RACERS WORLDWIDE -- v0.1.0-alpha
=====================================

Client-side prediction netcode for Dr. Robotnik's Ring Racers 2.4.
An unofficial fork, by Gibax. Not affiliated with or endorsed by Kart Krew
Dev: please never report this fork's bugs to them.

This is an ALPHA, the first public test build. Things will break: please
tell us when they do (see REPORTING A PROBLEM below).


WHAT IT DOES
------------

In stock Ring Racers online, nobody's input counts until the server has it,
so you feel your own ping as input delay. On a server in WORLDWIDE mode,
Ring Racers Worldwide shows you a prediction instead: your kart answers your
input at once, the world you see runs a few tics ahead of what the server
has confirmed, and the server's word corrects it. The server stays in
charge of the race.

On a stock 2.4 server, it plays exactly as a stock 2.4.


WHAT YOU NEED
-------------

- Ring Racers 2.4, installed: https://www.kartkrew.org (or, for the
  Flatpak, the official Ring Racers on Flathub). This download carries none
  of the game's data.
- Windows: 64-bit.
- Linux, the tarball: x86_64, glibc 2.35 or newer, and your system's SDL2
  (libsdl2-2.0-0 on Debian and Ubuntu, sdl2-compat on Arch and SteamOS,
  SDL2 on Fedora).
- Linux, the Flatpak, and the Steam Deck: Flatpak, with Flathub.


WHAT IS IN THE DOWNLOAD
-----------------------

  ringracers_worldwide.exe   the game (ringracers_worldwide and its lib
                             folder in the Linux tarball)
  worldwide.pk3              WORLDWIDE's title screen; optional
  README.txt                 this file
  LICENSE.txt                the GNU General Public License, version 2

The Flatpak is a single file, worldwide.pk3 inside.


INSTALLING
----------

Windows

  1. Open your Ring Racers 2.4 folder: the one with ringracers.exe and
     bios.pk3 in it.
  2. Copy ringracers_worldwide.exe there, next to ringracers.exe.
  3. Copy worldwide.pk3 into the "data" folder inside it.
  4. Start ringracers_worldwide.exe.

  Your ringracers.exe stays as it was. Both use the same profiles,
  progress and settings, so you can switch between them.

Linux, the tarball

  1. Unpack it, and copy ringracers_worldwide and the lib folder into the
     folder that holds the Ring Racers 2.4 data (bios.pk3 and the "data"
     folder).
  2. Copy worldwide.pk3 into that "data" folder.
  3. Run ./ringracers_worldwide from there -- or, from anywhere, with
     RINGRACERSWADDIR set to that folder.

  Settings, saves and logs go to ~/.ringracers.

Linux, the Flatpak

  1. The official Ring Racers, from Flathub, for its data:
       flatpak install --user flathub org.kartkrew.RingRacers
  2. Ring Racers Worldwide:
       flatpak install --user RingRacersWorldwide-v0.1.0-alpha.flatpak
  3. Start "Ring Racers Worldwide" from your menu, or:
       flatpak run io.github.ringracers_worldwide.RingRacersWorldwide

  Settings, saves and logs go to
  ~/.var/app/io.github.ringracers_worldwide.RingRacersWorldwide/.ringracers

Steam Deck

  1. In desktop mode, open a terminal (Konsole) and follow "Linux, the
     Flatpak" above.
  2. In Steam, still in desktop mode: Add a Game > Add a Non-Steam Game,
     and pick Ring Racers Worldwide.
  3. Back in game mode, it is in your library.


PLAYING ONLINE
--------------

Joining
  Play > Online > Browse, or Direct Join with the server's address. On a
  WORLDWIDE server, the prediction turns itself on: you have nothing to
  set. On a stock 2.4 server, you play the stock netcode.

Hosting
  Play > Online > Host Game. WORLDWIDE mode is on by default, and the host
  screen says so ("WORLDWIDE: On"). Only Ring Racers Worldwide players of
  the same version can join: a stock 2.4 player is refused, with a message
  saying why. For players outside your network, forward UDP port 5029 to
  your machine.

The public server list
  Turn on "Public Server" on the host screen. The game first shows Kart
  Krew's Server List Rules: it is their list, and their rules apply to
  WORLDWIDE servers too.

Hosting for stock 2.4 players
  Before anybody joins, turn off Options > Server Options > Advanced... >
  Network Connection > WORLDWIDE Mode, or start the game with
  +worldwide Off. You then host as a stock 2.4 would. The setting is saved:
  turn it back on there for your next WORLDWIDE game.

A dedicated server
  ringracers_worldwide -dedicated +worldwide On

Versions
  WORLDWIDE builds meet the same WORLDWIDE version only. An older or newer
  one is refused at the join, with a message saying which side to update.


ALSO IN THIS BUILD
------------------

None of these change the race or reach a stock server.

- Character dubs: a character can have more than one voice -- Sonic in
  Japanese, say -- and each pilot picks theirs at character select. Dub
  packs go in the "dubs" folder next to "addons" (none comes with this
  download). Options > Sound > Voice Language is the voice for bots and
  for pilots who picked none. Online, in WORLDWIDE mode, the others hear
  your kart in your dub if they have the same pack.
- Two players side by side: Options > HUD > 2P Splitscreen > Vertical.
- Photo mode: in the pause menu, offline and in replays.
- Gyro steering, for controllers with motion sensors: Options > Profile
  Setup > (your profile) > Accessibility > Gyro Steering.
- Resolutions up to 4K, and your desktop's own: Options > Video.
- Typing with your keyboard's layout (AZERTY and the others) in the
  console, the chat and the menus.
- WORLDWIDE Input Delay, in Profiles > Accessibility: holds your own input
  back a few tics on your machine only, for a steadier picture. 0, the
  default, changes nothing.


KNOWN ISSUES
------------

- Two people on two machines over the Internet: not raced yet. Until now,
  one driver with bots, on simulated latency, jitter and packet loss, and
  races over a LAN. Your reports are what this alpha is for.
- Other people are guessed until their real input arrives (their last
  input, repeated), so their karts can jump a little when it does.
- Above about 340 ms of round trip, the world you see trails your newest
  input by a few tics.
- On slower machines, a wrong guess makes the game run several tics again
  at once: a short hitch.
- Only Race has been played in WORLDWIDE mode. Battle, Grand Prix and
  Encore are untested.
- No replay is recorded while you play with prediction.
- On Windows, a crash writes no .rpt report: this build is 64-bit and the
  2.4 folder's crash handler is 32-bit. Send latest-log.txt instead.
- Character dubs between two machines are not tried yet.
- In Legacy GL, fullscreen draws at your desktop's size: 4K there needs a
  4K desktop. The software renderer at 4K is untested.


REPORTING A PROBLEM
-------------------

On the Ring Racers Worldwide Discord: TODO-DISCORD-LINK
Or as an issue: https://github.com/GibaxLeGrand/RingRacers_Worldwide/issues

Please say:
- what happened, and what you expected;
- the map, how many players, and whether the server was WORLDWIDE or stock;
- your system (Windows, Linux, Steam Deck) and how you connect (LAN,
  Internet, roughly how far from the server);

and send the latest-log.txt of that session, before you start the game
again (it is replaced at each start; earlier ones are in the "logs" folder
next to it):

  Windows      in your Ring Racers folder
  Tarball      ~/.ringracers/latest-log.txt
  Flatpak      ~/.var/app/io.github.ringracers_worldwide.RingRacersWorldwide/.ringracers/latest-log.txt


LICENSE AND SOURCE
------------------

Ring Racers Worldwide is free software, under the GNU General Public
License, version 2 or later: see LICENSE.txt. Its source code, this
version's included, is at

  https://github.com/GibaxLeGrand/RingRacers_Worldwide

(this release: the tag v0.1.0-alpha).

Dr. Robotnik's Ring Racers is by Kart Krew Dev: https://www.kartkrew.org
It is a work of fan art made available for free without intent to profit or
harm the intellectual property rights of the original works it is based on.
Kart Krew Dev is in no way affiliated with SEGA Corporation. Ring Racers
Worldwide is an unofficial modification of it, made under the same terms,
and not affiliated with or endorsed by Kart Krew Dev or SEGA.
