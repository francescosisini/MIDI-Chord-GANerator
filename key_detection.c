#include <string.h>
#include "key_detection.h"

/**
 * determine_key:
 * Determina la tonalità di un insieme di accordi dato.
 * 
 * Approccio utilizzato:
 * 1. Creazione di Contatori per le Tonalità:
 *    - Creiamo due array di contatori: uno per le tonalità maggiori e uno per le tonalità minori.
 * 
 * 2. Incremento dei Contatori:
 *    - Per ogni accordo nella progressione, verifichiamo se l'accordo contiene una delle tonalità maggiori o minori.
 *    - Se trova una corrispondenza, incrementiamo il contatore della tonalità corrispondente.
 * 
 * 3. Determinazione della Tonalità Prevalente:
 *    - Troviamo la tonalità con il contatore più alto sia per le tonalità maggiori che per quelle minori.
 *    - Confrontiamo i contatori delle tonalità maggiori e minori per determinare quale tipo di tonalità (maggiore o minore) è più prevalente.
 * 
 * 4. Restituiamo la tonalità prevalente come la tonalità principale del pezzo.
 * 
 * Limitazioni:
 * - Questo approccio è semplice e potrebbe non catturare tutte le complessità delle progressioni armoniche.
 * - Non tiene conto del contesto armonico complessivo e della funzione di ciascun accordo nella progressione.
 * 
 * @param chords: Un array di stringhe che rappresentano gli accordi.
 * @param num_chords: Il numero di accordi.
 * @return: La tonalità determinata come stringa.
 */
char* determine_key(char chords[MAX_CHORDS][MAX_LENGTH], int num_chords) {
    // Lista delle tonalità maggiori e minori
    const char* major_keys[] = {"C", "G", "D", "A", "E", "B", "F#", "C#", "F", "Bb", "Eb", "Ab", "Db", "Gb", "Cb"};
    const char* minor_keys[] = {"Am", "Em", "Bm", "F#m", "C#m", "G#m", "D#m", "A#m", "Dm", "Gm", "Cm", "Fm", "Bbm", "Ebm", "Abm"};
    int major_key_count[15] = {0}; // Contatore per ogni tonalità maggiore
    int minor_key_count[15] = {0}; // Contatore per ogni tonalità minore

    for (int i = 0; i < num_chords; i++) {
        // Incrementa il contatore per la tonalità corrispondente all'accordo
        for (int j = 0; j < 15; j++) {
            if (strstr(chords[i], major_keys[j]) != NULL) {
                major_key_count[j]++;
            }
            if (strstr(chords[i], minor_keys[j]) != NULL) {
                minor_key_count[j]++;
            }
        }
    }

    // Trova la tonalità con il maggior numero di occorrenze
    int max_major_count = 0;
    int max_minor_count = 0;
    int major_key_index = 0;
    int minor_key_index = 0;
    for (int i = 0; i < 15; i++) {
        if (major_key_count[i] > max_major_count) {
            max_major_count = major_key_count[i];
            major_key_index = i;
        }
        if (minor_key_count[i] > max_minor_count) {
            max_minor_count = minor_key_count[i];
            minor_key_index = i;
        }
    }

    // Determina se la tonalità è maggiore o minore
    if (max_major_count >= max_minor_count) {
        return (char*)major_keys[major_key_index];
    } else {
        return (char*)minor_keys[minor_key_index];
    }
}

/**
 * determine_keys_for_chords:
 * Determina la tonalità per ogni accordo nella progressione degli accordi.
 * 
 * Approccio utilizzato:
 * 1. Determinazione della tonalità iniziale utilizzando la funzione determine_key.
 * 2. Aggiornamento della tonalità corrente basata su possibili risoluzioni armoniche.
 *    - Se l'accordo corrente è un V grado, la tonalità probabilmente risolve nella tonalità tonica corrispondente.
 * 
 * Limitazioni:
 * - Questo approccio è basato su regole semplici e potrebbe non catturare tutte le complessità delle progressioni armoniche.
 * 
 * @param chords: Un array di stringhe che rappresentano gli accordi.
 * @param num_chords: Il numero di accordi.
 * @param keys: Un array di stringhe in cui memorizzare le tonalità determinate per ciascun accordo.
 */
void determine_keys_for_chords(char chords[MAX_CHORDS][MAX_LENGTH], int num_chords, char keys[MAX_CHORDS][MAX_LENGTH]) {
    char* current_key = determine_key(chords, num_chords);
    for (int i = 0; i < num_chords; i++) {
        // Se l'accordo è un V grado, probabilmente la tonalità è quella di risoluzione
        if (strstr(chords[i], "G") != NULL && strstr(current_key, "C") != NULL) {
            current_key = "C";
        } else if (strstr(chords[i], "D") != NULL && strstr(current_key, "G") != NULL) {
            current_key = "G";
        } else if (strstr(chords[i], "A") != NULL && strstr(current_key, "D") != NULL) {
            current_key = "D";
        } else if (strstr(chords[i], "E") != NULL && strstr(current_key, "A") != NULL) {
            current_key = "A";
        } else if (strstr(chords[i], "B") != NULL && strstr(current_key, "E") != NULL) {
            current_key = "E";
        } else if (strstr(chords[i], "F#") != NULL && strstr(current_key, "B") != NULL) {
            current_key = "B";
        } else if (strstr(chords[i], "C#") != NULL && strstr(current_key, "F#") != NULL) {
            current_key = "F#";
        } else if (strstr(chords[i], "F") != NULL && strstr(current_key, "Bb") != NULL) {
            current_key = "Bb";
        } else if (strstr(chords[i], "Bb") != NULL && strstr(current_key, "Eb") != NULL) {
            current_key = "Eb";
        } else if (strstr(chords[i], "Eb") != NULL && strstr(current_key, "Ab") != NULL) {
            current_key = "Ab";
        } else if (strstr(chords[i], "Ab") != NULL && strstr(current_key, "Db") != NULL) {
            current_key = "Db";
        } else if (strstr(chords[i], "Db") != NULL && strstr(current_key, "Gb") != NULL) {
            current_key = "Gb";
        } else if (strstr(chords[i], "Gb") != NULL && strstr(current_key, "Cb") != NULL) {
            current_key = "Cb";
        } else if (strstr(chords[i], "Cb") != NULL && strstr(current_key, "F") != NULL) {
            current_key = "F";
        } else {
            current_key = determine_key(chords, num_chords);
        }
        strcpy(keys[i], current_key);
    }
}
