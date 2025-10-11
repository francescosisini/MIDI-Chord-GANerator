#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <smf.h>
#include "section.h"
#include "chords.h"
#include "melody.h"
#include "key_detection.h"
#include "melody_generator.h"


// Globals
double SEMIMINIMA;           // Semiminima (1/4)
double MINIMA;               // Minima (1/2)
double CROMA;                // Croma (1/8)
double SEMICROMA;            // Semicroma (1/16)
double SEMIBREVE;            // Semibreve (intera)

typedef enum {
    INTRO,
    VERSO,
    RITORNELLO,
    PONTE,
    FINALE,
    NUM_SECTION_TYPES
} SectionType;

const char* section_names[] = {
    "INTRO",
    "VERSO",
    "RITORNELLO",
    "PONTE",
    "FINALE"
};

SectionType get_section_type(const char* name) {
    for (int i = 0; i < NUM_SECTION_TYPES; i++) {
        if (strcmp(name, section_names[i]) == 0) {
            return i;
        }
    }
    return NUM_SECTION_TYPES;
}


void write_midi_with_melody(char sections[MAX_SECTIONS][MAX_LENGTH], int section_repeats[MAX_SECTIONS], char section_chords[MAX_SECTIONS][MAX_CHORDS][MAX_LENGTH], int section_durations[MAX_SECTIONS][MAX_CHORDS], int song_structure[MAX_CHORDS][2], int song_structure_length, int beats_per_measure, int BPM, int include_bass, int include_drums, int generate_melody, int melody_follow_key, int piano_volume, int drum_volume, int bass_volume, int melody_volume, const char* filename,MelodyResult* melody_result);
void get_scale_notes(const char* key, int* scale_notes, int* num_notes);

void old_print_help() {
    printf("Usage: melody_generator [options] [section] chord duration ... [section] chord duration ... [structure] section repeat ...\n");
    printf("Options:\n");
    printf("  -t BPM            Set the BPM (default: 90)\n");
    printf("  -m meter          Set the meter (3 or 4, default: 4)\n");
    printf("  -r repeats        Set the number of repeats (default: 1)\n");
    printf("  -b                Include bass\n");
    printf("  -d                Include drums\n");
    printf("  -g                Generate melody\n");
    printf("  -k                Follow key for melody\n");
    printf("  -p piano_volume   Set the piano volume (0-127, default: 100)\n");
    printf("  -v drum_volume    Set the drum volume (0-127, default: 100)\n");
    printf("  -q bass_volume    Set the bass volume (0-127, default: 100)\n");
    printf("  -w melody_volume  Set the melody volume (0-127, default: 100)\n");
    printf("  -o filename       Set the output filename (default: output.mid)\n");
    printf("  -h                Show this help message\n");
}

void print_help() {
    printf("Usage:\n");
    printf("  melody_generator [options] [section] chord duration ... [section] chord duration ... [structure] section repeat ...\n\n");

    printf("Options:\n");
    printf("  -t BPM            Set the BPM (default: 90)\n");
    printf("  -m meter          Set the meter (3 or 4, default: 4)\n");
    printf("  -r repeats        Set the number of repeats (default: 1)\n");
    printf("  -b                Include bass\n");
    printf("  -d                Include drums\n");
    printf("  -g                Generate melody\n");
    printf("  -k                Follow key for melody\n");
    printf("  -p piano_volume   Set the piano volume (0-127, default: 100)\n");
    printf("  -v drum_volume    Set the drum volume (0-127, default: 100)\n");
    printf("  -q bass_volume    Set the bass volume (0-127, default: 100)\n");
    printf("  -w melody_volume  Set the melody volume (0-127, default: 100)\n");
    printf("  -o filename       Set the output filename (default: output.mid)\n");
    printf("  -h                Show this help message\n\n");

    printf("Sections syntax:\n");
    printf("  [SECTION_NAME] chord duration ...\n");
    printf("    Example: [INTRO] C 4  G 4  Am 4  F 4\n");
    printf("  [SONG] SECTION repeat ...   (define structure by referencing sections)\n");
    printf("    Example: [SONG] INTRO 1  VERSE 2  CHORUS 2\n\n");

    printf("Chords:\n");
    printf("  Roots:   C  C#  D  D#  E  F  F#  G  G#  A  A#  B   (also flats: Db Eb Gb Ab Bb)\n");
    printf("  Suffix:  (maj)  m  7  m7  maj7  dim  aug  6  m6  sus2  sus4  add9  madd9\n");
    printf("  Notes:\n");
    printf("    (maj)  = major triad (default when suffix omitted)\n");
    printf("    sus2   = {root, 2nd, 5th}      | sus4 = {root, 4th, 5th}\n");
    printf("    add9   = major triad + 9th     | madd9 = minor triad + 9th\n");
    printf("  Examples:\n");
    printf("    C, Am, F#maj7, Bb7, Dsus4, Gadd9, Emadd9, C6, Bdim, Aaug, Gm6\n\n");

    printf("Durations:\n");
    printf("  Duration values are counts in beats based on meter (e.g., in 4/4: 4 = a whole measure).\n\n");

    printf("Full example:\n");
    printf("  ./melody_generator -t 120 -m 4 -b -d -g -k -o my_song.mid \\\n");
    printf("    [INTRO] C 4  G 4 \\\n");
    printf("    [VERSE] Am 4  Em 4 \\\n");
    printf("    [CHORUS] Cadd9 4  G 4  F 4  Dsus4 4 \\\n");
    printf("    [SONG] INTRO 1  VERSE 2  CHORUS 2\n\n");

    printf("Tips:\n");
    printf("  - Enharmonics are accepted (e.g., C# == Db).\n");
    printf("  - MIDI reference octave: C4 = 60.\n");
}

static int index_of_section_name(char sections[][MAX_LENGTH], int section_count, const char* name) {
    for (int i = 0; i < section_count; ++i) {
        const char* a = name;
        const char* b = sections[i];
        int ok = 1;
        while (*a && *b) {
            char ca = tolower((unsigned char)*a++);
            char cb = tolower((unsigned char)*b++);
            if (ca != cb) { ok = 0; break; }
        }
        if (ok && *a == '\0' && *b == '\0') return i;
    }
    return -1;
}


int main(int argc, char **argv) {
    int BPM = 90;
    int beats_per_measure = 4;
    int num_repeats = 1;
    int include_bass = 0;
    int include_drums = 0;
    int generate_melody = 0;
    int melody_follow_key = 0;
    int piano_volume = 100;
    int drum_volume = 100;
    int bass_volume = 100;
    int melody_volume = 100;
    char sections[MAX_SECTIONS][MAX_LENGTH];
    int section_repeats[MAX_SECTIONS];
    char section_chords[MAX_SECTIONS][MAX_CHORDS][MAX_LENGTH];
    int section_durations[MAX_SECTIONS][MAX_CHORDS];
    int song_structure[MAX_CHORDS][2];
    int section_index = -1;
    int song_structure_length = 0;
    char output_filename[100] = "output.mid";

    int opt;
    while ((opt = getopt(argc, argv, "t:m:r:bdgkp:v:q:w:o:h")) != -1) {
        switch (opt) {
            case 't':
                BPM = atoi(optarg);
                if (BPM < 1 || BPM > 200) {
                    fprintf(stderr, "Invalid BPM. Must be between 1 and 200. Defaulting to 90.\n");
                    BPM = 90;
                }
                break;
            case 'm':
                beats_per_measure = atoi(optarg);
                if (beats_per_measure != 3 && beats_per_measure != 4) {
                    fprintf(stderr, "Invalid meter. Defaulting to 4/4.\n");
                    beats_per_measure = 4;
                }
                break;
            case 'r':
                num_repeats = atoi(optarg);
                break;
            case 'b':
                include_bass = 1;
                break;
            case 'd':
                include_drums = 1;
                break;
            case 'g':
                generate_melody = 1;
                break;
            case 'k':
                melody_follow_key = 1;
                break;
            case 'p':
                piano_volume = atoi(optarg);
                if (piano_volume < 0 || piano_volume > 127) {
                    fprintf(stderr, "Invalid piano volume. Must be between 0 and 127. Defaulting to 100.\n");
                    piano_volume = 100;
                }
                break;
            case 'v':
                drum_volume = atoi(optarg);
                if (drum_volume < 0 || drum_volume > 127) {
                    fprintf(stderr, "Invalid drum volume. Must be between 0 and 127. Defaulting to 100.\n");
                    drum_volume = 100;
                }
                break;
            case 'q':
                bass_volume = atoi(optarg);
                if (bass_volume < 0 || bass_volume > 127) {
                    fprintf(stderr, "Invalid bass volume. Must be between 0 and 127. Defaulting to 100.\n");
                    bass_volume = 100;
                }
                break;
            case 'w':
                melody_volume = atoi(optarg);
                if (melody_volume < 0 || melody_volume > 127) {
                    fprintf(stderr, "Invalid melody volume. Must be between 0 and 127. Defaulting to 100.\n");
                    melody_volume = 100;
                }
                break;
            case 'o':
                strcpy(output_filename, optarg);
                break;
            case 'h':
                print_help();
                exit(EXIT_SUCCESS);
            default:
                print_help();
                exit(EXIT_FAILURE);
        }
    }

    // Eval notes duration
    SEMIMINIMA = 60.0 / BPM;           // Semiminima (1/4)
    MINIMA = 2 * SEMIMINIMA;           // Minima (1/2)
    CROMA = SEMIMINIMA / 2;            // Croma (1/8)
    SEMICROMA = SEMIMINIMA / 4;        // Semicroma (1/16)
    SEMIBREVE = 4 * SEMIMINIMA;        // Semibreve (intera)
  

// ------------------------------------------------------------
// NUOVO: parsing dinamico con SectionRegistry
// ------------------------------------------------------------
    SectionRegistry reg;
    sections_init(&reg);

// Verifica che ci siano token dopo le opzioni
    if (optind >= argc) {
      fprintf(stderr, "No sections/structure provided. See -h.\n");
      sections_free(&reg);
      exit(EXIT_FAILURE);
    }
    
    // Passa gli argomenti rimanenti al parser dinamico
    int leftover_argc = argc - optind;
    char** leftover_argv = &argv[optind];
    
    int prc = parse_sections_and_structure(&reg, leftover_argc, leftover_argv);
    if (prc < 0) {
      sections_free(&reg);
      exit(EXIT_FAILURE);
    }
    
    // Pulisci le strutture esistenti (già dichiarate prima)
    for (int s = 0; s < MAX_SECTIONS; ++s) {
      sections[s][0] = '\0';
      section_repeats[s] = 1; // default
      for (int j = 0; j < MAX_CHORDS; ++j) {
        section_chords[s][j][0] = '\0';
        section_durations[s][j] = 0;
      }
    }
    song_structure_length = 0;
    
    // 1) Travaso sezioni dal registry → tue strutture
    int section_count = 0;
    for (size_t i = 0; i < reg.count; ++i) {
      if (section_count >= MAX_SECTIONS) {
        fprintf(stderr, "Exceeded MAX_SECTIONS (%d)\n", MAX_SECTIONS);
        sections_free(&reg);
        exit(EXIT_FAILURE);
      }
      // nome sezione
      snprintf(sections[section_count], MAX_LENGTH, "%s", reg.sections[i].name);
      section_repeats[section_count] = 1; // ripetizioni gestite in [SONG]

      // accordi/durate
      int chord_count = 0;
      for (size_t k = 0; k < reg.sections[i].count; ++k) {
        if (chord_count >= MAX_CHORDS) {
	  fprintf(stderr, "Exceeded MAX_CHORDS (%d) in section '%s'\n", MAX_CHORDS, reg.sections[i].name);
	  sections_free(&reg);
	  exit(EXIT_FAILURE);
        }
        snprintf(section_chords[section_count][chord_count], MAX_LENGTH, "%s", reg.sections[i].items[k].chord);
        section_durations[section_count][chord_count] = reg.sections[i].items[k].duration;
        chord_count++;
      }
      section_count++;
    }
    
    // 2) Travaso struttura [SONG]
    for (size_t i = 0; i < reg.song_count; ++i) {
      int idx = index_of_section_name(sections, section_count, reg.song[i].name);
      if (idx < 0) {
        fprintf(stderr, "Internal error: section '%s' referenced in [SONG] not found after conversion.\n", reg.song[i].name);
        sections_free(&reg);
        exit(EXIT_FAILURE);
      }
      if (song_structure_length >= MAX_CHORDS) {
        fprintf(stderr, "Exceeded MAX_CHORDS in song structure array (%d)\n", MAX_CHORDS);
        sections_free(&reg);
        exit(EXIT_FAILURE);
      }
      song_structure[song_structure_length][0] = idx;                 // sezione come indice
      song_structure[song_structure_length][1] = reg.song[i].repeats; // ripetizioni
      song_structure_length++;
    }
    
if (section_count == 0 || song_structure_length == 0) {
  fprintf(stderr, "No valid sections or [SONG] structure provided.\n");
  sections_free(&reg);
  exit(EXIT_FAILURE);
 }
 
// (debug) stampa come prima
 printf("Sections:\n");
 for (int i = 0; i < section_count; i++) {
   printf("Section %d (%s), repeats %d\n", i, sections[i], section_repeats[i]);
   for (int j = 0; j < MAX_CHORDS && section_chords[i][j][0] != 0; j++) {
     printf("  Chord %d: %s, duration: %d\n", j, section_chords[i][j], section_durations[i][j]);
    }
 }
 
 printf("Song structure:\n");
 for (int i = 0; i < song_structure_length; i++) {
   printf("  Section %d (%s), repeats %d\n",
	  song_structure[i][0],
	  sections[song_structure[i][0]],
	  song_structure[i][1]);
 }
 
 // Fine parsing dinamico: userai compose_melody() e write_midi_with_melody() come prima
 
 
 
    printf("Sections:\n");
    for (int i = 0; i <= section_index; i++) {
      printf("Section %d (%s), repeats %d\n", i, sections[i], section_repeats[i]);
      for (int j = 0; j < MAX_CHORDS && section_chords[i][j][0] != 0; j++) {
	printf("  Chord %d: %s, duration: %d\n", j, section_chords[i][j], section_durations[i][j]);
      }
    }
    
    printf("Song structure:\n");
    for (int i = 0; i < song_structure_length; i++) {
      printf("  Section %d, repeats %d\n", song_structure[i][0], song_structure[i][1]);
    }
    
    MelodyResult melody_result;
    compose_melody(sections, section_repeats, section_chords, section_durations, song_structure, song_structure_length, beats_per_measure, 0, 0,BPM, &melody_result);
    
    
    
    
    // Scrivi il file MIDI con la melodia
    write_midi_with_melody(sections, section_repeats, section_chords, section_durations, song_structure, song_structure_length, beats_per_measure,BPM, include_bass, include_drums, generate_melody, melody_follow_key, piano_volume, drum_volume, bass_volume, melody_volume, output_filename, &melody_result);
    
    
    printf("File MIDI generato: %s\n", output_filename);
    return 0;
}

// GM Drum Map (canale 10 -> status 0x99 on, 0x89 off)
// Kick 36, Snare 38, Closed HH 42, Open HH 46, Crash1 49, High Tom 50, Mid Tom 47, Low Tom 45, Floor Tom 43

static void add_drum_fill(smf_track_t* drum_track, double start, double beat_duration, int drum_volume) {
    // Fill semplice su tom in semicrome (4 colpi sul beat finale)
    const int pattern[] = {50, 47, 45, 43}; // HighTom, MidTom, LowTom, FloorTom
    const int n = (int)(sizeof(pattern)/sizeof(pattern[0]));
    double step = beat_duration / n;
    for (int i = 0; i < n; ++i) {
        double t_on  = start + i * step;
        double t_off = t_on + step * 0.85; // un po' più corto per essere "tight"
        smf_event_t* on  = smf_event_new_from_bytes(0x99, pattern[i], drum_volume);
        smf_event_t* off = smf_event_new_from_bytes(0x89, pattern[i], drum_volume);
        smf_track_add_event_seconds(drum_track, on,  t_on);
        smf_track_add_event_seconds(drum_track, off, t_off);
    }

    // Piccolo flam di rullante in chiusura del beat
    double flam_t = start + beat_duration * 0.90;
    smf_event_t* sn1_on  = smf_event_new_from_bytes(0x99, 38, drum_volume);
    smf_event_t* sn1_off = smf_event_new_from_bytes(0x89, 38, drum_volume);
    smf_event_t* sn2_on  = smf_event_new_from_bytes(0x99, 38, drum_volume);
    smf_event_t* sn2_off = smf_event_new_from_bytes(0x89, 38, drum_volume);
    smf_track_add_event_seconds(drum_track, sn1_on,  flam_t);
    smf_track_add_event_seconds(drum_track, sn1_off, flam_t + (beat_duration * 0.10));
    smf_track_add_event_seconds(drum_track, sn2_on,  flam_t + (beat_duration * 0.12));
    smf_track_add_event_seconds(drum_track, sn2_off, flam_t + (beat_duration * 0.22));
}

static void add_crash_downbeat(smf_track_t* drum_track, double start, double hold, int drum_volume) {
    // Crash sul downbeat della sezione successiva
    smf_event_t* cr_on  = smf_event_new_from_bytes(0x99, 49, drum_volume);
    smf_event_t* cr_off = smf_event_new_from_bytes(0x89, 49, drum_volume);
    smf_track_add_event_seconds(drum_track, cr_on,  start);
    smf_track_add_event_seconds(drum_track, cr_off, start + hold);
}

static void add_conductor_track(smf_t* smf, int bpm, int meter_num /* 3 o 4 */) {
    // Imposta PPQN (ticks per quarter). 480 è uno standard comodo.
    // Se hai già un set_ppqn altrove, lascia quello.
    smf_set_ppqn(smf, 480);

    smf_track_t* tempo_track = smf_track_new();
    smf_add_track(smf, tempo_track);

    // --- Meta: Set Tempo (FF 51 03 tttttt)
    int us_per_quarter = (int)(60000000.0 / (double)bpm);
    unsigned char tempo_msg[6];
    tempo_msg[0] = 0xFF;
    tempo_msg[1] = 0x51;
    tempo_msg[2] = 0x03;
    tempo_msg[3] = (us_per_quarter >> 16) & 0xFF;
    tempo_msg[4] = (us_per_quarter >> 8) & 0xFF;
    tempo_msg[5] = us_per_quarter & 0xFF;

    smf_event_t* ev_tempo = smf_event_new_from_pointer(tempo_msg, 6);
    smf_track_add_event_seconds(tempo_track, ev_tempo, 0.0);

    // --- Meta: Time Signature (FF 58 04 nn dd cc bb)
    // -m 4 => 4/4, -m 3 => 3/4 (quindi denominatore=4, dd=log2(4)=2)
    int numerator = (meter_num == 3) ? 3 : 4;
    int denominator = 4;
    int dd = 2;             // log2(4)
    int clocks_per_click = 24;
    int notes_per_quarter_32nds = 8;

    unsigned char ts_msg[7];
    ts_msg[0] = 0xFF;
    ts_msg[1] = 0x58;
    ts_msg[2] = 0x04;
    ts_msg[3] = (unsigned char)numerator;
    ts_msg[4] = (unsigned char)dd;
    ts_msg[5] = (unsigned char)clocks_per_click;
    ts_msg[6] = (unsigned char)notes_per_quarter_32nds;

    smf_event_t* ev_ts = smf_event_new_from_pointer(ts_msg, 7);
    smf_track_add_event_seconds(tempo_track, ev_ts, 0.0);
}


void write_midi_with_melody(
    char sections[MAX_SECTIONS][MAX_LENGTH], 
    int section_repeats[MAX_SECTIONS], 
    char section_chords[MAX_SECTIONS][MAX_CHORDS][MAX_LENGTH], 
    int section_durations[MAX_SECTIONS][MAX_CHORDS], 
    int song_structure[MAX_CHORDS][2], 
    int song_structure_length, 
    int beats_per_measure,
    int BPM,
    int include_bass, 
    int include_drums, 
    int generate_melody, 
    int melody_follow_key, 
    int piano_volume, 
    int drum_volume, 
    int bass_volume, 
    int melody_volume, 
    const char* filename,
    MelodyResult* melody_result
) {
    smf_t* smf = smf_new();

    //Aggiungo la traccia con le meta info
    add_conductor_track(smf, BPM, beats_per_measure);

    
    smf_track_t* piano_track = smf_track_new();
    smf_add_track(smf, piano_track);

    smf_track_t* melody_track = smf_track_new();
    smf_add_track(smf, melody_track);

    smf_track_t* bass_track = smf_track_new();
    smf_add_track(smf, bass_track);

    smf_track_t* drum_track = smf_track_new();
    smf_add_track(smf, drum_track);

    double time_seconds = 0.0;

    for (int r = 0; r < song_structure_length; r++) {
        int section_index = song_structure[r][0];
        int repeat = song_structure[r][1];

        /* Calcola la lunghezza (in numero di accordi) della sezione corrente */
        int section_len = 0;
        while (section_len < MAX_CHORDS && section_chords[section_index][section_len][0] != 0) {
            section_len++;
        }

        for (int j = 0; j < repeat; j++) {
            for (int i = 0; i < section_len; i++) {
                int notes[10];
                int num_notes = chord_to_notes(section_chords[section_index][i], notes);

                /* Timing del singolo accordo */
                double chord_start = time_seconds;
                double chord_dur_sec = section_durations[section_index][i] * SEMIMINIMA;
                double chord_end   = chord_start + chord_dur_sec;

                /* --- Piano (accordo tenuto per tutta la durata) --- */
                for (int k = 0; k < num_notes; k++) {
                    smf_event_t* note_on  = smf_event_new_from_bytes(0x90 | 0x00, notes[k], piano_volume);
                    smf_track_add_event_seconds(piano_track, note_on, chord_start);
                    smf_event_t* note_off = smf_event_new_from_bytes(0x80 | 0x00, notes[k], piano_volume);
                    smf_track_add_event_seconds(piano_track, note_off, chord_end);
                }

                /* --- Melodia generata (se richiesta) ---
                   NOTA: pianifichiamo la melodia RELATIVA a chord_start,
                   senza modificare time_seconds dentro il loop della melodia,
                   così l’allineamento resta sempre coerente. */
                if (generate_melody) {
                    int melody_length = melody_result->melody_lengths[section_index];
                    double t = chord_start;
                    for (int m = 0; m < melody_length; m++) {
                        double mdur = melody_result->melody_durations[section_index][m];
                        int mnote   = melody_result->melody_notes[section_index][m];

                        smf_event_t* note_on  = smf_event_new_from_bytes(0x90 | 0x01, mnote, melody_volume);
                        smf_track_add_event_seconds(melody_track, note_on, t);

                        smf_event_t* note_off = smf_event_new_from_bytes(0x80 | 0x01, mnote, melody_volume);
                        smf_track_add_event_seconds(melody_track, note_off, t + mdur);

                        t += mdur;
                        if (t > chord_end) break; /* evita di oltrepassare la durata dell'accordo */
                    }
                }

                /* --- Basso (root, ottava sotto, per tutta la durata dell’accordo) --- */
                if (include_bass && num_notes > 0) {
                    int bass_note = notes[0] - 12;
                    smf_event_t* bass_on  = smf_event_new_from_bytes(0x90 | 0x02, bass_note, bass_volume);
                    smf_track_add_event_seconds(bass_track, bass_on, chord_start);
                    smf_event_t* bass_off = smf_event_new_from_bytes(0x80 | 0x02, bass_note, bass_volume);
                    smf_track_add_event_seconds(bass_track, bass_off, chord_end);
                }

                /* --- Batteria: groove regolare sulla durata dell’accordo --- */
                if (include_drums) {
                    /* Kick all'inizio, Snare a metà, Hi-hat su ogni beat */
                    int kick_drum_note = 36;
                    int snare_drum_note = 38;
                    int hi_hat_note = 42;

                    /* Kick */
                    smf_event_t* kick_on  = smf_event_new_from_bytes(0x99, kick_drum_note, drum_volume);
                    smf_track_add_event_seconds(drum_track, kick_on, chord_start);
                    smf_event_t* kick_off = smf_event_new_from_bytes(0x89, kick_drum_note, drum_volume);
                    smf_track_add_event_seconds(drum_track, kick_off, chord_start + SEMIMINIMA);

                    /* Snare */
                    double snare_t = chord_start + (chord_dur_sec * 0.5);
                    smf_event_t* sn_on  = smf_event_new_from_bytes(0x99, snare_drum_note, drum_volume);
                    smf_track_add_event_seconds(drum_track, sn_on, snare_t);
                    smf_event_t* sn_off = smf_event_new_from_bytes(0x89, snare_drum_note, drum_volume);
                    smf_track_add_event_seconds(drum_track, sn_off, snare_t + SEMIMINIMA);

                    /* Hi-hat su ogni beat intero */
                    int total_beats = (int)(chord_dur_sec / SEMIMINIMA + 0.5);
                    for (int beat = 0; beat < total_beats; beat++) {
                        double t_on = chord_start + beat * SEMIMINIMA;
                        double t_off = t_on + CROMA;
                        if (t_on >= chord_end) break;
                        if (t_off > chord_end) t_off = chord_end;
                        smf_event_t* hh_on  = smf_event_new_from_bytes(0x99, hi_hat_note, drum_volume);
                        smf_track_add_event_seconds(drum_track, hh_on, t_on);
                        smf_event_t* hh_off = smf_event_new_from_bytes(0x89, hi_hat_note, drum_volume);
                        smf_track_add_event_seconds(drum_track, hh_off, t_off);
                    }
                }

                /* --- FILL DI BATTERIA allo stacco di sezione (ultimo accordo della sezione) --- */
                if (include_drums && i == section_len - 1) {
                    /* Durata del fill: l’ultimo beat dell’accordo (o meno, se l’accordo è più corto) */
                    double fill_beat = SEMIMINIMA;
                    if (fill_beat > chord_dur_sec) fill_beat = chord_dur_sec * 0.5; /* safety */

                    double fill_start = chord_end - fill_beat;

                    /* Pattern semplice sui tom in semicrome dentro il beat finale */
                    const int toms[] = {50, 47, 45, 43}; /* HighTom, MidTom, LowTom, FloorTom */
                    int n = (int)(sizeof(toms) / sizeof(toms[0]));
                    double step = fill_beat / n;
                    for (int ti = 0; ti < n; ++ti) {
                        double t_on  = fill_start + ti * step;
                        double t_off = t_on + step * 0.85;
                        smf_event_t* on  = smf_event_new_from_bytes(0x99, toms[ti], drum_volume);
                        smf_track_add_event_seconds(drum_track, on,  t_on);
                        smf_event_t* off = smf_event_new_from_bytes(0x89, toms[ti], drum_volume);
                        smf_track_add_event_seconds(drum_track, off, t_off);
                    }
                    /* Piccolo flam di rullante a chiusura */
                    double flam_t = chord_end - (fill_beat * 0.10);
                    smf_event_t* sn1_on  = smf_event_new_from_bytes(0x99, 38, drum_volume);
                    smf_event_t* sn1_off = smf_event_new_from_bytes(0x89, 38, drum_volume);
                    smf_event_t* sn2_on  = smf_event_new_from_bytes(0x99, 38, drum_volume);
                    smf_event_t* sn2_off = smf_event_new_from_bytes(0x89, 38, drum_volume);
                    smf_track_add_event_seconds(drum_track, sn1_on,  flam_t);
                    smf_track_add_event_seconds(drum_track, sn1_off, flam_t + (fill_beat * 0.10));
                    smf_track_add_event_seconds(drum_track, sn2_on,  flam_t + (fill_beat * 0.12));
                    smf_track_add_event_seconds(drum_track, sn2_off, flam_t + (fill_beat * 0.22));

                    /* Crash sul downbeat della sezione successiva */
                    smf_event_t* cr_on  = smf_event_new_from_bytes(0x99, 49, drum_volume);
                    smf_event_t* cr_off = smf_event_new_from_bytes(0x89, 49, drum_volume);
                    smf_track_add_event_seconds(drum_track, cr_on,  chord_end);
                    smf_track_add_event_seconds(drum_track, cr_off, chord_end + CROMA);
                }

                /* Avanza al prossimo accordo */
                time_seconds = chord_end;

                if (time_seconds < 0) {
                    fprintf(stderr, "Invalid time_seconds calculated: %f\n", time_seconds);
                    exit(EXIT_FAILURE);
                }
            }
        }
    }

    int result = smf_save(smf, filename);
    if (result != 0) {
        fprintf(stderr, "Errore nel salvataggio del file MIDI: %s\n", filename);
    }

    smf_delete(smf);
}



