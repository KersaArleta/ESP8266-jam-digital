#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Wifi *Important
const char* ssid     = "Name wifi";
const char* password = "Password";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200, 60000); 

bool showColon = true; 
String message = "Semangat terus yaa...";  // Massage
int textX = SCREEN_WIDTH;  
int textY = 50;
unsigned long previousMillis = 0;
unsigned long colonPreviousMillis = 0;
unsigned long textPreviousMillis = 0;
const long interval = 1000;  // Interval kedipan titik dua
const long textInterval = 50;  // Interval kecepatan teks berjalan

void setup(){
  Serial.begin(115200);

  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize NTPClient
  timeClient.begin();
  timeClient.update();

  setTime(timeClient.getEpochTime()); // Set from NTP

  display.clearDisplay();

  updateTimeAndDate();
}

void updateTimeAndDate() {
  display.fillRect(0, 0, SCREEN_WIDTH, 40, SSD1306_BLACK);

  // Clock setting
  display.setTextSize(4);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(5, 0);

  if (hour() < 10) {
    display.print("0");
  }
  display.print(hour());

  if (showColon) {
    display.print(":");
  } else {
    display.print(" ");
  }

  if (minute() < 10) {
    display.print("0");
  }
  display.print(minute());

  // Showing date
  display.setTextSize(1);
  display.setCursor(33, 32);

  String months[] = {"Jan", "Feb", "Mar", "Apr", "Mei", "Jun", "Jul", "Agu", "Sep", "Okt", "Nov", "Des"};
  String formattedDate = String(day()) + " " + months[month() - 1] + " " + String(year());
  display.print(formattedDate);
}

void drawScrollingText(String text, int &x, int y) {
  int textLength = text.length();
  int textWidth = textLength * 6; 

  display.fillRect(0, y -5, SCREEN_WIDTH, 20, SSD1306_BLACK);

  for (int i = 0; i < textLength; i++) {
    int charX = x + i * 6; 

    if (charX >= 0 && (charX + 6) <= SCREEN_WIDTH) {
      display.setCursor(charX, y);
      display.print(text[i]);
    }
  }

  x--;

  // Reset 
  if (x < -textWidth) {
    x = SCREEN_WIDTH;
  }
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Update NTP client 
  timeClient.update();
  setTime(timeClient.getEpochTime());

  if (currentMillis - colonPreviousMillis >= interval) {
    colonPreviousMillis = currentMillis;
    showColon = !showColon;
    updateTimeAndDate();
  }

  if (currentMillis - textPreviousMillis >= textInterval) {
    textPreviousMillis = currentMillis;
    drawScrollingText(message, textX, textY);  
  }

  display.display();
}
