# Multi-Chat-LSO

## Funzionalità da implementare :soon:

:white_check_mark: N utenti (client) possono accedere ad un sistema di chat <br />
:white_check_mark:	Gli utenti devono potersi registrare e loggare al server <br />
:white_check_mark:	Registrazione può essere sviluppata tramite database sql o file <br />
:white_check_mark:	Gli utenti possono creare stanze <br />
:black_square_button:	Gli utenti possono chattare solo con gli utenti presenti nella stanza <br />
:black_square_button:	Gli utenti possono accedere a stanze già create <br />
:black_square_button:	Il creatore della stanza deve poter accettare un utente che si vuole aggiungere alla stanza <br />
:white_check_mark:	[OPZIONALE] Usare un database Mysql or PostgreSQL <br />


## TO DO :page_with_curl:
**Impementazione della logica dei Client all'interno delle stanze**
  - :white_check_mark: Entrare e uscire 
  - :white_check_mark: Scrivere
  - :white_check_mark: OPZIONALE: Passare la proprietà della stanza

**Logica di cancellazione delle stanze**
  - :white_check_mark: Cancellare la stanza se il master esce
  - :white_check_mark: Cacciare la gente quando la stanza viene cancellata
 
## API :books:

|Tag    | Success                   | Failed
|:---:  | :---:                     | :---:
|\[MSG\]| Send message brodcast room| :boom:
|\[LGN\]| Send "Login succeful"     | :boom:

