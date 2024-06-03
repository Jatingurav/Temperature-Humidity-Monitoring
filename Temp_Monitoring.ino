#include <DHT.h> //DHT Library
//Import AsyncWebServer Library
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
//Import OLED_DISPLAY Library
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//OLED Specifications
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//SSID and PASSWORD
const char* ssid = "Mahesh";
const char* password = "Aryan@14";

AsyncWebServer server(80);

#define DHT_PIN 4  //Digital GPIO Pin
#define DHT_TYPE DHT11 //DHT11 Type Sensor
DHT dht(DHT_PIN, DHT_TYPE); //dht() is a object of class DHT

//Function for reading temperature
String readDHTTemperature(){
  float t = dht.readTemperature();

  if (isnan(t)){  //if t=nan(not a number)
    Serial.println("Failed to read Temperature");
  }
  else{
    Serial.println("Temperature: ");
    Serial.println(t);
    return String(t);
  } 
}

//Function for reading Humidity
String readDHTHumidity(){
  float h = dht.readHumidity();

  if (isnan(h)){  //if h=nan(not a number)
    Serial.println("Failed to read Humidity");
  }
  else{
    Serial.println("Humidity: ");
    Serial.println(h);
    return String(h);
  }
}

//Array of string containing HTML content which is stored in Program Memory and raw string means special characters are Allowed
const char index_html[] PROGMEM = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>ESP Temperature And Humidity Monitoring</title>
    <style>
      html{font-family: Arial; display: inline-block; margin: 0px auto; text-align: center;}
      body {background-image: url("https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcTRReiSWdoPWAKYC6NV4GwxOvj7fz9i9wK2tw&s"); background-repeat: no-repeat; background-size: cover;}");
      h2{font-size:6.0rem}
      p{font-size: 4.0rem}
      .units{font-size: 3.0rem}
      .dht-labels{font-size: 4rem; vertical-align:middle; padding-bottom: 15px;}
    </style>
  </head>
  <body>
    <h1>Temperature & Humidity Monitoring</h1>
    <h2>Place: Mumbai</h2>
    <p> 
      <span class="dht-labels">Temperature:  </span>
      <span id="temperature">%TEMPERATURE% </span>  
      <sup class="units">&deg;C</sup>
    </p>

    <p>
      <span class="dht-labels">Humidity:  </span>
      <span id-"humidity">%HUMIDITY% </span>
      <sup class="units">&percnt;</sup>
    </p>
  </body>

  <script>
  setInterval(function(){
    var xhttp = new XMLHttpRequest;
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.state == 200){
        document.getElementById("temperature").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET",/temperature, true);
    xhhtp.send();
  }, 10000 );

  setInterval(function(){
    var xhttp = new XMLHttpRequest;
    xhttp.onreadystatechange =function(){
      if (this.readyState == 4 && this.state==200){
        document.getElementById("humidity").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", /humidity, true);
  }, 10000 );

  </script>
  </html>
)rawliteral";

String processor_temp_hum(const String& var){
  if (var == "TEMPERATURE"){  //TEMPERATURE is PLACEHOLDER
    return readDHTTemperature();
  }

  else if (var == "HUMIDITY"){ //HUMIDITY is PLACEHOLDER
    return readDHTHumidity();
  }
  return String();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS); //OLED Display

  dht.begin(); //Initialize DHT Sensor 
  WiFi.begin(ssid, password); //WiFi Sensor
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html",index_html, processor_temp_hum);
  });

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", readDHTTemperature().c_str());
  });

    server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", readDHTHumidity().c_str());
  });

  server.begin();
    //Display on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Temperature: ");
  display.setTextSize(2);
  display.setCursor(0,10);
  display.println(readDHTTemperature());
  display.setTextSize(1);
  display.setCursor(70,10);
  display.cp437(true); //To print  º symbol, we use the Code Page 437 font. For that, you need to set the cp437 to true 
  display.write(167); //use the write() method to display your chosen character. The º symbol corresponds to character 167.
  display.setTextSize(2);
  display.print("C");

    // display humidity
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print("Humidity: ");
  display.setTextSize(2);
  display.setCursor(0, 45);
  display.print(readDHTHumidity());
  display.print(" %"); 

  display.display();


}

void loop() {
  // put your main code here, to run repeatedly:

}
