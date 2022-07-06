/* Skaner kodów kreskowych 
#include <SparkFun_DE2120_Arduino_Library.h>
*/

/* MD5 */
#include <MD5.h>

/* FRAM I2C */
#include <Adafruit_EEPROM_I2C.h>
#include <Adafruit_FRAM_I2C.h>

/* Czynik NFC */
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

/* BusIO */
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Adafruit_SPIDevice.h>

/* Wyświetlacz */
#include <Adafruit_SH110X.h>
#include <splash.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <Wire.h>

/*  Zegar */
#include <RTClib.h>

/* Zmienne i define wyswietlacza */
Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);
#define BUTTON_A  9
#define BUTTON_B  6
#define BUTTON_C  5

/* Funkcje wyswietlacza */
void setup_wyswietlacz()
{
  delay(250); // wait for the OLED to power up
  display.begin(0x3C, true); // Address 0x3C default
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  display.setRotation(1);
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  wyczysc();
  display.print("Wlaczony");
}

void wyswietl(String Data, String Godzina, String Kod, String Status, String tekst) 
{
  wyczysc();
  display.setCursor(0,0);
  display.print("Data: ");
  display.println(Data);
  display.print("Godzina: ");
  display.println(Godzina);
  display.print("Kod: ");
  display.println(Kod);
  display.print("Status: ");
  display.println(Status);
  display.print("Komunikat: ");
  display.println(tekst);
  display.display();
}

void wyczysc()
{
  display.clearDisplay();
  display.display();
}

/* Zmienne i define czasu */
RTC_PCF8523 rtc;
String data, godzina, data_godzina;

/* Funckje do czasu */
void setup_czas() 
{
  if (! rtc.begin()) 
  {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) 
  {
    Serial.println("RTC lost power, lets set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void czas()
{
  DateTime time = rtc.now();

  char Data[] = "DD-MM-YYYY";
  data = time.toString(Data);

  char Godzina[] = "hh:mm:ss";
  godzina = time.toString(Godzina);

  char Data_godzina[] = "YYMMDDhhmmss";
  data_godzina = time.toString(Data_godzina);
}

/* Zmienne i define NFC */
PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);
String tagId = "None";
byte nuidPICC[4];
String nfc_id = "AABBCCDD";

/* Funckje do NFC */
void bez_spacji(String tekst)
{
  int j = 0;
  for(int i = 0; i<tekst.length();i++)
  {
    j = i;
    if(tekst[i] == ' ')
    {
      while(j < tekst.length())
      {
        tekst[j] = tekst[j+1];
        j = j + 1;
      }
    }  
  }
  for(int i = 0; i<nfc_id.length();i++)
  {
    nfc_id[i] = tekst[i];
  }
}

/* MD5 */
String wejscie;
String komunikat = "003|";
void do_MD5(String data_godzina, String kierunek, String tag)
{
  komunikat = komunikat + data_godzina + "|" + kierunek +"|" + tag;
  unsigned char* hash=MD5::make_hash(*komunikat);
  char *md5str = MD5::make_digest(hash, 16);
  komunikat = komunikat + "|" + md5str;
}

/* MAIN */

void setup() 
{
  Serial.begin(9600);
  delay(3000);
  setup_czas();
  setup_wyswietlacz();
  nfc.begin();
}



void loop() 
{
  // Czytnik NFC
  if(nfc.tagPresent())
  {
    NfcTag tag = nfc.read();
    czas();
    tagId = tag.getUidString();
    bez_spacji(tagId);
    wejscie = "I";
    // MD5
    do_MD5(data_godzina, wejscie, nfc_id);
    
    wyczysc();
    wyswietl(data, godzina, nfc_id, "Wejscie", komunikat); 
    String komunikat = "003|";
    delay(2000);
  }
  // Czytnik kodów

 
  
  delay(10);
  yield();
  display.display();
}
