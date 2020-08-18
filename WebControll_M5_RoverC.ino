/*
 * Copyright (c) 2015, Majenko Technologies
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 * * Neither the name of Majenko Technologies nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
 #include <M5StickC.h>

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "RoverC.h"

/* Set these to your desired credentials. */
const char *ssid = "RoverC";
const char *password = "";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

RoverC rover_c(Wire);

/* set I2C library*/
#include <Wire.h>

unsigned int g_client_id = -1;

String form ="<!DOCTYPE html>\r\n"
"<html lang='ja' xml:lang='ja'>\r\n"
"<head>\r\n"
"  <meta charset='utf-8'>\r\n"
"  <meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1'>\r\n"
"  <title>SimpleWifiControl</title>\r\n"
"\r\n"
"  <style type='text/css'>\r\n"
"\r\n"
"    .parent {\r\n"
"      display: -webkit-box;\r\n"
"      display: -ms-flexbox;\r\n"
"      display: flex;\r\n"
"      -ms-flex-wrap: wrap;\r\n"
"      flex-wrap: wrap;\r\n"
"      -ms-flex-pack: distribute;\r\n"
"      justify-content: space-around;\r\n"
"      -webkit-box-align: center;\r\n"
"      -ms-flex-align: center;\r\n"
"      align-items: center;\r\n"
"    }\r\n"
"\r\n"
"    .child {\r\n"
"      color: #fff;\r\n"
"      text-align: center;\r\n"
"      background-color: #fff;\r\n"
"      margin: 10px;\r\n"
"    }\r\n"
"\r\n"
"    .operationpnl {\r\n"
"      line-height: 30px;\r\n"
"      height: 120px;\r\n"
"    }\r\n"
"\r\n"
"    .child04 {\r\n"
"      line-height: 70px;\r\n"
"      height: 70px;\r\n"
"    }\r\n"
"\r\n"
"    .controlpnl {\r\n"
"      line-height: 50px;\r\n"
"      width: 300px;\r\n"
"      height: 200px;\r\n"
"    }\r\n"
"    input.checkboxyz{\r\n"
"      opacity: 0;\r\n"
"    }\r\n"
"\r\n"
"    .power-icon{\r\n"
"        width: 200px;\r\n"
"        height: 200px;\r\n"
"        fill: white;\r\n"
"        stroke: black;\r\n"
"    }\r\n"
"\r\n"
"    #poweron:checked + label .power-icon{\r\n"
"        fill: white;\r\n"
"        stroke: red;\r\n"
"    }\r\n"
"\r\n"
"    .button {\r\n"
"      display       : inline-block;\r\n"
"      border-radius : 5%;\r\n"
"      font-size     : 10pt;\r\n"
"      text-align    : center;\r\n"
"      cursor        : pointer;\r\n"
"      padding       : 12px 12px;\r\n"
"      background    : #000066;\r\n"
"      color         : #ffffff;\r\n"
"      line-height   : 1em;\r\n"
"      transition    : .3s;\r\n"
"      box-shadow    : none;\r\n"
"      border        : 2px solid #000066;\r\n"
"      margin        : 5px;\r\n"
"      width         : 70px;\r\n"
"      height        : 50px;\r\n"
"    }\r\n"
"    .button:hover {\r\n"
"      box-shadow    : none;\r\n"
"      color         : #3c3cfa;\r\n"
"      background    : #ffffff;\r\n"
"    }\r\n"
"\r\n"
"    div.controlpnl {font-size:16pt;color:red;text-align:center;width:300px;}\r\n"
"    /*div.disp_count {font-size:16pt;color:red;margin: 0px}*/\r\n"
"\r\n"
"    .indicator {\r\n"
"      width : 14px;\r\n"
"      height : 14px;\r\n"
"      background : red;\r\n"
"      border        : 1px solid #fff;\r\n"
"      border-radius: 7px 7px 7px 7px; \r\n"
"      float : left;\r\n"
"      margin : 2px;\r\n"
"    }\r\n"
"\r\n"
"    .led-icon{\r\n"
"        width: 14px;\r\n"
"        height: 14px;\r\n"
"        fill: #770000;\r\n"
"        stroke: black;\r\n"
"    }\r\n"
"\r\n"
"  </style>\r\n"
"\r\n"
"  <script type = 'text/javascript'>\r\n"
"    var count_value = 0;\r\n"
"    var ws = new WebSocket('ws://192.168.4.1/ws');\r\n"
"\r\n"
"    ws.onopen = function() {\r\n"
"      window.alert('Connected');\r\n"
"    };\r\n"
"\r\n"
"    ws.onmessage = function(evt) {\r\n"
"      var remain_sec = Number(evt.data);\r\n"
"      if(remain_sec == -1)\r\n"
"      {\r\n"
"        document.getElementById('display').innerHTML  = 'Connected';\r\n"
"      }\r\n"
"    };\r\n"
"  </script>\r\n"
"</head>\r\n"
"<body>      \r\n"
"  <p id = 'display'>Not connected</p>\r\n"
"\r\n"
"    <div class='child controlpnl'>\r\n"
"      <form method='post' id='control_form' action='./control' target='tif3' >\r\n"
"        <iframe src='javascript: false;' style='display: none;' name='tif3' id='tif3'></iframe>\r\n"
"        <div class='controlpnl' >\r\n"
"          <input type='submit' name='dir' value='lforward' class='button' >\r\n"
"          <input type='submit' name='dir' value='forward' class='button' >\r\n"
"          <input type='submit' name='dir' value='rforward' class='button' ><br/>\r\n"
"          <input type='submit' name='dir' value='left' class='button' >\r\n"
"          <input type='submit' name='dir' value='stop' class='button' >\r\n"
"          <input type='submit' name='dir' value='right' class='button' ><br/>\r\n"
"          <input type='submit' name='dir' value='lback' class='button' >\r\n"
"          <input type='submit' name='dir' value='back' class='button' >\r\n"
"          <input type='submit' name='dir' value='rback' class='button' ><br/>\r\n"
"          <input type='submit' name='dir' value='rrotate' class='button' >\r\n"
"          <input type='submit' name='dir' value='lrotate' class='button' >\r\n"
"        </div>\r\n"
"      </form>\r\n"
"    </div>\r\n"
"</body>\r\n"
"</html>\r\n";

unsigned long pre_time;
String ctrl_str="None";

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */
void setup() {

  M5.begin();
  M5.update();
  
  Wire.begin(0, 26, 10000);
	delay(1000);
  
	Serial.begin(115200);
	Serial.println();
	Serial.print("Configuring access point...");

  //Wire.begin(4, 14);
  //delay(40);

  // Init M5Bala
  rover_c.begin();
  
	/* You can remove the password parameter if you want the AP to be open. */
	WiFi.softAP(ssid, password);
  delay(100);
	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

	server.on("/",HTTP_GET, handleRoot);
  server.on("/control", HTTP_POST,handle_control);
 
	server.begin();
  
	Serial.println("Server started");
  //pinMode(16,OUTPUT);
  //pinMode(12,OUTPUT);

  //LED_H;
  
  delay(100);
  pre_time = millis();
}

void loop() 
{
  if((unsigned long)(millis() - pre_time) > 300){
    rover_c.run();
    //Serial.println(rover_c.x_val);
    //Serial.println(rover_c.y_val);
    //Serial.println(rover_c.yaw_val);
    drawScreen();
    ws.text(g_client_id,String(-1));
    pre_time = millis();
  }
  //M5.update();
}

void drawScreen() {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(1, 1);
    M5.Lcd.printf("BatV:%5.1fV ", M5.Axp.GetBatVoltage());
    M5.Lcd.setCursor(1, 11);
    M5.Lcd.printf("BatC:%5.1fmA ", M5.Axp.GetBatCurrent());
    M5.Lcd.setCursor(1, 21);
    M5.Lcd.printf("%s",ctrl_str);
}

void handleRoot(AsyncWebServerRequest *request) {
  request->send(200, "text/html", form);
}

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){

  if(type == WS_EVT_CONNECT){
    Serial.println("Websocket client connection received");
    g_client_id = client->id();

  } else if(type == WS_EVT_DISCONNECT){
    Serial.println("Client disconnected");
    g_client_id = -1;
  } 
}



void handle_control(AsyncWebServerRequest *request)
{
  String s = request->arg("dir");
  ctrl_str = s;
  if (s == "forward") {
      rover_c.move(0,50,0);
  } else if (s == "left") {
      rover_c.move(50,0,0);
  } else if (s == "right") {
      rover_c.move(-50,0,0);
  } else if (s == "back") {
      rover_c.move(0,-50,0);
  } else if (s == "lforward") {
      rover_c.move(50,50,0);
  } else if (s == "rforward") {
      rover_c.move(-50,50,0);
  } else if (s == "lback") {
      rover_c.move(50,-50,0);
  } else if (s == "rback") {
      rover_c.move(-50,-50,0);
  } else if (s == "lrotate") {
      rover_c.turn(30,0);
  } else if (s == "rrotate") {
      rover_c.turn(-30,0);
  } else if (s == "stop") {
      rover_c.stop();
  } else {
      rover_c.stop();
  }
  request->send(200, "text/html", "");
}
