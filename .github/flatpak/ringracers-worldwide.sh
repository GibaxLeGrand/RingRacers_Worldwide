#!/bin/bash
# Launcher for the Ring Racers Worldwide Flatpak: the official Flatpak's
# launcher (ringracers.sh on Flathub), with the data looked for outside.

for i in {0..9}; do
	test -S $XDG_RUNTIME_DIR/discord-ipc-$i || ln -sf {app/com.discordapp.Discord,$XDG_RUNTIME_DIR}/discord-ipc-$i;
done

# This build carries no game data. It reads the 2.4 data of the official
# Flatpak (org.kartkrew.RingRacers, Flathub) when it is installed -- for this
# user first, then system-wide -- or the folder RINGRACERSWADDIR names, which
# this app must be allowed to read:
#   flatpak override --user --filesystem=/path/to/data:ro io.github.ringracers_worldwide.RingRacersWorldwide
if [ -z "${RINGRACERSWADDIR}" ]; then
	# Not $XDG_DATA_HOME: in the sandbox, it is this app's own data folder.
	for dir in \
		"$HOME/.local/share/flatpak/app/org.kartkrew.RingRacers/current/active/files/ringracers-data" \
		"/var/lib/flatpak/app/org.kartkrew.RingRacers/current/active/files/ringracers-data"; do
		if [ -d "$dir" ]; then
			export RINGRACERSWADDIR="$dir"
			break
		fi
	done
fi

if [ -z "${RINGRACERSWADDIR}" ]; then
	echo "Ring Racers Worldwide: no game data found. Install Dr. Robotnik's Ring Racers" >&2
	echo "from Flathub (org.kartkrew.RingRacers), or set RINGRACERSWADDIR to a folder" >&2
	echo "holding the 2.4 data and allow this app to read it (flatpak override)." >&2
fi

# WORLDWIDE's title graphics, worldwide.pk3, come with this app. The game reads
# them from data/ in its data folder, which here is the official Flatpak's,
# read only. So it is given a folder of its own, made again at each start:
# links to every file of that data, and worldwide.pk3 in its data/. A data
# folder that has its own worldwide.pk3 is used as it is.
WWPK3=/app/share/ringracers-worldwide/worldwide.pk3
if [ -n "${RINGRACERSWADDIR}" ] && [ -f "$WWPK3" ] && [ ! -e "${RINGRACERSWADDIR}/data/worldwide.pk3" ]; then
	merged="${XDG_DATA_HOME:-$HOME/.local/share}/ringracers-data"
	rm -rf "$merged"
	mkdir -p "$merged/data"
	for f in "$RINGRACERSWADDIR"/*; do
		[ -e "$f" ] && [ "$(basename "$f")" != data ] && ln -s "$f" "$merged/"
	done
	for f in "$RINGRACERSWADDIR"/data/*; do
		[ -e "$f" ] && ln -s "$f" "$merged/data/"
	done
	ln -s "$WWPK3" "$merged/data/worldwide.pk3"
	export RINGRACERSWADDIR="$merged"
fi

exec ringracers "$@"
