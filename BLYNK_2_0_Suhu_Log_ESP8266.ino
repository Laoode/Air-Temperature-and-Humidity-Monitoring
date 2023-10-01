// Definisi konfigurasi Blynk
#define BLYNK_TEMPLATE_ID "TMPL6ARpXN06c"
#define BLYNK_DEVICE_NAME "Monitoring Suhu dan Kelembapan"
#define BLYNK_AUTH_TOKEN "mCW5XQSTYwC69KDgw30db-sd0S9PgPrS"
#define BLYNK_PRINT Serial

// Penggunaan Library
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Inisialisasi objek LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set alamat LCD menjadi 0x27 dengan tampilan 16 karakter dan 2 baris

// Konfgurasi Wifi dan autenstikasi Blynk
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Yudhy's Network";  // Tulis nama WiFi Anda di sini
char pass[] = "123Yudhy!456";  // Tulis kata sandi WiFi Anda di sini

// Komfigurasi pin dan sensor DHT
#define DHTPIN D5        // Tentukan PIN tempat Anda menghubungkan sensor DHT
#define DHTTYPE DHT22    // Gunakan sensor DHT11
DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// Inisialisasi port virtual Blynk
#define BLYNK_SENSOR_TEMP    V0
#define BLYNK_SENSOR_HUM     V1

// Deklarasi timer Blynk dan variabel tampilan LCD
BlynkTimer timer;
int displayMode = 0;
unsigned long lastDisplayChange = 0;

// Fungsi deteksi_sensor() untuk membaca dan menampilkan data suhu dan kelembapan
void deteksi_sensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // atau dht.readTemperature(true) untuk Fahrenheit
  // Periksa kegagalan membaca sensor DHT 
  if (isnan(h) || isnan(t)) {
    Serial.println("Gagal membaca sensor DHT!");
    return;
  }
  lcd.clear();
  // Tampilan mode 0: Tampilkan suhu dalam derajat Celcius dan Fahrenheit
  if (displayMode == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Temperature:");
    lcd.setCursor(1, 1);
    lcd.print(t);
    lcd.print((char)223);
    lcd.print("C|");
    lcd.print((t * 9 / 5) + 32);
    lcd.print((char)223);
    lcd.print("F");
  }
  // Tampilan mode 1: Tampilkan kelembapan
  else if (displayMode == 1) {
    lcd.setCursor(0, 0);
    lcd.print("Humidity:  ");
    lcd.setCursor(1, 1);
    lcd.print(h);
    lcd.print("%");
  }
  // Tampilkan data suhu dan kelembapan di Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print("°C | ");
  Serial.print((t * 9 / 5) + 32);
  Serial.println("°F");
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println("%");
}

// . Fungsi send_sensor_blynk() untuk mengirim data suhu dan kelembapan ke platform Blyn
void send_sensor_blynk() {
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // atau dht.readTemperature(true) untuk Fahrenheit
  // Kirim data suhu dan kelembapan ke port virtual Blynk
  Blynk.virtualWrite(BLYNK_SENSOR_HUM, h);
  Blynk.virtualWrite(BLYNK_SENSOR_TEMP, t);
  // Log event jika suhu melebihi batas yang ditentukan 
  if (t > 35) {
    Blynk.email("mr.yudhyt@gmail.com", "Alert", "Temperature over 35°C!");
    Blynk.logEvent("temp_alert", "Suhu Melebihi Batas Yang Ditentukan");
  }
}

// Fungsi setup() untuk inisialisasi LCD, koneksi WiFi, dan Blynk
void setup() {
  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("MENDETEKSI");
  lcd.setCursor(0, 1);
  lcd.print("SUHU RUANGAN 1");
  delay(3000);
  lcd.clear();

  Serial.begin(115200);
  delay(100);
  lcd.setCursor(0, 0);
  lcd.print("Menghubungkan ke");
  lcd.setCursor(0, 1);
  lcd.print(ssid);
  delay(2000);
  lcd.clear();

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  lcd.setCursor(0, 0);
  lcd.print("WiFi terhubung..");
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Mulai ....");
  delay(2000);
  lcd.clear();

  Blynk.begin(auth, ssid, pass);
  dht.begin();
  timer.setInterval(1000L, deteksi_sensor);
  timer.setInterval(1000L, send_sensor_blynk);
}

// Fungsi loop() untuk menjalankan Blynk dan timer
void loop() {
  Blynk.run();
  timer.run();

  if (millis() - lastDisplayChange >= 5000) {
    displayMode = (displayMode + 1) % 2;
    lastDisplayChange = millis();
  }
}