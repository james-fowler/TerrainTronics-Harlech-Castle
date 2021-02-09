//**************************************************************//
//  Name    : Harlech Castle Wifi Control                       //
//  Author  : Dafydd Roche                                      //
//  Date    : 2/8/2021                                         //
//  Version : 1.0                                               //
//  Notes   : This code runs on the Wemos D1 Mini.              //
//****************************************************************


/* Tap the little plus sign for details including users guide!
 *  
 * Hi. This code creates a server on your network once the wifi is configured.
 * When you first run it, look for a new Wifi network. It'll be called "AutoConnectAP" with the password "password"
 * Configure it with your wifi details and it should connect to your network.
 * 
 * From there, you'll access it from your browser, through your normal wifi connection. This code is set up so that 
 * you'll be able to find it at "http://terraintronics01" but that can be changed below. To know the IP address, 
 * open the serial port monitor in arduino! You should be a message in there along the lines of: "*WM: [1] STA IP Address: 192.168.1.97"
 * If your network is being funny and not allowing the name, you can use http://192.168.1.97 (or whatever IP address your terrain was given) 
 * 
 * One good option might be to rename the "terraintronics01" to the name of your terrain, for instance "airstripterrain" - you'd then 
 * able to find it easily in your web browser.
 *  
 *  
 * How it works: 
 * https://github.com/tzapu/WiFiManager - WifiManager Library was used to allow users to config from their phone. it's cool.
 * Control is via WebSockets technology. The webpage is served FROM the Wemos. When you hit a button in the browser
 * your browser sends a "1", "2", or a "3" to the Wemos D1, which then takes an action based on which one you pressed.
 * The actions are detailed in the section starting with "void webSocketEvent" below.
 * The web page served by the wemos is below too. It is stored on the device. It's recommended to keep the web page as simple 
 * as possible to save space.
 *  
 * 
 * 
 * 
 * credits and notes:
 *  * Sketch: ESP8266_Part9_01_Websocket_LED
 * Intended to be run on an ESP8266
 * This Code Originally started over at: http://www.martyncurrey.com/esp8266-and-the-arduino-ide-part-9-websockets/
 * 
 */



 
String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
 
String html_1 = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'/>
  <meta charset='utf-8'>
  <style>
    body    { font-size:120%;} 
    #main   { display: table; width: 300px; margin: auto;  padding: 10px 10px 10px 10px; border: 3px solid blue; border-radius: 10px; text-align:center;} 
    .button { width:200px; height:40px; font-size: 80%;  }
  </style>
  <title>TerrainTronics Harlech Castle Demo</title>
</head>
<body>
  <div id='main'>
    <h3>Terrain Tronics Wifi Controllers</h3>
    <div id='content'>
      <p id='LED_status'>Development Date 2/7/2021</p>
      <button id='BTN_LEDA'class="button">Turn on Candles only</button>
      <button id='BTN_LEDB'class="button">Turn on Red Lights only </button>
      <button id='BTN_LEDC'class="button">All lights off</button>
    </div>
    <br />
   </div>
</body>
 
<script>
  var Socket;
  function init() 
  {
    Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
  }
 
  document.getElementById('BTN_LEDA').addEventListener('click', buttonAClicked);
  function buttonAClicked()
  {   
    sendText('0');
  }
  document.getElementById('BTN_LEDB').addEventListener('click', buttonBClicked);
  function buttonBClicked()
  {   
    sendText('1');
  }
 document.getElementById('BTN_LEDC').addEventListener('click', buttonCClicked);
  function buttonCClicked()
  {   
    sendText('2');
  }
  function sendText(data)
  {
    Socket.send(data);
  }
 
  window.onload = function(e)
  { 
    init();
  }
</script>
</html>
)=====";




 // ALL THAT STUFF ABOVE IS BROWSER/PC CODE.
 // THIS IS THE ESP8266 Stuff!


#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>

#include <strings_en.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager 


// #define DEBUG 0 // this is used to switch debug comments on and off. Comment it out with // to optimize the code.

 
/*------------------------------------------------------------*/ 
/* Hardware Pin Definitions                                   */
/*------------------------------------------------------------*/
byte pin_led = D4;
const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to.
int latchPin = D8;//These definitions configure the pins for the SPI interface to talk to the LED driver
int clockPin = D5;
int dataPin = D6;
int OE = D4;// Set the output enable pin. This can be rapidly PWM'd.

/*------------------------------------------------------------*/ 
/* WIFI and Server Configuration                              */
/*------------------------------------------------------------*/
WiFiServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
char myHostname[] = "TerrainTronics01"; // you can communicate with your product by accessing http://TerrainTronics01/


/*------------------------------------------------------------*/ 
/* USB Powerbank Keepalive Variables                          */
/*------------------------------------------------------------*/

// This sets up the USBBank Keep Alive Functionality. By default, this is set to 100mA of additional current, for 1S every 10.
#include <Ticker.h>  //Ticker Library
Ticker USBBank; // Sets up a ticker for the USB Powerbank Keep Alive
Ticker PatternRun; // Used in the background task used to run the patterns
int USBBankState = 0;
const int keepAlive = D3;// The Keepalive pin for USB Power Banks is D3.
const int kaTimeOn = 1; // Time to switch the keep alive circuit on.
const int kaTimeOff = 8; // Time to switch the keep alive circuit off for.


/*------------------------------------------------------------*/ 
/* LED Pattern Definitions and variables used in counting etc.*/
/*------------------------------------------------------------*/
int ledPattern[][3] = {   {0b11111111,  0,  500 } ,
                          {0b00000000,  0,  500 } ,
                          {0b11111111,  0,  500} ,
                          {0b00000000,  0,  500 } ,
                          {0b10001000,  0,  250 } ,
                          {0b01000100,  0,  250 } ,
                          {0b00100010,  0,  250 } ,
                          {0b00010001,  0,  250 } ,
                          {0b00000000,  0,  1000 } };


// Set Up and initialize the pointers used for patterns                          
int patternIndex = 0;
int patternData = 0;
int patternDelay = 0;
int numrows = 0;
int numRows = sizeof(ledPattern)/sizeof(ledPattern[0]);

int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)
int dutyCycle = 1023;          // set starting - PWM Duty Cycle. Data is inverted so 1023 is off.
int flashdelay = 500;
int breathDir = 0;  //Breathing Direction - 0 is counting up, 1 is counting down




//=======================================================================
// This is a subroutine that switches the keepAlive pin on/off to switch on a transistor that pulls current to ground, through a 47Ohm resistor
// PLEASE NOTE: You cannot leave this on for a long long time, as the resistor will begin to heat up beyond it's rating!

void keepAliveChangeState()
{
  Serial.println("Keepalive Change");
  if (USBBankState == 0 ){
    Serial.println("Switching off");
    USBBankState = 1;
    digitalWrite(keepAlive, 0); // Switch Transistor Off
    USBBank.attach(kaTimeOff, keepAliveChangeState); //Use attach_ms   
  }
  else if (USBBankState == 1 ){
    Serial.println("Switching on");
    USBBankState = 0;
    digitalWrite(keepAlive, 1); // Switch Transistor On
    USBBank.attach(kaTimeOn, keepAliveChangeState); //Use attach_ms   
  }
}

void runCurrentLedPattern()
{

  patternData = ledPattern[patternIndex][0];
  dutyCycle = ledPattern[patternIndex][1];
  patternDelay = ledPattern[patternIndex][2];
  //Serial.println(patternDelay);
  //Serial.println("runCurrentLedPattern");

   digitalWrite(latchPin, LOW); // Tells the LED driver IC to listen
   shiftOut(dataPin, clockPin, LSBFIRST, patternData);
   digitalWrite(latchPin, HIGH); // Tells the LED driver 
   analogWrite(OE, dutyCycle);
   patternIndex++; // Step to the next item in the index
   if (patternIndex == numRows) {    // if you hit the maximum number of rows in teh pattern, set it back to the 0'th row (the first)
    patternIndex = 0;
    }

    PatternRun.attach_ms(patternDelay, runCurrentLedPattern); //Use attach_ms   
  
}










 
void setup()
{
  Serial.begin(115200); // Switches on the data back to the PC.
  Serial.println();
  Serial.println("Serial started at 115200");
  Serial.println();
  
  
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;
  //reset settings - wipe credentials for testing
  //wm.resetSettings();
  // Automatically connect using saved credentials,
  // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
  // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
  // then goes into a blocking loop awaiting configuration and will return success result

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  res = wm.autoConnect("AutoConnectAP","password"); // password protected ap

  if(!res) {
      Serial.println("Failed to connect");
      // ESP.restart();
  } 
  else {
      //if you get here you have connected to the WiFi    
      Serial.println("connected...yeey :)");
      Serial.println("");
      Serial.println(F("[CONNECTED]"));   Serial.print("[IP ");  Serial.print(WiFi.localIP()); 
      Serial.println("]");
  }
  
  pinMode(pin_led, OUTPUT);
  digitalWrite(pin_led,LOW);

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  pinMode(keepAlive, OUTPUT);     // KeepAlive pin needs pulling low, otherwise it'll float up
  digitalWrite(keepAlive, LOW);   // and switch on the transistor. - THIS IS IMPORTANT, OTHERWISE IT FLOATS UP AND SWITCHES THE TRANSISTOR ON and the board will get hot!

 
 
 
  // start a server
  server.begin();
  Serial.println("Server started");

  // Server Starts, then starts the USB Powerbank keepalive, and the first pattern. Use comments (//) to not start patterns until you click the GUI.
  USBBank.attach(5, keepAliveChangeState); // This starts the timer for the USB Power Bank Keepalive.
  // PatternRun.attach_ms(500, runCurrentLedPattern); // This starts the timer for the patterns.

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}
 
void loop()
{
    //Serial.println("LOOP");
    webSocket.loop();
 
    WiFiClient client = server.available();     // Check if a client has connected
    if (!client)  {  return;  }
 
    client.flush();
    client.print( header );
    client.print( html_1 ); 
    Serial.println("New page served");
 
    delay(5);
}
 
void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length)
{
  if(type == WStype_TEXT)
  {
      if (payload[0] == '0')
      {
          Serial.println("CANDLES ON");    
          digitalWrite(latchPin, LOW); // Tells the LED driver IC to listen
          shiftOut(dataPin, clockPin, LSBFIRST, 0b00001100);
          digitalWrite(latchPin, HIGH); // Tells the LED driver 
          analogWrite(OE, 0);  
          delay(5);
      }
      else if (payload[0] == '1')
      {
          Serial.println("RED LIGHT ON"); 
          digitalWrite(latchPin, LOW); // Tells the LED driver IC to listen
          shiftOut(dataPin, clockPin, LSBFIRST, 0b00000011);
          digitalWrite(latchPin, HIGH); // Tells the LED driver 
          analogWrite(OE, 0);
          delay(5);
      }
      else if (payload[0] == '2')
      {
          Serial.println("ALL OFF"); 
          digitalWrite(latchPin, LOW); // Tells the LED driver IC to listen
          shiftOut(dataPin, clockPin, LSBFIRST, 0b00000000);
          digitalWrite(latchPin, HIGH); // Tells the LED driver 
          analogWrite(OE, 0);
          delay(5);
      }
  }
 
  else 
  {
    Serial.print("WStype = ");   Serial.println(type);  
    Serial.print("WS payload = ");
    for(int i = 0; i < length; i++) { Serial.print((char) payload[i]); }
    Serial.println();
 
  }
}
