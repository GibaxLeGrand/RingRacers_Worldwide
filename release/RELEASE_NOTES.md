# Ring Racers Worldwide v0.1.2-alpha

**Mods and dubs, together.** A fix for the alpha of Ring Racers Worldwide:
client-side prediction netcode for Dr. Robotnik's Ring Racers 2.4. On a
server in WORLDWIDE mode, your kart answers your input at once, whatever your
ping; the server stays in charge of the race and corrects the prediction. On
a stock 2.4 server, it plays exactly as a stock 2.4.

**Nothing changes on the wire:** v0.1.0-alpha, v0.1.1-alpha and v0.1.2-alpha
players race together, on each other's servers. **Update if you play with
mods and have dub packs**: that is the case this version fixes.

**This is an alpha.** Please report what you run into (below). An unofficial fork by Gibax, not
affiliated with or endorsed by Kart Krew Dev: please never report its bugs
to them. This project is AI-assisted; how and why is in
[ABOUT-AI.md](https://github.com/GibaxLeGrand/RingRacers_Worldwide/blob/v0.1.2-alpha/ABOUT-AI.md).

## What's new in v0.1.2-alpha

- **Joining a server with mods no longer crashes because of dubs.** With dub
  packs in your `dubs` folder, joining a server that runs addons crashed
  right after the game state loaded (`ReadError ... file empty`) whenever
  its level had scripts -- and the same happened to players without dubs
  joining a host who had them. The dub packs, which each player has or not,
  shifted how the game numbers the addons' files between two machines. They
  no longer count. A server without dub packs, as ours, needs no update; a
  v0.1.1-alpha player who has dubs still needs this version for such
  servers.

## Also since v0.1.0-alpha (in v0.1.1-alpha)

- **Replays, while you play with prediction**: the replay being recorded
  goes back with the world whenever a prediction is corrected, so it holds
  the race as the server confirmed it.
- **Desyncs, seen**: a WORLDWIDE server counts its consistency mismatches
  per player, in its log.
- **No prediction once you have finished**: once your kart has crossed the
  line, or the time is up, your game shows the race as the server confirms
  it.

## Downloads

You need **Ring Racers 2.4** installed: these carry none of the game's data.

| File | For |
|---|---|
| `RingRacersWorldwide-v0.1.2-alpha-windows-x64.zip` | Windows, 64-bit: into your Ring Racers 2.4 folder |
| `RingRacersWorldwide-v0.1.2-alpha-linux-x64.tar.gz` | Linux x86_64 (glibc 2.35+, the system's SDL2): into a folder with the 2.4 data |
| `RingRacersWorldwide-v0.1.2-alpha.flatpak` | Linux and the Steam Deck, beside the official Ring Racers from Flathub |
| `RingRacersWorldwide-v0.1.2-alpha-assets.zip` | every system: WORLDWIDE's title screen (the Flatpak has it already) and the dub packs, with their credits |

Take your system's download and the assets. Each game download holds a
`README-Worldwide.txt` with the steps, also
[in the repository](https://github.com/GibaxLeGrand/RingRacers_Worldwide/blob/v0.1.2-alpha/release/README.txt).
On Windows, both zips' contents copy into your Ring Racers 2.4 folder
without replacing anything of the game's. `SHA256SUMS.txt` holds the
files' checksums.

## Everything else in the alpha

- **WORLDWIDE mode**, on by default when you host: prediction for every
  WORLDWIDE player who joins; stock 2.4 players are refused with a message
  saying why. Turn it off before anybody joins to host for stock players.
- **The prediction**: your own input applied at once, your inputs still in
  flight replayed, the prediction kept when the server confirms it, smooth
  drawing between corrections, each sound heard once.
- **A light correction channel** in place of the stock full-state resend;
  and a server running late, its machine busy, no longer makes its clients
  correct themselves for its bots.
- **Not netcode**: character dubs chosen by each pilot, with Japanese
  voices for 17 of the game's characters and a few addon ones in the
  assets zip (credits in `dubs/CREDITS.txt`); two players side by side;
  photo mode; gyro steering; resolutions up to 4K; typing with your
  keyboard's layout; WORLDWIDE's title screen.

Measured so far, on one machine with simulated latency, jitter and loss, and
over a LAN: no input lag felt from 0 to 428 ms, a confirmed world that stays
exact, sixteen karts to the end of a race on a heavy map at under a quarter
of a tic of prediction work.

## Known issues

- Not raced yet by two people on two machines, nor over the Internet.
- Other people's karts can jump a little when their real input arrives.
- Above about 340 ms of round trip, what you see trails your newest input
  by a few tics.
- On slower machines, a wrong guess can show as a short hitch.
- Only Race has been played: Battle, Grand Prix and Encore are untested.
- Replays recorded with prediction: tested in Race, not yet with mods.
- On Windows, a crash writes no `.rpt`: send `latest-log.txt`.

The whole list is in `README-Worldwide.txt`.

## Reporting a problem

On the [Ring Racers Worldwide Discord](https://discord.gg/SvWgptrUxm), or as an
[issue](https://github.com/GibaxLeGrand/RingRacers_Worldwide/issues): what
happened, the map, how many players, WORLDWIDE or stock server, your system,
and the `latest-log.txt` of that session (where to find it:
`README-Worldwide.txt`).

## Source and license

GNU General Public License, version 2 or later. The source of this release
is the tag
[`v0.1.2-alpha`](https://github.com/GibaxLeGrand/RingRacers_Worldwide/tree/v0.1.2-alpha),
on the Ring Racers 2.4 release. Dr. Robotnik's Ring Racers is by
[Kart Krew Dev](https://www.kartkrew.org).
