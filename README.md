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

 - Gestione errore di Broadcast message (? c'è un TODO di questa cosa ma non so bene che si deve fare ?) 
 - Controllare le deallocazioni : Fatto in parte, controllare soprattutto per quando si chiude il server [Dipende quanto ci vogliamo impegnare, c'è qualcosa che rimane dalla cancellazione delle room]

**Logica di controllo**
  - :white_check_mark: Controllo e gestione degli errori
  - :white_check_mark: Controllo se utente già loggato

**Impementazione della logica dei Client all'interno delle stanze**
  - :white_check_mark: Entrare 
  - :white_check_mark: Uscire se si disconnette
  - :white_check_mark: Uscire a comando da una stanza
  - :white_check_mark: Scrivere nella chat a tutti i presenti
  - :white_check_mark: Passare la propietà della stanza, se sei il master ed esci, ad un utente casuale

**Logica di cancellazione delle stanze**
  - :white_check_mark: Cancellare la stanza se l'ultima persona esce
  - :white_check_mark: Cacciare la gente quando la stanza viene cancellata
 
 **Client Android :construction:WIP:construction:** 
  - :white_check_mark: Login e register
  - :white_check_mark: Creare, entrare e accettare nelle stanza
  - :white_check_mark: Invio messaggi
  - :white_check_mark: Timeout per le richieste di entrata
  - :white_check_mark: Limite ai caratteri dei messaggi
  - :black_square_button: Grafica chat

 
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
|\[EXT\]room_id       | Send "[EXT]"               | :boom:


