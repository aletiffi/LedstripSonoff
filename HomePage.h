const char webPage[] PROGMEM = R"=====(
  <!DOCTYPE html>
  <html lang="en">
  
    <head>
      <meta charset="utf-8">
      <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=yes">
      <title>Home Page</title>
      <style>
        body {
          background-color: #b7b7ee;
        }
        div {
          padding: 10px;
        }
        input {
          border-radius: 5px;
          width: 100%;
          height: 40px;
          box-sizing: border-box;
        }
        select {
          border-radius: 5px;
          padding: 10px;
          width: 100%;
        }
        body {
          text-align: center;
          font-family: verdana;
        }
        button {
          border-radius: 5px;
          width: 100%;
          height: 40px;
          color:black;
        }
        .container {
          border-radius: 5px;
          border: 2px solid #000000;
          width: 90%;
          max-width: 450px;
          background-color: rgb(62, 131, 131);
          margin: auto;
        }
        .actions-container {
          width: 90%;
          max-width: 350px;
          margin: auto;
        }
        .settings-container {
          width: 90%;
          max-width: 350px;
          margin: auto;
        }
      </style>
    </head>
    <body>
      <div class="container">
        <div class="actions-container">
          <h1>Home Page</h1>
          <hr>
          <p>
            <a href="/on"><button style="background-color: rgb(14, 121, 14);">ON</button></a>
          </p>
          <p>
            <a href="/off"><button style="background-color: rgb(209, 16, 16);">OFF</button></a>
          </p>
          <hr>
          <h2>Color picker</h2>
          <input class="input" style="padding: 0%; height: 10ch;" type="color" id="colorWell" name="colorWell" value="#a366cc">
          <hr>
          <h2>Brigthness</h2>
          <input type="range" value="" id="brigthness" min="0" max="255" onchange="get_brigthness()">
          <hr>
          <h2>Effect Selector</h2>
          <select id="effect" onchange="get_effect()">
            <option value="Solid">Solid</option> 
            <option value="Random">Random</option>
          </select>
          <hr>
        </div>
        <div class="settings-container">
          <h2>Settings</h2>
          <p>
            <label for="Hostname">Hostname</label>
          </p>
          <p>
            <input value="" id="Hostname" placeholder="     DeviceName">
          </p>
          <p>  
            <label for="Ssid">Ssid</label>
          </p>
          <p>
            <input value="" id="Ssid" placeholder="     Wifi Net Name">
          </p>
          <p>
            <label for="Password">Password</label>
          </p>
          <p>
            <input type="password" value="" id="Password" placeholder="     Wifi Password">
          </p>
          <p>  
            <label for="MQTT_Sub">MQTT Subscribe topic</label>
          </p>
          <p>
            <input value="" id="MQTT_Sub" placeholder="     cmnd/MyDevice">
          </p>
          <p>  
            <label for="MQTT_Pub">MQTT Publish topic</label>
          </p>
          <p>
            <input value="" id="MQTT_Pub" placeholder="     stat/MyDevice">
          </p>
          <p>  
            <label for="MQTT_Server">MQTT Server</label>
          </p>
          <p>
            <input value="" id="MQTT_Server" placeholder="     192.168.1.100">
          </p>
          <p>
            <label for="MQTT_User">MQTT User</label>
          </p>
          <p>
            <input value="" id="MQTT_User" placeholder="     MQTT User">
          </p>
          <p>  
            <label for="MQTT_Password">MQTT Password</label>
          </p>
          <p>
            <input type="password" value="" id="MQTT_Password" placeholder="     MQTT Password">
          </p>
          <p>  
            <label for="OTA_DRIVE_KEY">Ota Drive Product Key</label>
          </p>
          <p>
            <input value="" id="OTA_DRIVE_KEY" placeholder="     xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx">
          </p>
          <br>
          <p>
            <button type="button" id="savebtn" style="background-color: cornflowerblue;" onclick="save()">Save</button>
          </p>
          <hr>
          <p>
            <a href="/restart"><button style="background-color: rgb(170, 20, 216);">Restart</button></a>
          </p>
          <hr>
        </div>
      </div>
    </body>
  
    <script>
      var colorWell;
      var defaultColor = "#3289a8";
  
      window.addEventListener("load", startup, false);
  
      function startup() {
        colorWell = document.querySelector("#colorWell");
        colorWell.value = defaultColor;
        colorWell.addEventListener("change", changeColor, false);
        colorWell.select();
      }

      function get_brigthness(){
        var brigthness = document.getElementById("brigthness").value;
        var data = {brigthness:brigthness};
  
        var xhr = new XMLHttpRequest();
        var url = "/brigthness";
    
        xhr.onreadystatechange = function () {
          if (this.readyState == 4 && this.status == 200) {
            if (xhr.responseText != null) {
              console.log(xhr.responseText);
            }
          }
        };
        xhr.open("POST", url, true);
        xhr.send(JSON.stringify(data));        
      }

      function get_effect(){
        var effect = document.getElementById("effect").value;
        var data = {effect:effect};
  
        var xhr = new XMLHttpRequest();
        var url = "/effect";
    
        xhr.onreadystatechange = function () {
          if (this.readyState == 4 && this.status == 200) {
            if (xhr.responseText != null) {
              console.log(xhr.responseText);
            }
          }
        };
        xhr.open("POST", url, true);
        xhr.send(JSON.stringify(data));
      }

      function changeColor(event) {
        var color = event.target.value;
        var data = {color:color};
  
        var xhr = new XMLHttpRequest();
        var url = "/color";
    
        xhr.onreadystatechange = function () {
          if (this.readyState == 4 && this.status == 200) {
            if (xhr.responseText != null) {
              console.log(xhr.responseText);
            }
          }
        };
        xhr.open("POST", url, true);
        xhr.send(JSON.stringify(data));
      }
      
      function save() {
        var Hostname = document.getElementById("Hostname").value;
        var Ssid = document.getElementById("Ssid").value;
        var Password = document.getElementById("Password").value;
        var MQTT_Sub = document.getElementById("MQTT_Sub").value;
        var MQTT_Pub = document.getElementById("MQTT_Pub").value;
        var MQTT_Server = document.getElementById("MQTT_Server").value;
        var MQTT_User = document.getElementById("MQTT_User").value;
        var MQTT_Password = document.getElementById("MQTT_Password").value;
        var OTA_DRIVE_KEY = document.getElementById("OTA_DRIVE_KEY").value;
  
        var data = {Hostname:Hostname,
                    Ssid:Ssid,
                    Password:Password,
                    MQTT_Sub:MQTT_Sub,
                    MQTT_Pub:MQTT_Pub,
                    MQTT_Server:MQTT_Server,
                    MQTT_User:MQTT_User,
                    MQTT_Password:MQTT_Password,
                    OTA_DRIVE_KEY:OTA_DRIVE_KEY};
  
        var xhr = new XMLHttpRequest();
        var url = "/settings";
    
        xhr.onreadystatechange = function () {
          if (this.readyState == 4 && this.status == 200) {
            if (xhr.responseText != null) {
              console.log(xhr.responseText);
            }
          }
        };
        xhr.open("POST", url, true);
        xhr.send(JSON.stringify(data));
      };
    </script>
  
  </html>
)=====";
