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


### Example

./MelodyCraft \
  -t 120 -m 4 -b -d -g -k \
  -p 100 -v 100 -q 100 -w 100 \
  -o my_song.mid \
  [Intro] C 4  G 4 \
  [Verse_A] Am 4  Em 4 \
  [Chorus] C 4  A 4  F 4  G 4 \
  [SONG] Intro 1  Verse_A 2  Chorus 1  Verse_A 1  Chorus 2

What this does

BPM = 120

Meter = 4/4

Includes bass (-b), drums (-d), and melody generation (-g)

Melody follows the key (-k)

Volumes (piano/drums/bass/melody) = 100

Output file: my_song.mid

Sections defined

Intro: C (4 beats), G (4 beats)

Verse_A: Am (4 beats), Em (4 beats)

Chorus: C (4), A (4), F (4), G (4)

Song structure

Intro ×1 → Verse_A ×2 → Chorus ×1 → Verse_A ×1 → Chorus ×2

💡 Section names are free-form. Use any label inside brackets: [Intro], [Verse_A], [Bridge2], etc.

##Chord format (quick ref.)

Roots: C C# D D# E F F# G G# A A# B (anche bemolli: Db Eb Gb Ab Bb)

Suffixes: (maj) m 7 m7 maj7 dim aug 6 m6 sus2 sus4 add9 madd9

sus2 = {root, 2nd, 5th}, sus4 = {root, 4th, 5th}

add9 = major triad + 9th, madd9 = minor triad + 9th

Without suffix ⇒ triade maggiore (es. C)

##Contributions

Contributions are welcome! Please fork the repository and submit a pull request.