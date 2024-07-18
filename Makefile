# Nome del file eseguibile
TARGET = goro

# File sorgente
SRC = goro.c

# Percorsi di inclusione per le librerie
INCLUDES = -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include

# Librerie da collegare
LIBS = -lsmf -lglib-2.0

# Compilatore
CC = gcc

# Opzioni del compilatore
CFLAGS = $(INCLUDES) $(LIBS)

# Regola predefinita
all: $(TARGET)

# Regola per creare l'eseguibile
$(TARGET): $(SRC)
	$(CC) -o $(TARGET) $(SRC) $(CFLAGS)

# Regola per pulire i file generati
clean:
	rm -f $(TARGET)

# Regola per eseguire il programma
run: $(TARGET)
	./$(TARGET)
