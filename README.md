# Multi-Chat-LSO

## How to build/run?
```bash
cd Server-C/
make
bin/server -d

#Optional
make clean #(for clear executable)
```

### Database
Per far compilare il database c'è bisogno di installare le seguenti librerie:
```bash
# Debian/Ubuntu
sudo apt install libpq-dev

# Arch
sudo pacman -S postgresql-libs

# Other distro? Google it
```


## Funzionalità da implementare (Server Side) :soon:

:white_check_mark: N utenti (client) possono accedere ad un sistema di chat <br />
:white_check_mark:	Gli utenti devono potersi registrare e loggare al server <br />
:white_check_mark:	Registrazione può essere sviluppata tramite database sql o file <br />
:white_check_mark:	Gli utenti possono creare stanze <br />
:white_check_mark:	Gli utenti possono chattare solo con gli utenti presenti nella stanza <br />
:white_check_mark:	Gli utenti possono accedere a stanze già create <br />
:white_check_mark:	Il creatore della stanza deve poter accettare un utente che si vuole aggiungere alla stanza <br />
:white_check_mark:	[OPZIONALE] Usare un database Mysql or PostgreSQL <br />


## TO DO :page_with_curl:

**Cose a cui dare priorità (Vale)**
 - Controllare le deallocazioni
 - Implementare i lock per i thread

**Logica di controllo**
  - :black_square_button: Controllo e gestione degli errori :construction:WIP:construction:
  - :white_check_mark: Controllo se utente già loggato

**Impementazione della logica dei Client all'interno delle stanze**
  - :white_check_mark: Entrare 
  - :white_check_mark: Uscire se si disconnette
  - :white_check_mark: Uscire a comando da una stanza
  - :white_check_mark: Scrivere nella chat a tutti i presenti
  - :black_square_button: Passare la propietà della stanza, se sei il master ed esci, ad un utente casuale
  - :black_square_button: OPZIONALE: Passare la proprietà della stanza a un utente a scelta
  - :black_square_button: OPZIONALE: Disconnettersi normalmente dalla app da una stanza qualsiasi

**Logica di cancellazione delle stanze**
  - :black_square_button: Cancellare la stanza se l'ultima persona esce
  
  - :black_square_button: OPZIONALE: Il master può cancellare la stanza
  - :white_check_mark: Cacciare la gente quando la stanza viene cancellata
 
 **Client Android :construction:WIP:construction:** 
  - :white_check_mark: Login e register
  - :white_check_mark: Creare, entrare e accettare nelle stanza
  - :white_check_mark: Invio messaggi
  - :black_square_button: Timeout per le richieste di entrata
  - :black_square_button: Limite ai caratteri dei messaggi
  - :black_square_button: Grafica chat

  **Funzionalià interamente opzionali da sviluppare per utlime se ci va**
  - Propic
  - Scegliere a chi passare la proprietà della stanza
  - Master può cancellare la stanza
 
## API :books:

|Tag                  | Server                     | Client
|:---                 | :---:                      | :---:
|\[MSG\]data<>room_id | Send message brodcast room | :boom:
|\[LGN\]user<>pass    | Send "Login succeful<>id"  | :boom:
|\[RGT\]user<>pass    | Send "Register successful" | :boom:
|\[CRT\]RoomName      | Send "Register successful" | :boom:
|\[ACC\]sock<>room_id | Send "Register successful" | :boom:
|\[NAC\]sock_client   | Send "Register successful" | :boom:
|\[RQT\]room_id       | Send "Register successful" | :boom:
|\[EXT\]room_id       | TODO: Send "[EXT]"         | :boom:


