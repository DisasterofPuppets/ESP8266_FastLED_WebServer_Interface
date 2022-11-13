// Credit to https://tttapa.github.io/ESP8266/Chap10%20-%20Simple%20Web%20Server.html for tutorials on the server side

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h> 
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <FastLED.h>

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    88
CRGB leds[NUM_LEDS];


uint8_t gHue = 0; // rotating "base color" used by many of the patterns
#define BRIGHTNESS          255
#define FRAMES_PER_SECOND  120

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'

ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80

void handleRoot();              // function prototypes for HTTP handlers
void handleLEDselection();
void handleNotFound();

void setup(void){
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(300);
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  
  Serial.println('\n');

  wifiMulti.addAP("Username", "Password");   // add Wi-Fi networks you want to connect to


  Serial.println("Connecting ...");
  int i = 0;
  while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(250);
    Serial.print('.');
  }
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to the computer

  if (MDNS.begin("esp8266")) {              // Start the mDNS responder for esp8266.local
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }

  server.on("/", HTTP_GET, handleRoot);     // Call the 'handleRoot' function when a client requests URI "/"
  server.on("/LEDselection", HTTP_POST, handleLEDselection);  // Call the 'handleLEDselection' function when a POST request is made to URI "/Switches"
  server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

  server.begin();                           // Actually start the server
  Serial.println("HTTP server started");

  
}

void loop(void){

  // display the LEDS
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 
    // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  
  server.handleClient();                    // Listen for HTTP requests from clients
  delay(500);
}

void handleRoot() {                         // When URI / is requested, send a web page with a button to toggle the LED
/* / is required to escape the " character in the html */
  server.send(200, "text/html", "<h3>Select an option</h3><form action=\"/LEDselection\" method=\"POST\"><br><input type=\"checkbox\" name=\"rainbow\"><label for=\"rainbow\">Rainbow</label><br><input type=\"checkbox\" name=\"LEDselection2\"><label for=\"LEDselection2\">Selection 2</label><br><input type=\"checkbox\" name=\"off\"><label for=\"off\">Off</label><br><br><input type=\"submit\" value=\"submit\"></form>");
}

void handleLEDselection() {                          // If a POST request is made to URI /Switches

 if (server.hasArg("rainbow")){
Serial.println("Option 1");
//Serial.println(server.arg("rainbow"));returns on or off state if needed later

rainbowled();
}

/* Start Codeblock --- duplicate and update this codeblock for each checkbox --*/  

  else if (server.hasArg("LEDselection2")){
Serial.println("Option 2");

/*----------------------- End Code Block --------------------------------------*/
    
    
}
else if (server.hasArg("off")){
 Serial.println("Option 3");
  FastLED.clear();  
 
}
  
  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}


void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

void rainbowled() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}
