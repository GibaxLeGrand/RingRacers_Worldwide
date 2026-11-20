# About AI

## A word from Gibax

**This project is AI-assisted, and there is no need to pretend otherwise.**

I don't agree with using AI for art. Placeholder art on a small indie project
is the one use I can defend: not everyone can pay an artist. Hell, I did it
myself: the custom font of the "WORLDWIDE" lettering was made with AI, and
even this ugly Earth and Ring, and for that, I express my deepest apologies.

I fully expect people to be disappointed and leave the moment they see "AI"
on this project. I don't blame them: with how much slop AI has brought, I can
only understand and agree: most projects seem to not do. Still, I do believe
this project is one of the few decent ones. That is not my call to make,
though. So I'm asking you to at least try it, and then decide whether it's
truly slop or not.

I also know how the SRB2 Message Board and the SRB2 community see AI, and that
it is mostly negative, largely because of art theft. But I'll be honest: as a
programmer, I don't see generative AI as a horrible thing for coding. I was
never the biggest fan of typing, copying and pasting solutions from
Stackoverflow to solve an issue. Coding was, most of the time, a chore where
70/80% of your lines typed weren't the interesting stuff. Art and other
creative media are an entirely different subject, but I do believe AI is
helpful for code and speeds up the process by quite a margin, allowing faster
devs time. I don't believe AI is very "bright", as it can hallucinate results
on its own. It's more like an incredibly quick coder who can write efficiently
what you direct.

I am not associated with Kart Krew or Sonic Team Jr. This is a purely
independent fork. I don't want to be associated with them because of my use
of AI: I don't know their view on it, and I don't want my choices to drag
them into a shitstorm. I think I've made my stance clear, and I completely
understand if they don't support this project at all because of its use of
AI. But I won't take it down because they don't like it: the code is open
source, under the GNU General Public License, which lets anyone modify it and
share it as long as the result is published under the same license -- and
that is exactly what this fork does. That said, I am fully open to
constructive criticism on this, and anyone is welcome to take this work and
rework it their own way.

Over the years, and despite the games' popularity, I haven't seen anyone take
on the delay-based netcode inherited from Doom Legacy. It has clearly shown
its age, especially now that fast-paced games are everywhere and rollback
patches are being applied to everything (and thank god for that -- BBCF, my
beloved). Sure, there have been a few improvements along the way, but other
than that, it is pretty much the same stock lockstep, delay-based netplay as
SRB2's.

The closest thing to this I've seen is SRB2 NetPlus by LXShadow: a first step
toward client-side prediction, but a client-only fix, with plenty of room for
desyncs. Inspired by it, I decided to put some of my knowledge on the subject
to good use, and to modify Ring Racers' netcode to implement client-side
prediction, to smooth out the gameplay, especially at higher pings.

To be fair, though, my knowledge of C and C++ is pretty bare, just the
minimum, and a project of this scale was not something I could have taken on
alone: progress would have been too slow and I would have (most certainly)
lost motivation to finish the product quite early on. I also wanted to test
the limits of AI on this kind of project (sadly, with the state of the
programming world, it is becoming clear that you need it just to compete). So
at first it was only a test, to see how far AI could take a project like
this. After a while, it became a serious project I wanted to spend time on
and perfect.

Do I expect a ton of bugs and problems? Yes. In fact, I want people to report
the bugs and problems they run into: I want this fork to be the best it can
be, and for that I need your help. Will people boycott it because it's AI?
Probably, yes. Sadly, I expect it to flop hard, despite the real step forward
it brings to the SRB2 engine in general.

Thank you, Kart Krew, for your dedicated work throughout the years. I love
SRB2Kart, I've started to love Ring Racers, and despite what I said above,
which may have sounded negative: if anyone on the team could test this and
report bugs or give constructive criticism, it would be far more than I could
ever have asked for. I don't need praise; I just want to make a decent
product for the players. But if you decide to boycott it instead, and
(probably rightfully) judge it as "AI slop", I completely understand, and I
won't hold it against you, or against anyone else for that matter.

If you've read this far: first of all, thank you. I want you to know that I
did this for the love of the game, for the love of SRB2 and Ring Racers. I'm
not trying to take any credit or fame from it -- frankly, I don't care about
that. This was just a project for fun, and I wanted to share it with the
world.

-- Gibax

## How the AI is used, in practice

The assistant is Anthropic's Claude, through Claude Code, working under rules
written into the repository's agent instructions:

- **The AI assists; Gibax decides.** It reads the Ring Racers code base,
  traces a mechanism down to a line, writes code and test instruments, and
  drafts the documentation. Choosing between solutions, and what is kept or
  dropped, is Gibax's.
- **Nothing runs without Gibax's go-ahead, every time.** Gibax runs the tests
  and drives the races: the feel of a race is judged by a person, never
  inferred from a log.
- **Measure, don't assume.** Every change is tested on a binary verified by
  its hash, with the prediction written down *before* the run and a control
  in the same session.
- **Mistakes stay visible.** The journal ([docs/WORLDWIDE.md](docs/WORLDWIDE.md))
  is annotated, never rewritten: when a later result overturns an earlier
  one, the earlier one gets a pointer forward. Many of the wrong predictions
  in it are the AI's.
- **Commits written with the AI say so** in their trailer.
