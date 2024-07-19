
# MIDI Chord GANerator

MIDI Chord GANerator è un programma che genera sequenze MIDI basate su progressioni di accordi fornite dall'utente. Il programma può generare melodie casuali che seguono o meno la tonalità determinata dalla progressione degli accordi, e aggiunge una linea di basso e una batteria che tengono conto del metro scelto (3/4 o 4/4).

## Funzionalità

- Inserimento di progressioni di accordi e durate.
- Ripetizione della progressione degli accordi.
- Generazione di melodie casuali.
- Melodie che seguono la tonalità determinata dalla progressione degli accordi.
- Aggiunta di linea di basso basata sulla nota fondamentale degli accordi.
- Aggiunta di batteria con cassa, rullante e hi-hat.
- Supporto per i metri 3/4 e 4/4.
- Salvataggio del file MIDI generato.

## Dipendenze

- [libsmf](https://sourceforge.net/projects/libsmf/)
- [glib-2.0](https://developer.gnome.org/glib/)

## Installazione

1. Clona il repository:
   ```sh
   git clone https://github.com/tuo-username/midi-chord-ganerator.git
   cd midi-chord-ganerator
   ```

2. Esegui lo script di setup per installare le dipendenze:
   ```sh
   ./setup.sh
   ```

3. Compila il progetto:
   ```sh
   make
   ```

## Utilizzo

Esegui il programma con:
```sh
make run
```

Durante l'esecuzione, il programma ti chiederà di inserire informazioni sulla progressione degli accordi, le durate degli accordi, il numero di ripetizioni, se generare una melodia e se la melodia deve seguire la tonalità.

Esempio di esecuzione:

```plaintext
Scegli il metro (3 per 3/4 o 4 per 4/4): 4
Inserisci il numero di accordi nel giro armonico: 4
Inserisci gli accordi (es. C, G, Am, F) e la loro durata in quarti:
Accordo 1: C
Durata in quarti (1, 2, ... 4): 4
Accordo 2: Am
Durata in quarti (1, 2, ... 4): 4
Accordo 3: G
Durata in quarti (1, 2, ... 4): 4
Accordo 4: F
Durata in quarti (1, 2, ... 4): 4
Quante volte ripetere il giro armonico? 2
Vuoi generare una melodia casuale? (1 per sì, 0 per no): 1
Vuoi che la melodia segua la tonalità? (1 per sì, 0 per no): 1
File MIDI generato: output.mid
```

## Struttura del Progetto

- `main.c`: Contiene il flusso principale del programma.
- `chords.c` e `chords.h`: Funzioni per la gestione degli accordi.
- `melody.c` e `melody.h`: Funzioni per la generazione delle melodie.
- `key_detection.c` e `key_detection.h`: Funzioni per la determinazione della tonalità.
- `setup.sh`: Script per l'installazione delle dipendenze.
- `Makefile`: File per la compilazione del progetto.

## Contribuire

Se desideri contribuire a questo progetto, sentiti libero di fare una fork del repository e inviare una pull request con le tue modifiche.

## Licenza

Questo progetto è concesso in licenza sotto la licenza MIT. Vedi il file [LICENSE](LICENSE) per ulteriori dettagli.

## Autori

- [Francesco Sisini](https://github.com/francescosisini)

## Immagini del Progetto

![MIDI Chord GANerator](path/to/your/project/image.png)
