Sonoff Ledstrip Controller
=======================
Con questo firmware il Sonoff può comunicare tramite protocollo MQTT con un broker, e in modalità stand-alone accedendo alla homepage del dispositivo. Una volta connesso alla rete wifi sarà accessibile in localhost o all'indirizzo 192.168.4.1 collegandosi alla sua rete wifi.

<p align="center">
  <img src="https://github.com/aletiffi/Led_Sonoff/blob/main/img/HomePage.PNG" alt="Settings"/>
</p>

Collegamenti
-----------------------------------------
In questo progetto viene utilizzato un Sonoff r1, una striscia a led ws2812b ed un alimentatore, dimensionato opportunamente per il carico richiesto dalla striscia a led.

<p align="center">
  <img src="https://github.com/aletiffi/Led_Sonoff/blob/main/img/Schema.png" alt="Settings"/>
</p>

Setup
-----------------------------------------
Caricato il firmware, al primo avvio la EEPROM viene cancellata, e viene assegnato al dispositivo un nome di default.

Una volta connessi alla rete wifi con il nome di defalut del dispositivo, aprire il browser all'indirizzo 192.168.4.1. Rinominare il dispositivo quindi inserire il nome della rete wifi e la password al quale deve connettersi. Le altre impostazioni possono essere inserite in un secondo momento. Premendo il pulsante `Save Settings` in fondo alla pagina il Sonoff viene riavviato, quindi cerca di connettersi alla rete impostata.

<p align="center">
  <img src="https://github.com/aletiffi/Led_Sonoff/blob/main/img/Settings.PNG" alt="Settings"/>
</p>

L'avvenuta connessione è indicata dall'accensione del led integrato, nel caso in cui non si accenda il segnale potrebbe essere troppo debole, per riavviare il processo di connessione tenere premuto il pulsante fino ai 2 blink del led.

Ora il dispositivo è connesso alla rete locale, ed è possibile inserire i parametri MQTT tramite la pagina web raggiungibile in localhost, inserendo nel browser `NOMEDISPOSITIVO.local`.

Inserendo il ProductKey di OTA Drive sarà possibile gestire tramite questa piattaforma l'aggiornamento dei futuri firmware senza flashare il dispositivo fisicamente.

Funzionalità del pulsante integrato
-----------------------------------------
Mantenendo premuto il pulsante del Sonoff il led eseguirà un lampeggio al secondo. In base al numero di lampeggi raggiunti al rilascio si attiverà una delle seguenti funzioni.

- 1:  Toggle
- 2:  Connection check
- 3:  Check FW update
- 4:  Show ip address
- 5:  Wifi signal power
- 6:  Load settings
- 7:  Save settings
- 8:  EEPROM clean
- 9:  Read all EEPROM
- 10: Sonoff restart