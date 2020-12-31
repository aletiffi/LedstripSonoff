const char webPage[] PROGMEM = R"=====(
  <!DOCTYPE html>
  <html lang="en">
  
    <head>
      <meta charset="utf-8">
      <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
      <title>Home Page</title>
      <style>
        div {
          padding: 10px;
        }
    
        input {
          padding: 10px;
          width: 270px;
        }

        select {
          padding: 10px;
          width: 270px;
        }
    
        body {
          text-align: center;
          font-family: verdana;
        }
    
        button {
          padding: 5px;
          width: 300px;
        }
  
        .container {
          width: 350px;
          background-color: rgb(62, 131, 131);
          margin: auto;
        }
    
      </style>
    </head>
  
    <body>
      <div class="container">
        <h1>Home Page</h1>
        <hr>
        <p>
          <a href="/on"><button style="color:black; background-color: rgb(14, 121, 14);">ON</button></a>
        </p>
        <p>
          <a href="/off"><button style="color:black; background-color: rgb(209, 16, 16);">OFF</button></a>
        </p>
        <p>
          <a href="/restart"><button style="color:black; background-color: rgb(170, 20, 216);">Restart</button></a>
        </p>
        <hr>
        <h2>Color picker</h2>
        <input style="padding: 0%; width: 85%; height: 10ch;" type="color" id="colorWell" name="colorWell" value="#a366cc">
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
        <h2>Settings</h2>
        <p>
          <label for="Hostname">Hostname</label>
        </p>
        <p>
          <input value="" id="Hostname" placeholder="DeviceName">
        </p>
        <p>  
          <label for="Ssid">Ssid</label>
        </p>
        <p>
          <input value="" id="Ssid" placeholder="Wifi Net to connect">
        </p>
        <p>
          <label for="Password">Password</label>
        </p>
        <p>
          <input type="password" value="" id="Password" placeholder="Wifi Password">
        </p>
        <p>  
          <label for="Mqtt_Sub">MQTT Subscribe topic</label>
        </p>
        <p>
          <input value="" id="Mqtt_Sub" placeholder="cmnd/MyDevice">
        </p>
        <p>  
          <label for="Mqtt_Pub">MQTT Publish topic</label>
        </p>
        <p>
          <input value="" id="Mqtt_Pub" placeholder="stat/MyDevice">
        </p>
        <p>  
          <label for="MQTT_Server">MQTT Server</label>
        </p>
        <p>
          <input value="" id="MQTT_Server" placeholder="192.168.1.100">
        </p>
        <p>
          <label for="MQTT_User">MQTT User</label>
        </p>
        <p>
          <input value="" id="MQTT_User" placeholder="MQTT User">
        </p>
        <p>  
          <label for="MQTT_Password">MQTT Password</label>
        </p>
        <p>
          <input type="password" value="" id="MQTT_Password" placeholder="MQTT Password">
        </p>
        <br>
        <p>
          <button type="button" id="savebtn" style="color:black; background-color: cornflowerblue;" onclick="save()">Save</button>
        </p>
        <hr>
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
        var Mqtt_Sub = document.getElementById("Mqtt_Sub").value;
        var Mqtt_Pub = document.getElementById("Mqtt_Pub").value;
        var MQTT_Server = document.getElementById("MQTT_Server").value;
        var MQTT_User = document.getElementById("MQTT_User").value;
        var MQTT_Password = document.getElementById("MQTT_Password").value;
  
        var data = {Hostname:Hostname,
                    Ssid:Ssid,
                    Password:Password,
                    Mqtt_Sub:Mqtt_Sub,
                    Mqtt_Pub:Mqtt_Pub,
                    MQTT_Server:MQTT_Server,
                    MQTT_User:MQTT_User,
                    MQTT_Password:MQTT_Password};
  
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
