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
#define ADDR1  0x64

#define command_start  0
#define command_stop   1
#define command_back  2
#define forward       0x01
#define reverse       0x02
#define servo_left    65
#define servo_right   110

//#define LED_H       (digitalWrite( 12, HIGH ))
//#define LED_L       (digitalWrite( 12, LOW ))

char state = command_stop;
int offset = 0;

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
"    .powerpnl {\r\n"
"      line-height: 1px;\r\n"
"      width: 100px;\r\n"
"      height: 100px;\r\n"
"    }\r\n"
"\r\n"
"    .operationpnl {\r\n"
"      line-height: 30px;\r\n"
"      height: 120px;\r\n"
"    }\r\n"
"\r\n"
"    .timerpnl {\r\n"
"      line-height: 15px;\r\n"
"      height: 90px;\r\n"
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
"\r\n"
"    .configpnl {\r\n"
"      line-height: 50px;\r\n"
"      width: 100px;\r\n"
"      height: 100px;\r\n"
"    }\r\n"
"\r\n"
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
"    function show_led_icon()\r\n"
"    {\r\n"
"      for(var i=0;i<4;i++){\r\n"
"        var target_led = document.getElementById('timerled' + i);\r\n"
"        if(i < count_value){\r\n"
"          target_led.style.fill = '#FF0000';\r\n"
"        }\r\n"
"        else\r\n"
"        {\r\n"
"          target_led.style.fill = '#770000';\r\n"
"        }\r\n"
"      }\r\n"
"    }\r\n"
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
"      }else{           \r\n"
"        document.getElementById('display').innerHTML  = 'remain: ' + evt.data + '[sec]';\r\n"
"\r\n"
"        if(remain_sec == 0){\r\n"
"          count_value = 0;\r\n"
"        } else {\r\n"
"          count_value = parseInt(remain_sec/60 + 1);\r\n"
"        }\r\n"
"\r\n"
"        show_led_icon();\r\n"
"\r\n"
"      }\r\n"
"    };\r\n"
"\r\n"
"    window.onload = function() {\r\n"
" \r\n"
"      var count_up_btn = document.getElementById('btn_count_up');\r\n"
"      \r\n"
"      var count_down_btn = document.getElementById('btn_count_down');\r\n"
"      count_down_btn.onclick = function (){\r\n"
"        if(count_value > 0){\r\n"
"          count_value -= 1;\r\n"
"\r\n"
"          show_led_icon();\r\n"
"        }\r\n"
"      };\r\n"
"\r\n"
"      count_up_btn.onclick = function (){\r\n"
"          if(count_value < 4){\r\n"
"            count_value += 1;\r\n"
"            \r\n"
"            show_led_icon();\r\n"
"            \r\n"
"            var nowcnt = document.getElementById('timer_count');\r\n"
"            nowcnt.value = count_value;\r\n"
"          }\r\n"
"      };\r\n"
"\r\n"
"      count_up_btn.onmousedown = function() {\r\n"
"      }\r\n"
"\r\n"
"      count_up_btn.onmouseup = function() {\r\n"
"      }\r\n"
"\r\n"
"    }\r\n"
"  </script>\r\n"
"</head>\r\n"
"<body>      \r\n"
"  <p id = 'display'>Not connected</p>\r\n"
"          \r\n"
"  <div class='parent'>\r\n"
"    <div class='child powerpnl'>\r\n"
"      <form method='post' id='power_form' action='./power' target='tif0' >\r\n"
"        <iframe src='javascript: false;' style='display: none;' name='tif0' id='tif0'></iframe>\r\n"
"        <input type='checkbox' name='poweron' class='checkboxyz' id='poweron' onchange='document.forms.power_form.submit();'>\r\n"
"        <label for='poweron'>\r\n"
"          <svg class='power-icon' version='1.1' id='_x32_' xmlns='http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink'\r\n"
"            viewBox='-12.5 0 100 100' style='' xml:space='preserve' preserveAspectRatio='none'>\r\n"
"            <g>\r\n"
"              <path\r\n"
"                class='st0'\r\n"
"                d='M 21.648156,5.4768425 A 11.90625,11.90625 0 0 1 24.229107,18.452157 11.90625,11.90625 0 0 1 13.229166,25.802083 11.90625,11.90625 0 0 1 2.2292255,18.452158 11.90625,11.90625 0 0 1 4.8101758,5.476843'\r\n"
"                style='stroke-width:3;stroke-linecap:round;' />\r\n"
"             <path\r\n"
"                class='st0'\r\n"
"                d='m 13.229167,3.3125 c 0,9.260417 0,9.260417 0,9.260417'\r\n"
"                style='stroke-width:3;stroke-linecap:round;' />\r\n"
"            </g>\r\n"
"          </svg>\r\n"
"        </label>\r\n"
"      </form>\r\n"
"    </div>\r\n"
"\r\n"
"    <div class='child operationpnl'>\r\n"
"      <form method='post' id='operation_form' action='./operation' target='tif1' >\r\n"
"        <iframe src='javascript: false;' style='display: none;' name='tif1' id='tif1'></iframe>\r\n"
"        <div>\r\n"
"          <input type='submit' name='operation' value='start' class='button' ><br/>\r\n"
"          <input type='submit' name='operation' value='stop' class='button' >\r\n"
"        </div>\r\n"
"      </form>\r\n"
"    </div>\r\n"
"\r\n"
"    <div class='child timerpnl'>\r\n"
"      <form method='post' id='timersetting_form' action='./timersetting' target='tif2' >\r\n"
"        <iframe src='javascript: false;' style='display: none;' name='tif2' id='tif2'></iframe>\r\n"
"        <div>\r\n"
"          <!--div id='disp_count'>0</div-->\r\n"
"          <svg version='1.1' id='timerindicator' xmlns='http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink' \r\n"
"          x='0' y='0' width='70' height='16' viewBox='0 0 70 16' style='' xml:space='preserve' preserveAspectRatio='none' >\r\n"
"            <g>\r\n"
"              <ellipse id='timerled0' class='led-icon' ry='7' rx='7' cy='8' cx='8' style='fill-opacity:1;stroke:#000000;stroke-width:1;' />\r\n"
"              <ellipse id='timerled1' class='led-icon' ry='7' rx='7' cy='8' cx='26' style='fill-opacity:1;stroke:#000000;stroke-width:1;' />\r\n"
"              <ellipse id='timerled2' class='led-icon' ry='7' rx='7' cy='8' cx='44' style='fill-opacity:1;stroke:#000000;stroke-width:1;' />\r\n"
"              <ellipse id='timerled3' class='led-icon' ry='7' rx='7' cy='8' cx='62' style='fill-opacity:1;stroke:#000000;stroke-width:1;' />\r\n"
"            </g>\r\n"
"          </svg><br>\r\n"
"          <input type='hidden' name='timer_count' id='timer_count' >\r\n"
"          <input type='submit' name='timer' value='timerset' class='button' id='btn_count_up' />\r\n"
"        </div>\r\n"
"      </form>\r\n"
"    </div>\r\n"
"\r\n"
"    <div class='child child04'>\r\n"
"        <input type='submit' name='timer' value='set' class='button' id='btn_count_down' />\r\n"
"    </div>\r\n"
"\r\n"
"    <div class='child controlpnl'>\r\n"
"      <form method='post' id='control_form' action='./control' target='tif3' >\r\n"
"        <iframe src='javascript: false;' style='display: none;' name='tif3' id='tif3'></iframe>\r\n"
"        <div class='controlpnl' >\r\n"
"          <input type='submit' name='dir' value='forward' class='button' ><br/>\r\n"
"          <input type='submit' name='dir' value='left' class='button' >\r\n"
"          <input type='submit' name='dir' value='stop' class='button' >\r\n"
"          <input type='submit' name='dir' value='right' class='button' ><br/>\r\n"
"          <input type='submit' name='dir' value='back' class='button' >\r\n"
"        </div>\r\n"
"      </form>\r\n"
"    </div>\r\n"
"\r\n"
"    <div class='child configpnl'>\r\n"
"      <form method='post' id='config_form' action='./get_config' >\r\n"
"        <input type='submit' name='config_open' value='open' class='button' />\r\n"
"      </from>\r\n"
"    </div>\r\n"
"    %YOU_HAVE_CONTROL%\r\n"
"  </div>\r\n"
"    \r\n"
"</body>\r\n"
"</html>\r\n";

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
}

void loop() 
{

  rover_c.run();

  //if((unsigned long)(millis() - pre_time) > 1000){

    //noInterrupts();
  //  if(b_web_connect){
  //    ws.text(g_client_id,String(remain_timer_sec));
  //    led_bar_rainbow_scroll(sc_cnt++);
  //  }
    //interrupts();

  //  pre_time = millis();
  //}

  //M5.update();
}

void handleRoot(AsyncWebServerRequest *request) {
  request->send(200, "text/html", form);
}

void control(AsyncWebServerRequest *request)
{
  String s = request->arg("dir");
  str = s;
  if (s == "forward") {
      rover_c.move(50,0);
      //Serial.println("forward");
  } else if (s == "left") {
      rover_c.turn(-50,0);
      //Serial.println("left");
  } else if (s == "right") {
      rover_c.turn(50,0);
      //Serial.println("right");
  } else if (s == "back") {
      rover_c.move(-50,0);
      //Serial.println("back");
  } else if (s == "stop") {
      rover_c.stop();
      //Serial.println("stop");
  } else {
      rover_c.stop();
      //Serial.println("stop");
  }
  request->send(200, "text/html", "");
}

void drive(){
    if(state == command_back){
     stop_motor();

     delay(10);

     start_motor();
  
  }else if(state == command_stop){
    start_motor();
  }
  state = command_start;
}

void back(){
    if(state == command_start){
     stop_motor();
     delay(10);
     reverse_motor();
  }else if(state == command_stop){
    reverse_motor();
  }
  state = command_back;
}

void start_motor(){
//  char i, volt;
//  volt = 0x20;
//  for(i=0;i<5;i++){ 
//    volt = volt + ((0x40)*i);
//    volt = volt | forward;
//    motor_func(ADDR1 , volt);
//    delay(10);
//  }
  Setspeed(0, 25, 0);
  delay(10);
}

void reverse_motor(){
//    char i, volt;
//  volt = 0x20;
//  for(i=0;i<5;i++){ 
//    volt = volt + ((0x40)*i);
//    volt = volt | reverse;
//    motor_func(ADDR1 , volt);
//    delay(10);
//  }
  Setspeed(0, -25, 0);
  delay(10);
}

void stop_motor(){
  //motor_func(ADDR1 , 0x18);
  //delay(10);
  //motor_func(ADDR1 , 0x1B);
  Setspeed(0, 0, 0);
  delay(10);
}
