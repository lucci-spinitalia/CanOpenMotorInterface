=================================[ Manuale interno ]=================================

Rev.9 -- da confrontare con il manuale cliente! --
Data 1/09/2015


# Indice

  1. Descrizione generale
  2. Guida rapida al sistema Alma3d
  3. Determinazione della posizione del tripode
  4. Operazioni con il tripode
  5. I file di simulazione
  6. Gestione degli errori asincroni
  7. Processi
  8. Funzionalita da implementare
  9. Unittest

  A. Unita' di misura
  B. Stati del sistema
  C. Riferimenti


# 1. Descrizione generale

Il tripode Spinitalia Alma3D e' dotato di una connessione Ethernet 10/100 accessibile tramite il connettore RJ45 (CN09) del quadro di comando generale (QCG).

Una volta connesso alla rete, il tripode si configura con un indirizzo IP statico pre-configurato. E' possibile modificare il suddetto indirizzo tramite l'apposito comando.

All'avvio del sistema viene eseguito come servizio il programma _"alma3d_service"_, che si occupa di aprire due connessioni TCP verso l'esterno, una per i comandi ed una per le posizioni in coordinate Roll, Pitch e Yaw; eseguire il programma _"alma3d_canopenshell"_; dei comandi inviati alcuni li invia al programma _"alma3d_canopenshell"_, altri li utilizza ed elabora direttamente; implementa al suo interno gli algoritmi per la conversione del file di simulazione in Roll Pitch e Yaw in _coordinate motori, e per la decodifica delle posizioni motori nella terna Roll, Pitch e Yaw. Si occupa inoltre di gestire la barriera infrarossi e la sirena.

Il programma _"alma3d_canopenshell"_ è scritto in linguaggio C e si occupa di dialogare con i motori, nonchè di inizializzare e gestire l'intero stack canopen. Ogni comunicazione da e verso il programma avviene tramite il suo standard input/output. Per l'invio delle posizioni e dello stato dei motori in tempo reale viene usata una named pipe creata all'avvio del servizio. 

Per distinguere i sorgente ed il destinatario dei messaggi descritti nel testo, viene adottata la seguente convenzione:

    <<<< <messaggio da alma3d_canopenshell destinato a alma3d_service>
    
    >>>> <messaggio da alma3d_service destinato a alma3d_canopenshell>
    
    %%%% <messaggio dalla named pipe di alma3d_canopenshell>
    
    <tcp< <messaggio da alma3d_service destinato all'utente>
    
    >tcp> <messaggio dall'utente destinato ad alma3d_service>
    
    %tcp% <messaggio dalla seconda connessione di alma3d_canopenshell>

#2. Guida rapida al sistema Alma3d

##2.1 Ricerca dell'indirizzo del tripode

Un metodo per trovare l'indirizzo IP consiste nel richiedere la risoluzione inversa al server WINS del computer SPINITALIA_ALMA3D.

##2.2 Login

Una volta scoperto l'indirizzo IP, bisogna innanzitutto fornire le credenziali di accesso al sistema, tramite il comando LGN. A seguito del riconoscimento dell'utente è possibile interagire inviando gli altri comandi.

Esempio:

    >tcp> LGN alma_user spinitalia
    <tcp< OK LGN

##2.3 Inizializzazione del sistema

Prima di compiere qualsiasi operazione è necessario inizializzare il sistema con il comando CT0, possibilmente seguito dall'indicazione del peso montato sul piatto.

Esempio:

    >tcp> CT0 W50
    <tcp< OK CT0

##2.4 Homing

Se il controllo iniziale del sistema viene eseguito con successo, è possibile passare alla procedura di homing, mandatoria alla prima accensione e dopo ogni uscita da uno stato di emergenza.

Esempio:

    >tcp> CT2 P1
    <tcp< CT2 P1

Da questo momento il tripode spinitalia alma3d è pronto per eseguire simulazioni o spostamenti.

##2.5. Invio di una simulazione

Per memorizzare un file della simulazione all'interno del sistema, basta accedere alla directory condivisa \\\\SPINITALIA-ALMA3D\simulazioni e salvarlo al suo interno.

##2.6. Conversione della simulazione

Prima di poter avviare la simulazione e necessario convertire i dati nel formato riconosciuto dai motori, oltre ad eseguire un controllo sulla congruità dei dati passati. Queste funzioni sono racchiuse nel comando CT3, che prende come parametro il nome della simulazione contenuta nella precedente cartella.

Esempio:

    >tcp> CT3 simulazione.csv
    <tcp< OK CT3

##2.7. Avvio simulazione

La simulazione convertita può essere avviata tramite il comando CT4. Il tripode spinitalia alma3d risponderà con un OK CT4 concluso la serie di movimenti richiesti.

Esempio:

    >tcp> CT4
      ...
    <tcp< OK CT4

##2.8. Centraggio

Dopo la fine della simulazione, è necessario centrare il sistema prima di poterne avviare un'altra, tramite il comando __*CT2 P2*__

Esempio:

    >tcp> CT2 P2
    <tcp< OK CT2 P2

##2.9. Comando di posizionamento (GOTO)

Quando si vuole posizionare il tripode spinitalia alma3d con un certo angolo, dalla posizione "CENTRATO" basta inviare il comando CT1

Esempio:

    >tcp> CT1 R20 P10 Y360 V30
    <tcp< OK CT1

##2.10. Spegnimento del sistema

Per spegnere il sistema inviare il comando CT6, che riporterà il tripode all'altezza minima ed inviarà il segnale di shutdown al controllore.

Esempio:

    >tcp> CT6
    <tcp< OK CT6

##2.11 Uso tipico del tripode spinitalia alma3d

    >tcp> LGN alma_user spinitalia
    <tcp< OK LGN

    >tcp> CT0 W50
    <tcp< OK CT0

    >tcp> CT2 P1
    <tcp< CT2 P1

    >tcp> CT3 simulazione.csv
    <tcp< OK CT3

    >tcp> CT4
        .
        .
        .
    <tcp< OK CT4

    >tcp> CT2 P2
    <tcp< OK CT2 P2

    >tcp> CT6
    <tcp< OK CT6


# 3. Determinazione della posizione del tripode


## 3.1. Determinazione delle posizioni dal programma alma3d_canopenshell

Il programma alma3d_canopenshell apre una pipe all'avvio, che si trova nella directory /tmp e ha nome fake_alma_3d_spinitalia_pos_stream_pipe se in funzionamento virtuale oppure alma_3d_spinitalia_pos_stream_pipe se in funzionamento reale.

Una volta aperta fornisce le seguenti informazioni secondo il seguente formato:

    %%%% @M119 S0 @M120 S0 @M121 S0 @M122 S0 AS6 T9 C0

Le informazioni fornite possiedono la seguente notazione:

  Sigla  | Descrizione                           | Intervallo valori ammesso
  ------ | ------------------------------------- | --------------------------
  @M-    | Indirizzo motore                      | 119 ... 122
  S-     | Step del motore                       | -320000 ... 320000 (+/-2^31)
  AS-    | Stato del tripode                     | (vedi appendice B)
  T-     | Periodo di invio dei messaggi \[ms\] | 0.00 ... 999.99 (tipico 10.00)
  C-     | Progresso analisi/simulazione \[%\]  | 0 ... 100

Esempio:

    %%%% @M119 S0 @M120 S100 @M121 S1234 @M122 S320000 AS6 T9.98 C0

I motori identificati come @M120...122 comandano i tre pistoni verticali, possiedono 8000 step in un giro, ed in ogni giro si elevano di 10mm. Lo zero corrisponde ad un altezza di 400mm più e l'escurione totale ad 800mm. La distanza centro sfera centro cerniera dei pistoni e' di 1285mm. Il motore identificato come @M119 comanda la rotazione del piatto, possiede 8000 step in un giro e la movimentazione passa attraverso un riduttore da 1:115. 

## 3.2. Determinazione delle posizioni dal programma Alma3d

Il sistema legge le informazioni sulla posizione fornite da alma3d_canopenshell in step motore, le trasforma le altezze dei pistoni e poi in angoli R, P e Y.

Connettendosi alla porta 10001 del tripode si puo' ricevere la posizione dello stesso, che viene trasmessa come un flusso continuo di coordinate in questa forma:

    %tcp% R12.321;P-2.23;Y0;AS0;T10;C0
    %tcp% R12.321;P-2.23;Y0;AS0;T10;C0

Il tempo, come nel caso di alma3d_canopenshell, rappresenta i ms intercorsi tra un'informazione e l'altra. Gli angoli ed il tempo sono espressi secondo la seguente notazione:

  Sigla | Descrizione       | Range valori
  ------|-------------------|------------------
  (R)   | Roll [gradi]      | -45.00 ... +45.00
  (P)   | Pitch [gradi]     | -45.00 ... +45.00
  (Y)   | Yaw [gradi]       | 0.00 ... 360.00
  (AS)  | Stato del tripode | (vedi appendice B)
  (T)   | Time [ms]         | 0.00 ... 999.99
  (C)   | Progress[%]       | 0 ... 100

Il tempo si intende come quello impiegato a raggiungere la posizione indicata, e viene espresso in ms.

#4. Operazioni con il tripode

Il controllo del Tripode Spinitalia Alma 3d avviene tramite procotollo TCP sulla porta 10002. Per le possibili interazioni tra i comandi e gli stati si rimanda alla figura 1 (tripode.png). Le seguenti affermazioni circa il protocollo sono sempre valide:

  - I comandi, sia via TCP che via PIPE, restituiscono se a buon fine:

        OK <cmd>
        
  - I comandi, sia via TCP che via PIPE, restituiscono se in errore:

        CERR <cmd> <num>: <descr>
        
  - Esistono errori non causati dall'esecuzione di un comando. In tal caso puo' essere restituito un errore asincrono oltre alla risposta normale al comando indicata nei punti precedenti:

        AERR <num>: <descr>

I comandi sono codificati in modo che siano raggruppati in famiglie funzionali:

  Comando | Descrizione
  --------|------------------------------------------------
  LGN     | Accesso al sistema (LoGiN)
  CT<x>   | Avvio della procedura x (Comando Tripode)
  EM<x>   | Avvio del comando di emergenza x (EMergenza)
  PR<x>   | Lettura o scrittura del parametro x (PaRametro)

##4.1 Differenze tra i comandi Alma3d e quelli alma3d_canopenshell

Anche se il formato tra i comandi Alma3d e alma3d_canopenshell è simile, le due implementazioni possono presentare parametri diversi. La spiegazione di tali differenze risiede nel fatto che il primo implementa delle procedure specifiche per l'applicazione, mentre il secondo rimane ad un livello di astrazione più alto.

Di seguito vengono elencate le corrispondenze tra i comandi Alma3d e alma3d_canopenshell, rimandando più avanti una descrizione più dettagliata.

  Alma3d                  | alma3d_canopenshell
  ------------------------|---------------------
   LGN                    |  -
  ------------------------|-------------------------------
   CT0 [Waa]              | CT0 M4
                          | \[PR5 M120 O60FB S008 T32s xx\]
                          | \[PR5 M121 O60FB S008 T32s xx\]
                          | \[PR5 M121 O60FB S008 T32s xx\]
   CT1 Raa Pbb Ycc Vdd    | CT1 M120 Pxx VMyy AMzz
                          | CT1 M121 Pxx VMyy AMzz
                          | CT1 M122 Pxx VMyy AMzz S
   CT2 P1                 | CT2 P1
   CT2 P2                 | CT2 P1
   CT2 P3                 | CT2 P3
   CT2 P4                 | CT1 M119 P312000 VMyy AMzz
                          | CT1 M120 P312000 VMyy AMzz
                          | CT1 M121 P312000 VMyy AMzz
                          | CT1 M122 P312000 VMyy AMzz S
   CT3 <nome file>        | -
   CT4                    | PR5 M120 O2101 S03 T16u 2
                          | PR5 M121 O2101 S03 T16u 2
                          | PR5 M122 O2101 S03 T16u 2
                          | PR5 M120 O2101 S03 T16u 3
                          | PR5 M121 O2101 S03 T16u 3
                          | PR5 M122 O2101 S03 T16u 3
                          | CT4
   CT5                    | CT5
   CT6                    | CT1 M119 P312000 VMyy AMzz
                          | CT1 M120 P312000 VMyy AMzz
                          | CT1 M121 P312000 VMyy AMzz
                          | CT1 M122 P312000 VMyy AMzz S
                          | CT6
  ------------------------|---------------------------------
   EM1                    | EM1
   EM2                    | EM2
  ------------------------|---------------------------------
   PR1                    | -
   PR2                    | -
   PR3 A\[R\|P\|Y\] Lxx Uyy | -
   PR4 xxx yyy            | -
   -                      | PR5 Mxx Oyy Szz T8u ww
   PR6 xx yy              | -

##4.2 Descrizione comandi Alma3d

Qui di seguito viene riportato l'elenco completo le procedure implementate:

### LGN \<user\> \<password\>

Accede al sistema tramite le credenziali fornite. Il nome utente è fisso, 'alma_user', mentre la password di default é 'spinitalia'. Per l'accesso come amministratore in caso di manutenzione, basta loggarsi come 'alma3d_admin' e password da decidere.

In caso di successo viene inviata la stringa:

    <tcp< OK LGN

In caso di errore:

    <tcp< CERR LGN 0: Wrong password

Esempio:

    >tcp> LGN alma_user pippo
    <tcp< CERR LGN 0: Wrong password
    
    >tcp> LGN alma_user spinitalia
    <tcp< OK LGN

### CT0 \[Wx\]
        
Avvia la procedura di inizializzazione dei 4 motori inviando il comando CT0 M4 al programma alma3d_canopenshell. Se l'inizializzazione va a buon fine, allora viene restuito:

    <tcp< OK CT0    

altrimenti:

    <tcp< CERR CT0 0: Motori dichiarati non trovati

Esempio:

    >tcp>CT0
    >>>> CT0 M4
    <<<< @A119
    <<<< @A120
    <<<< CERR CT0 0: Motori dichiarati non trovati
    <tcp< CERR CT0 0: Motori dichiarati non trovati
    
    >tcp>CT0
    >>>> CT0 M4
    <<<< @A119
    <<<< @A120
    <<<< @A121
    <<<< @A122
    <<<< OK CT0
    <tcp< OK CT0

Il parametro opzionale W indica il peso caricato sul piatto del tripode. Nel caso fosse presente, il programma Alma3d si occupa di mandare dei messaggi aggiuntivi per impostare il PID dei motori per avere migliori prestazioni dinamiche.

Esempio:

    >tcp>CT0 W100
    >>>> CT0 M4
    <<<< @A119
    <<<< @A120
    <<<< @A121
    <<<< @A122
    <<<< OK CT0
    <tcp< OK CT0
    
    >>>> PR5 M120 O60FB S008 T32s C3500
    <<<< OK PR5
    >>>> PR5 M121 O60FB S008 T32s C3500
    <<<< OK PR5
    >>>> PR5 M122 O60FB S008 T32s C3500
    <<<< OK PR5

### CT1 R<roll> P<pitch> Y<yaw> V<% vel max>

Porta il tripode nel punto identificato dalla terna RPY. Alma3d converte i punti dalle RPY in step motore tramite l'algoritmo di cinematica inversa ed invia tanti comandi CT1 Mxx Pyy VMzz AMww da passare a alma3d_canopenshell. La velocità massima è definita in un file di configurazione chiuso ed il termine <% vel max> rappresenta la percentuale di velocità massima da utilizzare. Alma3d si preoccupa anche di controllare se la destinazione finale impostata rientra nell'area di lavoro del tripode.

Se i parametri sono corretti e l'operazione va a buon fine, viene restituito:

    <tcp< OK CT1

altrimenti:

<   tcp< CERR CT1 0: 

Per esempio, per spostare il tripode nella posizione R20 P10 Y360 con una velocità pari al 30% di quella massima:

    >tcp> CT1 R20 P10 Y360 V30
    >>>> CT1 M119 P1231232 VM123 AM124
    <<<< OK CT1
    >>>> CT1 M120 P320000 VM123 AM124
    <<<< OK CT1
    >>>> CT1 M121 P34587 VM123 AM124
    <<<< OK CT1
    >>>> CT1 M122 P3156 VM123 AM124 S
    <<<< OK CT1
    <tcp< OK CT1

### CT2 P1

Copia la posizione di HOME predefinita in ogni file dei 4 motori ed avvia la procedura di ricerca il fine corsa. Questo comando serve per impostare la posizione iniziale dei motori i quali non presentano un encoder assoluto integrato.Una volta conclusa la procedura, il tripode si trova nella posizione di HOME.

In caso di successo, viene restituito:

    <tcp< OK CT2 P1

altrimenti:

    <tcp< CERR CT2 P1

Esempio:

    >tcp> CT2 P1
    >>>> CT2 P1
    <<<< CT2 P1
    <tcp< CT2 P1

### CT2 P2

Sposta il sistema nella posizione di HOME.

Esempio:

    >tcp> CT2 P2
    >>>> CT2 P2
    <<<< CT2 P2
    <tcp< CT2 P2

### CT2 P3

Rilascia il controllo dei motori con l'effetto dell'abbassamento dei pistoni per effetto gravità. Rimane comunque attivo il freno-motore così da rallentarne la caduta.

### CT2 P4

Porta il sistema al minimo di altezza controllando la discesa inviando dei comandi CT1 a tutti i motori con impostata l'altezza minima raggiungibile.

In caso di successo, viene restituito:

    <tcp< OK CT2 P4
          
altrimenti:
 
    <tcp< CERR CT2 P4

Esempio:

    >tcp> CT2 P4
    >>>> CT1 M119 P312000 VM300000 AM100
    <<<< OK CT1
    >>>> CT1 M120 P312000 VM300000 AM100
    <<<< OK CT1
    >>>> CT1 M121 P312000 VM300000 AM100
    <<<< OK CT1
    >>>> CT1 M122 P312000 VM300000 AM100
    <<<< OK CT1
    <tcp< OK CT2 P4
          

### CT3 \<nome file\>

Avvia il controllo della simulazione \<nome file\> che deve essere presente nella nella cartella condivisa '\\TRIPODEALMA3D\' 

Se il contenuto del file risulta regolare, il comando restituisce:

    <tcp< OK CT3

Durante la conversione, il processo indica la percentuale di completamento nello stream della porta 10001.

Esempio:

    >tcp> CT3 simulazione.csv
    %tcp% R0;P0;Y0;AS7;T10.0;C0
    %tcp% R0;P0;Y0;AS7;T10.0;C1
    %tcp% R0;P0;Y0;AS7;T10.0;C5
    ...
    %tcp% R0;P0;Y0;AS7;T10.0;C100
    <tcp< OK CT3

### CT4

Per prima cosa abilita i limiti di giunto per i pistoni per garantire la movimentazione completa, ma li lascia disabilitati nel motore di rotazione (yaw).

    >>>> PR5 M120 O2101 S03 T16u 2
    >>>> PR5 M120 O2101 S03 T16u 3

Avvia la simulazione e, una volta terminata, il sistema restituisce:

    <tcp< OK CT4

Invece, nel caso la simulazione sia interrotta da un evento asincrono o da un comando, viene restituito:

    <tcp< CERR CT4

Durante la simulazone, il processo indica la percentuale di completamento nello stream della porta 10001.

Esempio:

    >tcp> CT4
    >>>> PR5 M120 O2101 S03 T16u 2
    <<<< OK PR5
    >>>> PR5 M121 O2101 S03 T16u 2
    <<<< OK PR5
    >>>> PR5 M122 O2101 S03 T16u 2
    <<<< OK PR5
    >>>> PR5 M120 O2101 S03 T16u 3
    <<<< OK PR5
    >>>> PR5 M121 O2101 S03 T16u 3
    <<<< OK PR5
    >>>> PR5 M122 O2101 S03 T16u 3
    <<<< OK PR5
    >>>> CT4
    %%%% @M119 S0 @M120 S0 @M121 S0 @M122 S0 AS7 T9.98 C0
    %tcp% R0.000;P0.000;Y0.000;AS7;T10.0;C0
    %%%% @M119 S100 @M120 S0 @M121 S0 @M122 S0 AS7 T10.00 C1
    %tcp% R0.123;P0.000;Y0.000;AS7;T10.0;C1
    %%%% @M119 S200 @M120 S0 @M121 S0 @M122 S0 AS7 T9.96 C5
    %tcp% R0.500;P0.000;Y0.000;AS7;T10.0;C5
    ...
    %%%% @M119 S-320000 @M120 S0 @M121 S0 @M122 S0 AS7 T9.98 C100
    %tcp% R22.500;P0.000;Y0.000;AS7;T10.0;C100
    <<<< OK CT4
    <tcp< OK CT4

### CT5

Arresta la simulazione interrompendola. Questo comando è l'unico ad essere accettato durante una simulazione, e provoca le seguenti risposte:

    CERR CT4 0: Simulazione interrotta
    OK CT5

Esempio:

    >tcp> CT4
    >>>> PR5 M120 O2101 S03 T16u 2
    <<<< OK PR5
    >>>> PR5 M121 O2101 S03 T16u 2
    <<<< OK PR5
    >>>> PR5 M122 O2101 S03 T16u 2
    <<<< OK PR5
    >>>> PR5 M120 O2101 S03 T16u 3
    <<<< OK PR5
    >>>> PR5 M121 O2101 S03 T16u 3
    <<<< OK PR5
    >>>> PR5 M122 O2101 S03 T16u 3
    <<<< OK PR5
    >>>> CT4
    %%%% @M119 S0 @M120 S0 @M121 S0 @M122 S0 AS7 T9.98 C0
    %tcp% R0.000;P0.000;Y0.000;AS7;T10.0;C0
    %%%% @M119 S100 @M120 S0 @M121 S0 @M122 S0 AS7 T10.00 C1
    %tcp% R0.123;P0.000;Y0.000;AS7;T10.0;C1
    %%%% @M119 S200 @M120 S0 @M121 S0 @M122 S0 AS7 T9.96 C5
    %tcp% R0.500;P0.000;Y0.000;AS7;T10.0;C5
    ...
    >tcp> CT5
    >>>> CT5
    <<<< CERR CT4 0: Simulazione interrotta
    <tcp< CERR CT4 0: Simulazione interrotta
    <<<< OK CT5
    <tcp< OK CT5

### CT6

Spegne il sistema portando il piano in posizione di riposo orizzontale alla sua altezza minima. Al termine dell'operazione invia:

    <tcp< OK CT6

Successivamente esegue l'arresto completo del dispositivo.

Esempio:
    
    >tcp> CT6
    >>>> CT1 M119 P312000 VM300000 AM100
    <<<< OK CT1
    >>>> CT1 M120 P312000 VM300000 AM100
    <<<< OK CT1
    >>>> CT1 M121 P312000 VM300000 AM100
    <<<< OK CT1
    >>>> CT1 M122 P312000 VM300000 AM100
    <<<< OK CT1
    >>>> CT6
    <<<< OK CT6
    <tcp< OK CT6

### EM2

Blocca tutti i motori ovunque essi siano. Normalmente restituisce:

    <tcp< OK EM2

Questo comando di emergenza viene inviato automaticamente da Alma3d quando vengono interrotte le barriere infrarossi.

### PR1

Richiedi lo stato del tripode spinitalia. Nell'appendice B è possibile consultare i diversi valori restituiti.

Esempio:
        
    >tcp> PR1
    >>>> PR1
    <<<< OK PR1: 4, Inizializzato
    <tcp< OK PR1: 4, Inizializzato
    

### PR2

Viene richiesta la posizione corrente della piattaforma.

    R:34.100 P:12.200 Y:330.200
    OK PR2

### PR3 A\[R\|P\|Y\] L00.000 U00.000

Imposta i limiti di uno dei tre giunti ( Roll, Pitch, Yaw ), tramite due valori, Lower ed Upper.

### PR4 \<indirizzo ip\> \<netmask\> \<gateway\>

Imposta l'Ip, il Netmask ed il Getaway.
        
Esempio:

    >tcp> PR4 192.168.178.2 255.255.255.0 192.168.178.1
    <tcp< OK PR4

### PR6 \<user\> \<nuova_password\>

Imposta la password di accesso al sistema, che deve essere una stringa alfanumerica di 8-32 caratteri, scelti tra [0-9], [a-z], [A-Z], _ e -.

Esempio:

    >tcp> PR6 simone spano
    <tcp< OK PR6

## 4.3 Descrizione comandi alma3d_canopenshell

### CT0 M  num motori \>

Invia un segnale di reset a tutti i dispositivi canopen presenti sul bus che ne impone la dichiarazione. Ad ogni motore che si dichiara viene generata in output la seguente stringa:

    <<<< @A<indirizzo motore>
        
Se dopo un certo periodo di tempo i motori che si sono dichiarati sono proprio <num motori>, allora alma3d_canopenshell restituisce:

    <<<< OK CT0

altrimenti:

    <<<< CERR CT0 0: Motori dichiarati non trovati

Esempio:

    >>>> CT0 M4
    <<<< @A119
    <<<< @A120
    <<<< CERR CT0 0: Motori dichiarati non trovati

    >>>> CT0 M4
    <<<< @A119
    <<<< @A120
    <<<< @A121
    <<<< @A122
    <<<< @A123
    <<<< CERR CT0 0: Motori dichiarati non trovati

    >>>> CT0 M4
    <<<< @A119
    <<<< @A120
    <<<< @A121
    <<<< @A122
    <<<< OK CT0

### CT1 M<motore> P<step> VM<velocità> AM<accelerazione> [S]

Porta il tripode nel punto identificato dalle quattro coordinate in step dei motori, con una velocita' <velocità> in unita' motore con un'accellerazione <accelerazione> in unità motore. 

Se il comando viene impartito senza il parametro opzionale [S], il motore interessato carica la posizione finale, ma non viene avviato. Nel momento in cui viene inviato il comando con il parametro [S], tutti i motori precaricati si avviano. Questo è un modo per far partire i movimenti comandati in modo sincrono.
          
Se i parametri sono corretti e l'operazione va a buon fine, viene restituito:

    <tcp< OK CT1

altrimenti:

    <tcp< CERR CT1

Esempio:

    >>>> CT1 M119 P1231232 VM123 AM124
    <<<< OK CT1
    >>>> CT1 M120 P320000 VM123 AM124
    <<<< OK CT1
    >>>> CT1 M121 P34587 VM123 AM124
    <<<< OK CT1
    >>>> CT1 M122 P3156 VM123 AM124 S
    <<<< OK CT1

I parametri <velocità> ed <accelerazione> sono espressi in unità poprietarie del motore. Per convertire una velocità espressa in giri al secondo, oppure un'accelerazione espressa in giri al secondo^2, utilizzare le seguenti formule:

    VT = Velocity * 65536      ( Giri al secondo   )
    AT = Acceleration * 8192   ( Giri al secondo^2 )
    
Per l'attuatore rotativo, questi valori diventano:
    
    VT = Velocity * 65536 * 115 / 360     ( Gradi al secondo   )
    AT = Acceleration * 8192 * 115 / 360  ( Gradi al secondo^2 )
    
Per l'attuatore lineare, questi valori diventano:
    
    VT = Velocity * 65536 * 10     ( mm al secondo   )
    AT = Acceleration * 8192 * 10  ( mm al secondo^2 )

### CT2 P1

Avvia la procedura di ricerca il fine corsa di tutti i motori e si sposta nella posizione configurata come HOME. La posizione di HOME e la velocità di spostamento per la ricerca vengono letti dal file motore .mot, il quale deve contenere la stringa nel giusto formato (vedi "I file di simulazione")

In caso di successo, viene restituito:

    <tcp< OK CT2 P1

altrimenti, nel caso la stringa di homing non fosse conforme al formato atteso:

    <tcp< CERR CT2 P1

Esempio:
        
    >tcp> CT2 P1
    >>>> CT2 P1
    <<<< CT2 P1
    <tcp< CT2 P1

### CT2 P2
    
Sposta il sistema nella posizione di HOME.

### CT2 P3

Rilascia il controllo dei motori con l'effetto dell'abbassamento dei pistoni per effetto gravità. Rimane comunque attivo il freno-motore così da rallentarne la caduta.

### CT4

Avvia la simulazione prendendo le posizioni dai file motori .Una volta terminata, il sistema restituisce:

    <<<< OK CT4

Invece, nel caso la simulazione sia interrotta da un evento asincrono o da un comando, viene restituito:

    <<<< CERR CT4

Durante la simulazone, la percentuale di completamento viene aggiornata nello stream della pipe:

Esempio:
        
    >>>> CT4
    %%%% @M119 S0 @M120 S0 @M121 S0 @M122 S0 AS7 T9.98 C0
    %%%% @M119 S100 @M120 S0 @M121 S0 @M122 S0 AS7 T10.00 C1
    %%%% @M119 S200 @M120 S0 @M121 S0 @M122 S0 AS7 T9.96 C5
    ...
    %%%% @M119 S-320000 @M120 S0 @M121 S0 @M122 S0 AS7 T9.98 C100
    <<<< OK CT4

### CT5

Arresta la simulazione interrompendola. Questo comando è l'unico ad essere accettato durante una simulazione, e provoca le seguenti risposte:

    CERR CT4 0: Simulazione interrotta
    OK CT5

Esempio:
  
    >>>> CT4
    %%%% @M119 S0 @M120 S0 @M121 S0 @M122 S0 AS7 T9.98 C0
    %%%% @M119 S100 @M120 S0 @M121 S0 @M122 S0 AS7 T10.00 C1
    %%%% @M119 S200 @M120 S0 @M121 S0 @M122 S0 AS7 T9.96 C5
    ...
    >>>> CT5
    <<<< CERR CT4 0: Simulazione interrotta
    <<<< OK CT5

### CT6

Rilascia tutte le risorse canopen e chiude il programma. Appena prima della chiusura, viene inviato:

    <<<< OK CT6

Esempio:
    
    >>>> CT6
    <<<< OK CT6

### EM2

Blocca tutti i motori ovunque essi siano. Normalmente restituisce:

    <<<< OK EM2

### PR1

Richiedi lo stato del tripode spinitalia. Nell'appendice B è possibile consultare i diversi valori restituiti.

Esempio:
        
    >>>> PR1
    <<<< OK PR1: 4, Inizializzato

### PR5 M<motore> O<registro_hex> S<indice> T<num bit><tipo> <valore>

Cambia direttamente il valore del registro canopen <registro_hex> del motore <motore>.

I valori <registro_hex> devono essere registri validi e devono essere scritti in esadecimale senza il prefisso '0x'.
  
I valori <indice> devono essere degli indici validi e devono essere scritti in decimale.

Il parametro <num bit> indica il numero di bit di <valore> e può essere uno dei seguenti: 8, 16, 32

Il parametro <tipo> indica il tipo di dato passato 

Esempio:

Imposto a 0Fh il parametro con sub-index 5 dell'oggetto 60FBh con un 8bit unsigned ( <8/16/32><s/u> )

    PR5 M121 O2101 S03 T16u 2

# 5. I file di simulazione

Alma3d ed alma3d_canopenshell lavorano su diverse grandezze fisiche: mentre il primo accetta dei valori in posizione espressi nella terna RPY in gradi, il secondo vuole come input soltanto step motore. Quindi la prima rappresentazione viene trasformata tramite la cinematica inversa in quattro valori diversi, uno per ogni motore.

    {<roll><pitch><yaw>} -> {<step_motore1>} {<step_motore2>} {<step_motore3>} {<step_motore4>}

Per questo motivo, quando si genera una simulazione, dal file di input dell'utente con estensione .csv vengono prodotti 4 "file motore" con estensione .mot.

## 5.1 Il file simulazione per Alma3d

Il file simulazione prodotto dall'utente dovrà contenere delle righe contenenti i parametri nel seguente formato:

  <roll>;<pitch>;<yaw>;<time_to_reach_in_ms>;<optional comment>

La prima riga può anche essere l'intestazione del file: in questo caso verrà automaticamente ignorata.

Esempio:

  <roll>;<pitch>;<yaw>;<time_to_reach_in_ms>;<optional comment>
  0.000;0.000;0.000;010;
  0.002;0.000;0.000;010;
  0.008;0.000;0.000;010;
  0.018;0.000;0.000;010;
  0.032;0.000;0.000;010;
  0.050;0.000;0.000;010;

I parametri sono espressi secondo la seguente notazione:

  <roll>  -45.00 ... +45.00 gradi
  <pitch> -45.00 ... +45.00 gradi
  <yaw>     0.00 ... 360.00 gradi
  <time>       1 ... 256000  ms

Il parametro <time> si intende come quello necessario a raggiungere la posizione indicata, e viene espresso in ms, fino ad un massimo di 256000. Vista la sua funzione, il tempo non puo' essere minore di 1 ms, pena l'esclusione del comando.

## 5.2 Il file simulazione per alma3d_canopenshell

Le file motori devono rispettare il seguente formato:

  CT1 M<inirizzo motore> S<step> T<tempo>

e, per essere riconosciuti, devono essere nominati come:

  <indirizzo motore>.mot

e memorizzati nella sottocartella /tmp/spinitalia/motor_data/

I parametri sono espressi secondo la seguente notazione:

  <indirizzo motore>       1 ... 127
  <step>               -2^31 ... 2^31
  <tempo>                  1 ... 256000    ms

Il parametro <tempo> si intende come quello necessario a raggiungere la posizione indicata, e viene espresso in ms, fino ad un massimo di 256000. Vista la sua funzione, il tempo non puo' essere minore di 1 ms, pena l'esclusione del comando.

Esempio:

Per generare un file simulazione per il motore con indirizzo canopen 120, si deve prima creare il file

    /tmp/spinitalia/motor_data/120.mot

ed al suo interno è necessario scrivere le posizioni nel seguente formato:

    CT1 M120 S0 T10
    CT1 M120 S16 T10
    CT1 M120 S64 T10
    CT1 M120 S144 T10
    CT1 M120 S256 T10
    CT1 M120 S400 T10
    CT1 M120 S576 T10
    CT1 M120 S784 T10
    CT1 M120 S1024 T10
    CT1 M120 S1296 T10
    CT1 M120 S1600 T10

Quindi, prendendo in considerazione la seconda linea, una volta avviata la simulazione, il motore 120 si muoverà nella posizione 16 in 10 ms. Raggiunto l'obiettivo nel tempo stabilito, si muoverà nella posizione 64 in 10 ms, ecc.

E' importante che la formattazione ed il range dei parametri siano rispettati, altrimenti la riga non conforme verrà scartata e verrà eseguita la successiva, fino al raggiungimento della fine del file.

ATTENZIONE: alma3d_canopenshell non ha cognizione della cinematica, quindi non esiste alcuno controllo sulla posizione finale richiesta. Inoltre un cambiamento di posizione grande tra un riga e l'altro, oppure un tempo di raggiungimento troppo piccolo, potrebbe generare una richiesta in velocità troppo elevata, con conseguente blocco del motore (di solito segnalato come un "position error").

## 5.2 Il file di homing

Il comando di homing aziona la rotazione dei motori fino a quando viene catturato l'evento di raggiungimento del limite di giunto. Questo è un punto con posizione nota, quindi il motore può impostare il nuovo punto d'origine per poi posizionarcisi. Per eseguire questa procedura è necessario fornire delle informazioni, come la distanza in step del limite di giunto con il nuovo origine, la velocità di rotazione durante la ricerca e quella durante il raggiungimento della nuova origine. Il metodo per passare questi parametri è simile a quello utilizzato per i file motori, solo che, in questo caso, la formattazione diventa:

    CT1 M<indirizzo motore> H<step limite giunto> VF<vel ricerca> VB<vel ritorno>

Come per le simulazioni, i file devono essere nominati come:

    <indirizzo motore>.mot

e memorizzati nella sottocartella /tmp/spinitalia/motor_data/

I parametri sono espressi secondo la seguente notazione:

    <indirizzo motore>       1 ... 127
    <step limite giunto>     -2^31 ... 2^31
    <vel ricerca>            -2^-31 ... 2^31
    <vel ritorno>            -2^-31 ... 2^31

Il parametro <step limite giunto> rappresenta la distanza in step tra il limite e la nuova origine. Il segno indica la direzione di raggiungimento, quindi: nel caso in cui, per raggiungere l'origine, il motore deve girare nel senso opposto a quello di ricerca, il valore del parametro sarà negativo.

I parametri <vel ricerca> e <vel ritorno> hanno un'unità di misura proprio del motore. Per ottenere il loro valore partendo da velocità con unità di misura standard, utilizzare la seguente formula:

    VT = Velocity * 65536      ( Giri al secondo   )

Esempio:

Prima di avviare l'homing, si devono creare i file

    /tmp/spinitalia/motor_data/119.mot
    /tmp/spinitalia/motor_data/120.mot
    /tmp/spinitalia/motor_data/121.mot
    /tmp/spinitalia/motor_data/122.mot

ed al loro interno è necessario inserire i dati nel seguente formato:

    CT1 M120 H-320000 VF300000 VB900000

In questo caso il motore 120, appena raggiunto il limite di giunto, imposterà l'origine a -320000 step dalla posizione attuale nel verso opposto a quello di ricerca. La velocità di ricerca è di 300000 e quella di ritorno 900000.

# 6. Gestione degli errori asincroni

Qualora si verificasse un errore durante il funzionamento del sistema, le seguenti affermazioni sono sempre vere:

    - Il sistema puo' interrompere il suo funzionamento e cambiare di stato
    - Lo stream sulla porta 10001 riporta lo stato 0
    - A qualsiasi comando inviato il sistema risponde con AERR, e poi con OK o ERR a seconda della gravita' dell'evento.

Per conoscere lo stato del sistema e' possibile in ogni momento inviare il comando PR1.

# 7. Processi

  - TesInterface: Permette l'accesso al sistema dall'esterno. Contiene il gestore della connessione ethernet, ed il parser del protocollo. Consente l'aggiornamento del sistema stesso.
  - CinematicaDiretta: Si occupa della conversione delle coordinate angolari dei giunti in coordinate angolari del tripode
  - CinematicaInversa: Si occupa della conversione delle coordinate angolari del tripode fornite in forma tabellare, in tanti file quanti i giunti in coordinate angolari dei giunti
  - GestoreMotori: Si occupa del dialogo con i motori, fornendogli costantemente coordinate da raggiungere nella loro tabella interna, avviando e arrestando la simulazione.

# 8. Funzionalita da implementare

  [Stati obbligatori]
  - Il sistema deve attendere una connessione in ingresso [CONNECTED]
  - Il sistema deve attendere un'autenticazione [AUTHENTICATED]
  - Reset del sistema e conteggio motori [RESET]
  - Il sistema esegue l'homing [HOMING]
  - Il sistema deve attendere una simulazione [SIMULATION_UPLOADED]
    - Errore in caso di MD5SUM del file errata
    - Errore nel caso di coordinata del singolo giunto errata
    - I file rimangono temporanei finche' non termina la procedura
    - La procedura termina se tutti i files sono presenti
  - Il sistema deve eseguire il self test senza arrivare al limite di movimentazione per evitare di sbattere ad ostacoli [SELFTEST]
  - Solo dopo posso avviare

  [Stati facoltativi]
  - Viene avviata una simulazione
    - Vengono restituite le coordinate attuali
      - Errore della conversione, non bloccante
    - La percentuale di completamento

  [Stati sempre attivi]
  - Intrusione nel perimetro
  - Problemi stato motori
  - Stato del tripode indipendente dalla simulazione
  - Problemi di comunicazione CAN/CANOPEN
  - Quanti motori sono attivi e' compito di TesIntf chiederlo e segnalere l'errore ( la configurazione del tripode risiede in TesIntf )
  - TesIntf deve eseguire lo zero
  - TesIntf deve eseguire una manovra dimostrativa
  - TesIntf deve spostare il tripode in posizione attacca antenna
  - TesIntf ha la posizione dall'homing configurata per giunti
  - USB-AUTOMOUNT deve eseguire l'upgrade del firmware e del kernel

# A. Grandezze utilizzate

  \<roll\>:      -45.00 ... +45.00
  \<pitch\>:     -45.00 ... +45.00
  \<yaw\>:         0.00 ... 360.00
  \<tempo\>:          1 ... 256000
  \<uint\>:           0 ... 2^32 (unsigned int)
  \<status\>:         0 ... A

# B. Stati del sistema

Gli stati in cui può trovarsi il tripode sono i seguenti:

  - 0, Errore asincrono*
  - 1, Spento
  - 2, Emergenza
  - 3, Acceso
  - 4, Inizializzato
  - 5, In ricerca del centro
  - 6, Centrato
  - 7, In analisi del file fornito
  - 8, Simulazione
  - 9, Fermo
  - 10, In centraggio
  - 11, Rilasciato
  - 12, Nessuna credenziale fornita
  - 13, In posizione

*) Per ragioni estranee al comando in esecuzione, si possono verificare errori che causano il blocco del sistema, come ad esempio un calo della tensione di alimentazione dei motori per l'eccessivo sforzo. Questa condizione è indicata dallo stato 0. Per ottenere informazioni circa l'errore basta inviare un qualsiasi comando (ad esempio PR1), e nella risposta verrà esplicitata la condizione di errore prima della risposta al comando stesso.

# C. Riferimenti

1. https://tools.ietf.org/html/rfc1321
2. http://www.md5summer.org/
