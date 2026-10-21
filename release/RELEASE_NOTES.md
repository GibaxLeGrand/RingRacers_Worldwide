# Ring Racers Worldwide v0.1.0-alpha

The first public test build of Ring Racers Worldwide: client-side
prediction netcode for Dr. Robotnik's Ring Racers 2.4. On a server in
WORLDWIDE mode, your kart answers your input at once, whatever your ping;
the server stays in charge of the race and corrects the prediction. On a
stock 2.4 server, it plays exactly as a stock 2.4.

**This is an alpha.** It has never been raced by two people on two machines
over the Internet: that is what it is for. Please report what you run into
(below). An unofficial fork by Gibax, not affiliated with or endorsed by
Kart Krew Dev: please never report its bugs to them. This project is
AI-assisted; how and why is in the repository's
[README](https://github.com/GibaxLeGrand/RingRacers_Worldwide#about-ai).

## Downloads

You need **Ring Racers 2.4** installed: these carry none of the game's data.

| File | For |
|---|---|
| `RingRacersWorldwide-v0.1.0-alpha-windows-x64.zip` | Windows, 64-bit: into your Ring Racers 2.4 folder |
| `RingRacersWorldwide-v0.1.0-alpha-linux-x64.tar.gz` | Linux x86_64 (glibc 2.35+, the system's SDL2): into a folder with the 2.4 data |
| `RingRacersWorldwide-v0.1.0-alpha.flatpak` | Linux and the Steam Deck, beside the official Ring Racers from Flathub |

Each holds a `README.txt` with the steps, also
[in the repository](https://github.com/GibaxLeGrand/RingRacers_Worldwide/blob/v0.1.0-alpha/release/README.txt).

## What's in it

- **WORLDWIDE mode**, on by default when you host: prediction for every
  WORLDWIDE player who joins; stock 2.4 players are refused with a message
  saying why. Turn it off before anybody joins to host for stock players.
- **The prediction**: your own input applied at once, your inputs still in
  flight replayed, the prediction kept when the server confirms it, smooth
  drawing between corrections, each sound heard once.
- **A light correction channel** in place of the stock full-state resend;
  and a server running late, its machine busy, no longer makes its clients
  correct themselves for its bots.
- **Not netcode**: character dubs chosen by each pilot; two players side by
  side; photo mode; gyro steering; resolutions up to 4K; typing with your
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
- No replay is recorded while you play with prediction.
- On Windows, a crash writes no `.rpt`: send `latest-log.txt`.

The whole list is in `README.txt`.

## Reporting a problem

On the [Ring Racers Worldwide Discord](TODO-DISCORD-LINK), or as an
[issue](https://github.com/GibaxLeGrand/RingRacers_Worldwide/issues): what
happened, the map, how many players, WORLDWIDE or stock server, your system,
and the `latest-log.txt` of that session (where to find it: `README.txt`).

## Source and license

GNU General Public License, version 2 or later. The source of this release
is the tag
[`v0.1.0-alpha`](https://github.com/GibaxLeGrand/RingRacers_Worldwide/tree/v0.1.0-alpha),
on the Ring Racers 2.4 release. Dr. Robotnik's Ring Racers is by
[Kart Krew Dev](https://www.kartkrew.org).
