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

// Wipi
const char* ssid     = "ARUN-05";
const char* password = "Arun2015";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200, 60000); 

bool showColon = true; // Untuk mengatur kedipan titik dua
String message = "Semangat terus yaa...";  // Pesan yang ingin ditampilkan
int textX = SCREEN_WIDTH;  // Mulai dari kanan layar
int textY = 50;
unsigned long previousMillis = 0;
unsigned long colonPreviousMillis = 0;
unsigned long textPreviousMillis = 0;
const long interval = 1000;  // Interval kedipan titik dua
const long textInterval = 50;  // Interval kecepatan teks berjalan

void setup(){
  // Initialize Serial Monitor
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

  setTime(timeClient.getEpochTime()); // Set waktu awal dari NTP

  display.clearDisplay();

  // Menampilkan Jam dan Tanggal pertama kali
  updateTimeAndDate();
}

void updateTimeAndDate() {
  // Bersihkan hanya area jam dan tanggal
  display.fillRect(0, 0, SCREEN_WIDTH, 40, SSD1306_BLACK);

  // Menampilkan Jam
  display.setTextSize(4);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(5, 0);

  // Menambahkan nol di depan jam jika kurang dari 10
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

  // Menampilkan Tanggal di bawah Jam
  display.setTextSize(1);
  display.setCursor(33, 32);

  // Format tanggal: 18 Agustus 2024
  String months[] = {"Jan", "Feb", "Mar", "Apr", "Mei", "Jun", "Jul", "Agu", "Sep", "Okt", "Nov", "Des"};
  String formattedDate = String(day()) + " " + months[month() - 1] + " " + String(year());
  display.print(formattedDate);
}

void drawScrollingText(String text, int &x, int y) {
  int textLength = text.length();
  int textWidth = textLength * 6; // Setiap karakter dengan font ukuran 1 membutuhkan 6 pixel

  // Bersihkan area teks berjalan sebelum menggambar ulang
  display.fillRect(0, y -5, SCREEN_WIDTH, 20, SSD1306_BLACK);

  // Loop melalui setiap karakter dalam string
  for (int i = 0; i < textLength; i++) {
    int charX = x + i * 6; // Menghitung posisi X untuk setiap karakter

    // Pastikan karakter tidak keluar dari layar
    if (charX >= 0 && (charX + 6) <= SCREEN_WIDTH) {
      display.setCursor(charX, y);
      display.print(text[i]);
    }
  }

  // Geser teks ke kiri
  x--;

  // Reset posisi jika seluruh teks sudah lewat dari layar
  if (x < -textWidth) {
    x = SCREEN_WIDTH;
  }
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Update NTP client untuk mendapatkan waktu terbaru
  timeClient.update();
  setTime(timeClient.getEpochTime());

  // Perbarui waktu dan tanggal setiap detik
  if (currentMillis - colonPreviousMillis >= interval) {
    colonPreviousMillis = currentMillis;
    showColon = !showColon;
    updateTimeAndDate();
  }

  // Menampilkan teks berjalan
  if (currentMillis - textPreviousMillis >= textInterval) {
    textPreviousMillis = currentMillis;
    drawScrollingText(message, textX, textY);  // Menggambar teks berjalan
  }

  display.display();
}
