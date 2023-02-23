// Import required libraries
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#else
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "Kanata";
const char* password = "1234567890";
const char* http_username = "admin";
const char* http_password = "admin";


String sliderValue = "10";
// setting PWM properties

const char* PARAM = "value1";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
   <head>
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@4.3.1/dist/css/bootstrap.min.css" integrity="sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T" crossorigin="anonymous">
      <title>Shuttlecock Collector</title>
      <style>
         html {font-family: Arial; display: inline-block; text-align: center; }
         h2 {font-size: 2.3rem;}
         p {font-size: 1rem;
         margin-bottom: 0.5rem;
         }
         body {max-width: 400px; margin:auto; padding-bottom: 25px;}
         .slider { -webkit-appearance: none; 
         margin-left: 80px ; width: 235px; height: 25px; background: #01105C;background-image: linear-gradient(to right, lightblue,lightgreen,orange, red);
         outline: none; -webkit-transition: .2s; transition: opacity .2s;border-radius:2px;}
         .slider::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; 
         width: 35px; height: 35px;
         background-color:blue;
         cursor: pointer;border-radius:5px;}
         .slider::-moz-range-thumb { width: 35px; height: 35px; background: #103249; cursor: pointer; } 
         .grid {
         display: table;
         border-spacing: 5px;
         //text-align:center;
         margin-left: 76px;
         }
         .row {
         display: table-row
         }
         .cell {
         width: 80px;
         height: 80px;
         display: table-cell;
         line-height:80px;
         }
         .batteryContainer {
         display: flex;
         flex-direction: row;
         align-items: center;
         height:60px;
         width:70%;
         }
         .batterybody {
         border-radius: 3px;
         border: 2px solid #444;
         padding: 1px ;
         width:150px;
         height: 50px;
         position:relative;
         /*margin-left:auto;*/
         left:118px;
         }
         .batteryHead {
         border-radius: 0px 2px 2px 0px;
         background-color: #444;
         margin: 0px;
         width: 10px;
         height: 26px;
         position:relative;
         left:118px;
         /*top:0px;*/
         }
         .batteryLevel {
         border-radius: 1px;
         background-color: #73AD21;
         width: 0%;
         height:100%;
         }
         .batteryText{
         padding:11px;
         text-align:center;
         }
         .sliderstart {
         position: relative;
         left:81px;
         top:-6px;
         font-weight: bold;
         }
         .sliderend {
         position: relative;
         left:-2px;
         top:-6px;
         font-weight: bold;  
         }
         .switch input
         {
         display: none;
         } 
         .switch 
         {
         display: inline-block;
         width: 120px; /*=w*/
         height: 50px; /*=h*/
         margin-left: 137px;
         position: relative;
         }
         .slider1
         {
         position: absolute;
         top: 0;
         bottom: 0;
         left: 0;
         right: 0;
         border-radius: 30px;
         box-shadow: 0 0 0 2px #777, 0 0 4px #777;
         cursor: pointer;
         border: 4px solid transparent;
         // overflow: hidden;
         transition: 0.2s;
         }
         .slider1:before
         {
         position: absolute;
         content: "";
         width: 40%;
         height: 42px;
         background-color: #777;
         border-radius: 30px;
         transform: translateX(0px); 
         transition: 0.2s;
         }
         input:not(:checked) + .slider1:before{
         background-color: gray;
         }
         input:not(:checked) + .slider1
         {
         // box-shadow: 0 0 0 2px red, 0 0 8px red;
         }
         input:checked + .slider1:before
         {
         transform: translateX(67px); 
         background-color: limeGreen;
         }
         input:checked + .slider1
         {
         box-shadow: 0 0 0 2px limeGreen, 0 0 8px limeGreen;
         }
         .logoutButton
         {
         margin-left:-20px;
         background-color: lightblue;
         }
      </style>
      <script src="https://code.jquery.com/jquery-3.3.1.slim.min.js" integrity="sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo" crossorigin="anonymous"></script>
      <script src="https://cdn.jsdelivr.net/npm/popper.js@1.14.7/dist/umd/popper.min.js" integrity="sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1" crossorigin="anonymous"></script>
      <script src="https://cdn.jsdelivr.net/npm/bootstrap@4.3.1/dist/js/bootstrap.min.js" integrity="sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM" crossorigin="anonymous"></script>
   </head>
   <body>
      <h4 style="text-align:center" >Shuttlecock Collector</h4>
      <p style="text-align:center"><b>Battery Level</b>
      <div>
         <div class="batteryContainer">
            <div class="batterybody">
               <div class="batteryLevel">
                  <div class="batteryText" id="batteryText">
                     %LEVEL%
                  </div>
               </div>
            </div>
            <div class="batteryHead"></div>
         </div>
      </div>
      </p>
      <p style="text-align:center"> <b>Pickup Motor Relay</b></p>
      <label class="switch">
      <span style="font-weight:bold; color:limeGreen; margin: 11px 0 0 15px; position:absolute;">                                                                        
      ON</span><span style="font-weight:bold; color:gray; margin: 11px 0 0 76px; position:absolute;"> OFF
      </span>
      <input type="checkbox" onclick="relay(this)" id="checkbox">
      <span class="slider1"></span>
      </label>
      <p>
      <p style="text-align:center" ><b>Drive Motor Speed:<span id="textSliderValue">%SLIDERVALUE%</span></b></p>
      <p style="margin-bottom:0px;">
         <span class ="sliderstart">1</span>
         <input type="range" name="RangeInput" onchange="updateSlider(this)" id="speedSlider" min="1" max="10" value="%SLIDERVALUE%" step="1" class="slider"> 
         <span class ="sliderend">10</span>
      </p>
      <p style="text-align:center"><b>Drive Motor Direction</b></p>
      <div class="grid" >
      <div class="row">
         <div class="cell"></div>
         <div class="cell">
            <button type="button" class="btn btn-secondary btn-block" id="up" onclick ="motorControl(this)" style="width: 50px; height: 50px">
               <svg xmlns="http://www.w3.org/2000/svg" fill="currentColor" class="bi bi-arrow-up-circle" viewBox="0 0 16 16">
                  <path fill-rule="evenodd" d="M1 8a7 7 0 1 0 14 0A7 7 0 0 0 1 8zm15 0A8 8 0 1 1 0 8a8 8 0 0 1 16 0zm-7.5 3.5a.5.5 0 0 1-1 0V5.707L5.354 7.854a.5.5 0 1 1-.708-.708l3-3a.5.5 0 0 1 .708 0l3 3a.5.5 0 0 1-.708.708L8.5 5.707V11.5z"></path>
               </svg>
            </button>
         </div>
         <div class="cell"></div>
      </div>
      <div class="row">
         <div class="cell">
            <button type="button" class="btn btn-secondary btn-block" id = "left" onclick="motorControl(this)" style="width: 50px; height: 50px">
               <svg xmlns="http://www.w3.org/2000/svg" fill="currentColor" class="bi bi-arrow-left-circle" viewBox="0 0 16 16">
                  <path fill-rule="evenodd" d="M1 8a7 7 0 1 0 14 0A7 7 0 0 0 1 8zm15 0A8 8 0 1 1 0 8a8 8 0 0 1 16 0zm-4.5-.5a.5.5 0 0 1 0 1H5.707l2.147 2.146a.5.5 0 0 1-.708.708l-3-3a.5.5 0 0 1 0-.708l3-3a.5.5 0 1 1 .708.708L5.707 7.5H11.5z"></path>
               </svg>
            </button>
         </div>
         <div class="cell">
            <button type="button" class="btn btn-light btn-block " id = "brake" onclick="motorControl(this)" style="width: 50px; height: 50px; color:red; background-color: #070707;">
               <svg xmlns="http://www.w3.org/2000/svg" fill="currentColor" class="bi bi-sign-stop " viewBox="0 0 16 16" >
                  <path d="M3.16 10.08c-.931 0-1.447-.493-1.494-1.132h.653c.065.346.396.583.891.583.524 0 .83-.246.83-.62 0-.303-.203-.467-.637-.572l-.656-.164c-.61-.147-.978-.51-.978-1.078 0-.706.597-1.184 1.444-1.184.853 0 1.386.475 1.436 1.087h-.645c-.064-.32-.352-.542-.797-.542-.472 0-.77.246-.77.6 0 .261.196.437.553.522l.654.161c.673.164 1.06.487 1.06 1.11 0 .736-.574 1.228-1.544 1.228Zm3.427-3.51V10h-.665V6.57H4.753V6h3.006v.568H6.587Z"></path>
                  <path fill-rule="evenodd" d="M11.045 7.73v.544c0 1.131-.636 1.805-1.661 1.805-1.026 0-1.664-.674-1.664-1.805V7.73c0-1.136.638-1.807 1.664-1.807 1.025 0 1.66.674 1.66 1.807Zm-.674.547v-.553c0-.827-.422-1.234-.987-1.234-.572 0-.99.407-.99 1.234v.553c0 .83.418 1.237.99 1.237.565 0 .987-.408.987-1.237Zm1.15-2.276h1.535c.82 0 1.316.55 1.316 1.292 0 .747-.501 1.289-1.321 1.289h-.865V10h-.665V6.001Zm1.436 2.036c.463 0 .735-.272.735-.744s-.272-.741-.735-.741h-.774v1.485h.774Z"></path>
                  <path fill-rule="evenodd" d="M4.893 0a.5.5 0 0 0-.353.146L.146 4.54A.5.5 0 0 0 0 4.893v6.214a.5.5 0 0 0 .146.353l4.394 4.394a.5.5 0 0 0 .353.146h6.214a.5.5 0 0 0 .353-.146l4.394-4.394a.5.5 0 0 0 .146-.353V4.893a.5.5 0 0 0-.146-.353L11.46.146A.5.5 0 0 0 11.107 0H4.893ZM1 5.1 5.1 1h5.8L15 5.1v5.8L10.9 15H5.1L1 10.9V5.1Z"></path>
               </svg>
            </button>
         </div>
         <div class="cell">
            <button type="button" class="btn btn-secondary btn-block" id = "right" onclick="motorControl(this)" style="width: 50px; height: 50px">
               <svg xmlns="http://www.w3.org/2000/svg" fill="currentColor" class="bi bi-arrow-right-circle" viewBox="0 0 16 16">
                  <path fill-rule="evenodd" d="M1 8a7 7 0 1 0 14 0A7 7 0 0 0 1 8zm15 0A8 8 0 1 1 0 8a8 8 0 0 1 16 0zM4.5 7.5a.5.5 0 0 0 0 1h5.793l-2.147 2.146a.5.5 0 0 0 .708.708l3-3a.5.5 0 0 0 0-.708l-3-3a.5.5 0 1 0-.708.708L10.293 7.5H4.5z"></path>
               </svg>
            </button>
         </div>
      </div>
      <div class="row">
         <div class="cell"></div>
         <div class="cell">
            <button type="button" class="btn btn-secondary btn-block" id="down" onclick="motorControl(this)" style="width: 50px; height: 50px">
               <svg xmlns="http://www.w3.org/2000/svg" fill="currentColor" class="bi bi-arrow-down-circle" viewBox="0 0 16 16">
                  <path fill-rule="evenodd" d="M1 8a7 7 0 1 0 14 0A7 7 0 0 0 1 8zm15 0A8 8 0 1 1 0 8a8 8 0 0 1 16 0zM8.5 4.5a.5.5 0 0 0-1 0v5.793L5.354 8.146a.5.5 0 1 0-.708.708l3 3a.5.5 0 0 0 .708 0l3-3a.5.5 0 0 0-.708-.708L8.5 10.293V4.5z"></path>
               </svg>
            </button>
         </div>
         <div class="cell"></div>
      </div>
      <button class ="logoutButton" onclick="logoutButton()" >Logout</button>
   </body>
   <script>
   "use strict";
      function logoutButton() {
         var xhr = new XMLHttpRequest();
         xhr.open("GET", "/logout", true);
         xhr.send();
         // setTimeout(function(){ window.open("/logged-out","_self"); }, 1000);
      }
      $(document).ready(function(){
         var level=$.trim($(".batteryLevel").text());
         $(".batteryLevel").css("width", level);
        })
         function updateSlider(element) {
           var sliderValue = document.getElementById("speedSlider").value;
           var xmlHttp = new XMLHttpRequest();
           document.getElementById("textSliderValue").innerHTML = sliderValue;
           xmlHttp.open("GET", "/slider?value1="+sliderValue, true)
           xmlHttp.send();
          //  const Slider = document.querySelector('input[name=RangeInput]')
          //  Slider.oninput =_=> Slider.style.setProperty('--SliderColor', `hsl(${Slider.value}, 100%, 50%)`)
           }
          function motorControl(element) {
           var xmlHttp = new XMLHttpRequest();
           xmlHttp.open("GET", "/control?value1="+element.id, true);
           xmlHttp.send();
           xmlHttp.onload = function() {
            let  res = xmlHttp.response;
            if (res != "OK" ){
             var level0=parseInt(res);
             if (level0<30){
               $(".batteryLevel").delay(1000).css("background-color","red");
             }else if(level0 <60){
               $(".batteryLevel").delay(1000).css("background-color","yellow");  
             }
             var level1 = String(level0 + "%");
             $(".batteryText").text(level1);
            }
           }
          }
         function relay(element) {
           var xmlHttp = new XMLHttpRequest();
           var checkbox=document.getElementById("checkbox");
           if (checkbox.checked == true ){
                checkvalue="checked";
           } else {
                checkvalue="unchecked";
           }
           xmlHttp.open("GET", "/relay?value1="+checkvalue, true);
           xmlHttp.send();
          }
      
           
         
         
         
   </script>
   )rawliteral";
   const char logout_html[] PROGMEM = R"rawliteral(
   <!DOCTYPE HTML>
   <html>
      <head>
         <meta name="viewport" content="width=device-width, initial-scale=1">
      </head>
      <body>
         <p>Logged out or <a href="/">return to homepage</a>.</p>
         <p><strong>Note:</strong> close all web browser tabs to complete the logout process.</p>
      </body>
   </html>
)rawliteral";

String readBatteryLevel(){
  
  int value= round(((3.3*analogRead(A0))-512)*100/512);
  if (value <0) {value=0;}
  if (value >100) {value=100;}
  return String(value);   
}
// Replaces placeholder with button section in your web page
String processor(const String& var) {
  //Serial.println(var);
  if (var == "SLIDERVALUE") {
    return sliderValue;
  }
 if (var == "LEVEL") {
   return readBatteryLevel()+"%";
 }
  return String();
}
void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(D0, OUTPUT);
  digitalWrite(D0,LOW);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);

  // configure motor PWM 
  analogWriteRange(1024);
  analogWriteFreq(1000);
  


  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    // delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/logout", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(401); 
  });
//   server.on("/logged-out", HTTP_GET, [](AsyncWebServerRequest *request){
//     request->send_P(200, "text/html", logout_html, processor);
//   });
 
  // Send a GET request to <ESP_IP>/slider?value=<inputMessage>
  server.on("/slider", HTTP_GET, [](AsyncWebServerRequest* request) {
   if(!request->authenticate(http_username, http_password)) return request->requestAuthentication();
    String inputMessage1;
    if (request->hasParam(PARAM)) {
      inputMessage1 = request->getParam(PARAM)->value();
      sliderValue = inputMessage1;
      int pwmValue = 300+(sliderValue.toInt()-1)*724/9;
      Serial.print("pwmValue=");
      Serial.println(pwmValue);
      analogWrite(D1, pwmValue);
      analogWrite(D6, pwmValue);
      request->send(200, "text/plain", "OK");
    }else {
      request->send(200, "text/plain", "No Message Sent");
    }
  });
  server.on("/relay", HTTP_GET, [](AsyncWebServerRequest* request) {
    if(!request->authenticate(http_username, http_password)) return request->requestAuthentication();
    String inputMessage1;
    if (request->hasParam(PARAM)) {
      inputMessage1 = request->getParam(PARAM)->value();
      if (inputMessage1=="checked") {
        digitalWrite(D0,HIGH);
      } 
      if (inputMessage1=="unchecked") {
        digitalWrite(D0,LOW);
      } 
      request->send(200, "text/plain", "OK");
    } else {
      request->send(200, "text/plain", "No Message Sent");
    }
  });
  server.on("/control", HTTP_GET, [](AsyncWebServerRequest* request) {
    if(!request->authenticate(http_username, http_password)) return request->requestAuthentication();
    String inputMessage1;
    if (request->hasParam(PARAM)) {
      inputMessage1 = request->getParam(PARAM)->value();
      Serial.println(inputMessage1);
      //Channel B: ENB=D1(PWM), IN3=D3, IN4=D2, 
      //Channel A: ENA=D6(PWM), IN1=D5, IN2=D4  

      //  Left motor           right motor
      //  (OUT3,OUT4)          (OUT1,OUT2)
      //  (D4,D5)              (D1,D2)      ---control
      // (D5,D4)                (D3,D2)
      //  (1,0)                (1,0)        -- up(forward)
      //  (0,1)                (0,1)        -- down(backward)
      //  (0,1)  (down)        (1,0) (up)   -- left
      //  (1,0)  (up)          (0,1) (down) -- right
      if (inputMessage1 =="brake"){
        // analogWrite(D3, 0);
        // analogWrite(D6, 0);
        digitalWrite(D5, LOW);  //left motor off
        digitalWrite(D4, LOW);
        digitalWrite(D3, LOW);  //right motor off
        digitalWrite(D2, LOW);
      }else if (inputMessage1 == "left") {
        digitalWrite(D5, HIGH);  //left motor forward
        digitalWrite(D4, LOW);
        digitalWrite(D3, LOW);  //right motor reverse
        digitalWrite(D2, HIGH);
        // Serial.println("left:D1=0,D2=0; D4=1,D5=0");
       }else if (inputMessage1 == "right") {
      
        digitalWrite(D5, LOW);  //left motor reverse
        digitalWrite(D4, HIGH);
        digitalWrite(D3, HIGH);  //right motor forward
        digitalWrite(D2, LOW);
        //Serial.println("right:D1=1,D2=0; D4=0,D5=0");
       } else if (inputMessage1 == "up") {
       
        digitalWrite(D5, HIGH);  //left motor forward
        digitalWrite(D4, LOW);
        digitalWrite(D3, HIGH);  //right motor forward
        digitalWrite(D2, LOW);
        // Serial.println("up:D1=1,D2=0; D4=1,D5=0");
       } else if (inputMessage1 == "down") {
       
        digitalWrite(D5, LOW);  //left motor backward
        digitalWrite(D4, HIGH);
        digitalWrite(D3, LOW);  //right motor backward
        digitalWrite(D2, HIGH);
        // Serial.println("down:D1=0,D2=1; D4=0,D5=1");
       }
       if (inputMessage1 == "brake") { 
          request->send(200, "text/plain", readBatteryLevel());
        
       }else if (inputMessage1 == "up"||inputMessage1 == "down"||inputMessage1 == "left"||inputMessage1 == "right"){
          request->send(200, "text/plain", "OK");

       } 
       else {
          request->send(200, "text/plain", "No message sent");
       } 

    
    }
  });  

  server.onNotFound(notFound);
  // Start server
  server.begin();
}


void loop() {
}
