#include "melody.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Mappa per le scale maggiori diatoniche
const int major_scale_intervals[] = {2, 2, 1, 2, 2, 2, 1};

void generate_random_notes(smf_track_t* track, int num_notes, double start_time, double duration, int volume) {
    srand(time(NULL));
    double note_duration = duration / num_notes;
    for (int i = 0; i < num_notes; i++) {
        int note = rand() % 128; // Note casuale tra 0 e 127
        smf_event_t* note_on = smf_event_new_from_bytes(0x90, note, volume);
        smf_track_add_event_seconds(track, note_on, start_time + i * note_duration);
        smf_event_t* note_off = smf_event_new_from_bytes(0x80, note, volume);
        smf_track_add_event_seconds(track, note_off, start_time + (i + 1) * note_duration);
    }
}

void generate_random_notes_with_scale(smf_track_t* track, int num_notes, double start_time, double duration, int* scale_notes, int num_scale_notes, int volume) {
    srand(time(NULL));
    double note_duration = duration / num_notes;
    for (int i = 0; i < num_notes; i++) {
        int note = scale_notes[rand() % num_scale_notes]; // Note casuale dalla scala
        smf_event_t* note_on = smf_event_new_from_bytes(0x90, note, volume);
        smf_track_add_event_seconds(track, note_on, start_time + i * note_duration);
        smf_event_t* note_off = smf_event_new_from_bytes(0x80, note, volume);
        smf_track_add_event_seconds(track, note_off, start_time + (i + 1) * note_duration);
    }
}


void generate_melody_with_rules(smf_track_t* track, int num_notes, double start_time, double duration, int* scale_notes, int num_scale_notes, int volume) {
    srand(time(NULL)); // Inizializza il generatore di numeri casuali con il tempo attuale
    double note_duration = duration / num_notes; // Calcola la durata di ciascuna nota dividendo la durata totale per il numero di note
    int prev_note_index = rand() % num_scale_notes; // Seleziona casualmente l'indice di una nota nella scala come punto di partenza

    for (int i = 0; i < num_notes; i++) {
        int note_index;
        if (rand() % 2 == 0) {
            // Passaggio scalare: muoversi alla nota adiacente nella scala
            note_index = prev_note_index + (rand() % 2 ? 1 : -1); // Decide se andare su o giù nella scala
            if (note_index < 0) note_index = 0; // Se l'indice è inferiore a 0, lo imposta a 0 (nota più bassa della scala)
            if (note_index >= num_scale_notes) note_index = num_scale_notes - 1; // Se l'indice supera la scala, lo imposta all'ultima nota della scala
        } else {
            // Salto consonante: salti di terza o quinta
            int jump = (rand() % 2 + 1) * 2; // Determina l'intervallo del salto (di terza o quinta)
            note_index = prev_note_index + (rand() % 2 ? jump : -jump); // Decide se saltare su o giù
            if (note_index < 0) note_index = 0; // Controlla che l'indice non sia inferiore a 0
            if (note_index >= num_scale_notes) note_index = num_scale_notes - 1; // Controlla che l'indice non superi la scala
        }

        int note = scale_notes[note_index]; // Ottiene la nota corrispondente dall'indice della scala
        smf_event_t* note_on = smf_event_new_from_bytes(0x90, note, volume); // Crea un evento "Note On" per iniziare a suonare la nota
        smf_track_add_event_seconds(track, note_on, start_time + i * note_duration); // Aggiunge l'evento "Note On" alla traccia MIDI al tempo corretto
        smf_event_t* note_off = smf_event_new_from_bytes(0x80, note, volume); // Crea un evento "Note Off" per terminare la nota
        smf_track_add_event_seconds(track, note_off, start_time + (i + 1) * note_duration); // Aggiunge l'evento "Note Off" alla traccia MIDI alla fine della durata della nota

        prev_note_index = note_index; // Aggiorna l'indice della nota precedente per il prossimo ciclo
    }
}

void generate_varied_duration_melody(smf_track_t* track, int num_notes, double start_time, double duration, int* scale_notes, int num_scale_notes, int volume) {
    srand(time(NULL)); // Inizializza il generatore di numeri casuali con il tempo attuale

    double remaining_duration = duration;

    // Seleziona casualmente l'indice di una delle note della triade (tonica, terza o quinta)
    int triad_indices[] = {0, 2, 4}; // Indici della tonica, terza e quinta nella scala maggiore
    int prev_note_index = triad_indices[rand() % 3];

    for (int i = 0; i < num_notes; i++) {
        int note_index;
        if (rand() % 2 == 0) {
            // Passaggio scalare: muoversi alla nota adiacente nella scala
            note_index = prev_note_index + (rand() % 2 ? 1 : -1); // Decide se andare su o giù nella scala
            if (note_index < 0) note_index = 0; // Se l'indice è inferiore a 0, lo imposta a 0 (nota più bassa della scala)
            if (note_index >= num_scale_notes) note_index = num_scale_notes - 1; // Se l'indice supera la scala, lo imposta all'ultima nota della scala
        } else {
            // Salto consonante: salti di terza o quinta
            int jump = (rand() % 2 + 1) * 2; // Determina l'intervallo del salto (di terza o quinta)
            note_index = prev_note_index + (rand() % 2 ? jump : -jump); // Decide se saltare su o giù
            if (note_index < 0) note_index = 0; // Controlla che l'indice non sia inferiore a 0
            if (note_index >= num_scale_notes) note_index = num_scale_notes - 1; // Controlla che l'indice non superi la scala
        }

        int note = scale_notes[note_index]; // Ottiene la nota corrispondente dall'indice della scala

        // Genera una durata variabile per la nota corrente
        double note_duration = remaining_duration / (num_notes - i);
        if (rand() % 2 == 0) {
            note_duration *= 0.5;
        } else {
            note_duration *= 1.5;
        }

        // Assicura che la durata della nota non superi la durata rimanente
        if (note_duration > remaining_duration) {
            note_duration = remaining_duration;
        }

        smf_event_t* note_on = smf_event_new_from_bytes(0x90, note, volume); // Crea un evento "Note On" per iniziare a suonare la nota
        smf_track_add_event_seconds(track, note_on, start_time); // Aggiunge l'evento "Note On" alla traccia MIDI al tempo corretto
        smf_event_t* note_off = smf_event_new_from_bytes(0x80, note, volume); // Crea un evento "Note Off" per terminare la nota
        smf_track_add_event_seconds(track, note_off, start_time + note_duration); // Aggiunge l'evento "Note Off" alla traccia MIDI alla fine della durata della nota

        // Aggiorna il tempo di inizio per la prossima nota e la durata rimanente
        start_time += note_duration;
        remaining_duration -= note_duration;
        prev_note_index = note_index; // Aggiorna l'indice della nota precedente per il prossimo ciclo

        // Se la durata rimanente è troppo breve, esci dal ciclo
        if (remaining_duration <= 0) {
            break;
        }
    }
}



void get_scale_notes(const char* key, int* scale_notes, int* num_notes) {
    int base_note;
    *num_notes = 7; // Una scala diatonica maggiore ha 7 note

    // Determina la nota base dalla tonalità
    if (strcmp(key, "C") == 0) {
        base_note = 60; // C
    } else if (strcmp(key, "C#") == 0 || strcmp(key, "Db") == 0) {
        base_note = 61; // C# / Db
    } else if (strcmp(key, "D") == 0) {
        base_note = 62; // D
    } else if (strcmp(key, "D#") == 0 || strcmp(key, "Eb") == 0) {
        base_note = 63; // D# / Eb
    } else if (strcmp(key, "E") == 0) {
        base_note = 64; // E
    } else if (strcmp(key, "F") == 0) {
        base_note = 65; // F
    } else if (strcmp(key, "F#") == 0 || strcmp(key, "Gb") == 0) {
        base_note = 66; // F# / Gb
    } else if (strcmp(key, "G") == 0) {
        base_note = 67; // G
    } else if (strcmp(key, "G#") == 0 || strcmp(key, "Ab") == 0) {
        base_note = 68; // G# / Ab
    } else if (strcmp(key, "A") == 0) {
        base_note = 69; // A
    } else if (strcmp(key, "A#") == 0 || strcmp(key, "Bb") == 0) {
        base_note = 70; // A# / Bb
    } else if (strcmp(key, "B") == 0) {
        base_note = 71; // B
    } else {
        base_note = 60; // Default to C
    }

    // Costruisci la scala maggiore diatonica basata sulla nota base
    scale_notes[0] = base_note;
    for (int i = 1; i < *num_notes; i++) {
        scale_notes[i] = scale_notes[i - 1] + major_scale_intervals[i - 1];
    }
}
