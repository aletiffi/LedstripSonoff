Sonoff Ledstrip Controller
=======================

In questo progetto viene utilizzato un Sonoff r1, una striscia a led ws2812b ed un alimentatore, dimensionato opportunamente per il carico richiesto dalla striscia a led.

`DA FARE Descrizione dei collegamenti elettrici DA FARE`

Con questo firmaware il Sonoff può comunicare tramite protocollo MQTT con un broker, o in modalità stand-alone accedendo alla homepage del dispositivo. Una volta connesso alla rete wifi sarà accessibile in localhost o all'indirizzo 192.168.4.1 collegandosi alla sua rete wifi.

Setup
-----------------------------------------
Una volta caricato il firmware, al primo avvio la EEPROM viene cancellata, e viene assegnato al dispositivo un nome di default.

Una volta connessi alla rete wifi con il nome di defalut del dispositivo, aprire il browser all'indirizzo 192.168.4.1 e rinominare il dispositivo con un nome a vostro piacere. Inserire il nome della rete wifi e la password al quale deve connettersi. Le altre impostazioni possono essere inserite in un secondo momento. Premendo il pulsante `Save Settings` in fondo alla pagina il Sonoff viene riavviato, quindi cerca di connettersi alla rete impostata.

L'avvenuta connessione è indicata dall'accensione del led integrato, nel caso in cui non si accenda il segnale potrebbe essere troppo debole, per riavviare il processo di connessione tenere premuto il pulsante fino ai 2 blink del led.

Ora il dispositivo è connesso alla rete locale, ed è possibile inserire i parametri MQTT tramite la pagina web usata poco fà, inserendo nel browser `NOMEDISPOSITIVO.local`, anche se ancora disponibile non è più necessario collegarsi alla rete generata dal dispositivo stesso.

Inserendo il ProductKey di OTA Drive sarà possibile gestire tramite questa piattaforma l'aggiornamento dei futuri firmware senza flashare il dispositivo fisicamente.

<p align="center">
  <img src="https://github.com/aletiffi/Led_Sonoff/blob/main/img/Settings.PNG" alt="Settings"/>
</p>