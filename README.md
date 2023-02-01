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
  - Tutto 
 
## API :books:

|Tag    | Success                   | Failed
|:---:  | :---:                     | :---:
|\[MSG\]| Send message brodcast room| :boom:
|\[LGN\]| Send "Login succeful"     | :boom:

