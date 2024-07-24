#include <ESP32Servo.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

Servo myservo;  // create servo object to control a servo
const char* ssid = "Quantanics";
const char* password = "Qu@nt@nics18";
const char* correctPassword = "1234"; // set your desired password

int pos = 0;    // variable to store the servo position
int servoPin = 23;
AsyncWebServer server(80);

String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>IoT Garage System</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      background-color: #f4f4f9;
      margin: 0;
      padding: 0;
      text-align: center;
    }
    .container {
      margin-top: 50px;
      background-color: #ffffff;
      border-radius: 10px;
      box-shadow: 0 0 20px rgba(0,0,0,0.1);
      display: inline-block;
      padding: 20px;
    }
    h2 {
      color: #333333;
    }
    input[type=password], button {
      font-size: 18px;
      padding: 10px;
      margin: 10px;
      border: none;
      border-radius: 5px;
    }
    input[type=password] {
      width: 80%;
      max-width: 300px;
      background-color: #eeeeee;
    }
    button {
      background-color: #5cb85c;
      color: #ffffff;
      cursor: pointer;
      transition: background-color 0.3s;
    }
    button:hover {
      background-color: #4cae4c;
    }
    .hidden {
      display: none;
    }
    .control-buttons {
      display: flex;
      justify-content: center;
    }
    .control-buttons button {
      margin: 0 10px;
    }
    footer {
      margin-top: 50px;
      color: #888888;
    }
  </style>
</head>
<body>
  <div class="container">
    <h2>IoT Garage System</h2>
    <p>Please enter the password to access the controls:</p>
    <input type="password" id="password" placeholder="Enter password">
    <button onclick="checkPassword()">Submit</button>
    <div id="control" class="hidden">
      <h2>Control Garage Door</h2>
      <div class="control-buttons">
        <button onclick="controlServo('open')">Open</button>
        <button onclick="controlServo('close')">Close</button>
      </div>
    </div>
  </div>
  <footer>
    <p>IoT Garage System &copy; 2024</p>
  </footer>
  <script>
    function checkPassword() {
      var inputPassword = document.getElementById("password").value;
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/check?password=" + inputPassword, true);
      xhr.onreadystatechange = function () {
        if (xhr.readyState == 4 && xhr.status == 200) {
          if (xhr.responseText == "ok") {
            document.getElementById("control").classList.remove("hidden");
          } else {
            alert("Incorrect Password");
          }
        }
      };
      xhr.send();
    }
    function controlServo(action) {
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/servo?action=" + action, true);
      xhr.send();
    }
  </script>
</body>
</html>
)rawliteral";

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize Servo
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(servoPin, 1000, 2000); // attaches the servo on pin 23 to the servo object

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // Serve the HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", html);
  });

  // Handle password check
  server.on("/check", HTTP_GET, [](AsyncWebServerRequest *request){
    String inputPassword;
    if (request->hasParam("password")) {
      inputPassword = request->getParam("password")->value();
    }
    if (inputPassword == correctPassword) {
      request->send(200, "text/plain", "ok");
    } else {
      request->send(200, "text/plain", "fail");
    }
  });

  // Handle servo control
  server.on("/servo", HTTP_GET, [](AsyncWebServerRequest *request){
    String action;
    if (request->hasParam("action")) {
      action = request->getParam("action")->value();
    }
    if (action == "open") {
      for (pos = 0; pos <= 180; pos += 1) { 
        myservo.write(pos);
        delay(15);
      }
    } else if (action == "close") {
      for (pos = 180; pos >= 0; pos -= 1) { 
        myservo.write(pos);
        delay(15);
      }
    }
    request->send(200, "text/plain", "done");
  });

  // Start server
  server.begin();
}

void loop() {
  // Nothing needed here
}
