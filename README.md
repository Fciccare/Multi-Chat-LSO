# Multi-Chat-LSO

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

**Logica di controllo**
  - :black_square_button: Controllo e gestione degli errori
  - :black_square_button: Controllo se utente già loggato

**Impementazione della logica dei Client all'interno delle stanze**
  - :white_check_mark: Entrare 
  - :black_square_button: Uscire se si disconnette
  - :black_square_button: Uscire a comando
  - :white_check_mark: Scrivere nella chat a tutti i presenti
  - :black_square_button: OPZIONALE: Passare la proprietà della stanza

**Logica di cancellazione delle stanze**
  - :black_square_button: Cancellare la stanza se il master esce
  - :black_square_button: Cacciare la gente quando la stanza viene cancellata
 
 **Client Android**
  - Tutto - Login e Regiser

  **Funzionalià interamente opzionali da sviluppare per utlime se ci va**
  - Propic
  - Scegliere a chi passare la proprietà della stanza
 
## API :books:

|Tag                  | Server                     | Client
|:---:                | :---:                      | :---:
|\[MSG\]data<>room_id | Send message brodcast room | :boom:
|\[LGN\]user<>pass    | Send "Login succeful"      | :boom:
|\[RGT\]user<>pass    | Send "Register successful" | :boom:
|\[CRT\]RoomName      | Send "Register successful" | :boom:
|\[ACC\]sock<>room_id | Send "Register successful" | :boom:
|\[NAC\]sock_client   | Send "Register successful" | :boom:
|\[RQT\]room_id       | Send "Register successful" | :boom:


