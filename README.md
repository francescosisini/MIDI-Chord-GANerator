# MelodyCraft

MelodyCraft is a tool for generating MIDI files with chord progressions, bass lines, drum patterns, and melodies. Users can specify different sections of a song (e.g., intro, verse, chorus) and define the song structure by arranging these sections.

## Features

- Generate chord progressions
- Add bass lines
- Add drum patterns
- Generate melodies that follow the key
- Define song sections and structure

## Installation

1. Clone the repository:
    ```bash
    git clone https://github.com/francescosisini/MIDI-Chord-GANerator.git
    cd MIDI-Chord-GANerator
    ```

2. Install dependencies:
    ```bash
    sudo apt-get install libsmf-dev libglib2.0-dev
    ```

3. Compile the project:
    ```bash
    make
    ```

## Usage

Run the MelodyCraft executable with the desired options:

```bash
./MelodyCraft [options] [section] chord duration ... [section] chord duration ... [structure] section repeat ...

Options

    -t BPM Set the BPM (default: 90)
    -m meter Set the meter (3 or 4, default: 4)
    -r repeats Set the number of repeats for the whole song (default: 1)
    -b Include bass
    -d Include drums
    -g Generate melody
    -k Follow key for melody
    -p piano_volume Set the piano volume (0-127, default: 100)
    -v drum_volume Set the drum volume (0-127, default: 100)
    -q bass_volume Set the bass volume (0-127, default: 100)
    -w melody_volume Set the melody volume (0-127, default: 100)
    -o filename Set the output filename (default: output.mid)
    -h Show the help message

./MelodyCraft -t 120 -m 4 -b -d -g -k -p 100 -v 100 -q 100 -w 100 -o my_song.mid [INTRO] C 4 G 4 [VERSO] Am 4 Em 4 [RITORNELLO] C 4 A 4 F 4 G 4 [CANZONE] INTRO 1 VERSO 2 RITORNELLO 1 VERSO 1 RITORNELLO 2

In this example:

    The BPM is set to 120.
    The meter is 4/4.
    Bass, drums, and melody generation are included.
    The melody follows the key.
    Volumes for piano, drums, bass, and melody are set to 100.
    The output file is named my_song.mid.

The sections defined are:

    INTRO with chords C and G, each lasting 4 beats.
    VERSO with chords Am and Em, each lasting 4 beats.
    RITORNELLO with chords C, A, F, and G, each lasting 4 beats.

The song structure is:

    INTRO repeated once.
    VERSO repeated twice.
    RITORNELLO repeated once.
    VERSO repeated once.
    RITORNELLO repeated twice.

Contributions

Contributions are welcome! Please fork the repository and submit a pull request.