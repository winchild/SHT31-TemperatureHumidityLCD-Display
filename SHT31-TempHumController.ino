//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "Adafruit_SHT31.h"

// INPUT / OUTPUT 핀 설정
#define DHTPIN 8        // SDA 핀의 설정
#define DHTTYPE DHT22   // DHT22 (AM2302) 센서종류 설정

#define TEMP_PIN 6  // 온도 히터
#define HUMI_PIN 7  // 가습기 히터

#define TEMPERATE_SETTING_MODE 0
#define HUMIDITY_SETTING_MODE  1

// Rotary Encoder Pin Assign
#define RT_SW_PIN 4   // Rotary Push Button Pin #
#define RT_DATA_PIN 2 // Data Pin #
#define RT_CLK_PIN 3  // Click Pin #

// 온도 및 습도 최소와 최대 설정치
#define TEMPE_MAX 50 // 온도 최대치
#define TEMPE_MIN 20 // 온도 최저치
#define HUMDT_MAX 95 // 습도 최대치
#define HUMDT_MIN 30 // 습도 최저치

#define HUMDT_PUZZY_CONST 0.9

// 함수 선언
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
Adafruit_SHT31 sht31 = Adafruit_SHT31();
//DHT dht(DHTPIN, DHTTYPE); // DHT 온습도 센서

// S E T U P
//
void setup()
{
  lcd.init();                      // initialize the lcd 
  //lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Temp & Humid CTL");
  lcd.setCursor(0,1);
  lcd.print("by winchild");

  delay (1000);

  // 히터 릴레이 초기화
  pinMode (TEMP_PIN, OUTPUT);
  pinMode (HUMI_PIN, OUTPUT);

  // 로터리 엔코더 값 초기화
  pinMode(RT_SW_PIN, INPUT);
  pinMode(RT_DATA_PIN, INPUT);
  pinMode(RT_CLK_PIN, INPUT);
  digitalWrite(RT_SW_PIN, HIGH);

  // 로터리 엔코더 값 입력시 인터럽트 함수 호출
  attachInterrupt(digitalPinToInterrupt(RT_DATA_PIN), serviceEncoderInterrupt, CHANGE); 
  attachInterrupt(digitalPinToInterrupt(RT_CLK_PIN), serviceEncoderInterrupt, CHANGE);

  Serial.begin(9600); 
  Serial.println("Temperature Humidity Controller start!!!");
 
  //dht.begin();
  // Serial.println("SHT31 test");
  if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    //Serial.println("Couldn't find SHT31");
    lcd.setCursor(0,0);
    lcd.print ("Couldn't find SHT31");
    while (1) delay(1);
  }
}

// 글로벌 변수
//
int TargetHumdt = 80; // target humidity
int TargetTempe = 26; // target temperature

int toggle = 0;

volatile int RotaryValue = 26; // 로터리 엔코더 누적 값.
int prevRotaryValue = 26;
int SettingMode = TEMPERATE_SETTING_MODE;

// 메인루프
// 
void loop() {

  double Distance;
  
  // 센서의 온도와 습도를 읽어온다.
  //int Humdt = dht.readHumidity();
  //int Tempe = dht.readTemperature();
  float Tempe = sht31.readTemperature();
  float Humdt = sht31.readHumidity();

  
  if (isnan(Tempe) || isnan(Humdt)) {
    //값 읽기 실패시 시리얼 모니터 출력
    //Serial.println("Failed to read from DHT");
    lcd.setCursor (0,0); lcd.print("Value Get Error");
  } else {

    // 온도 표시 LCD 출력    
    lcd.setCursor (0,0); lcd.print("TMP");
    if (SettingMode == TEMPERATE_SETTING_MODE)
    {
      if (toggle) lcd.print("_");
      else lcd.print (" ");
    }
    else lcd.print (" ");
    lcd.print(Tempe);lcd.print("/");
    lcd.print(TargetTempe);
    lcd.print ("C[");
    
    if (Tempe < TargetTempe) {
      digitalWrite (TEMP_PIN, LOW); // 히터 ON
      if (toggle) lcd.print("*");
      else lcd.print (" ");
    }
    else {
      digitalWrite (TEMP_PIN, HIGH); // 히터 OFF
      lcd.print(" ");
    }
    lcd.print ("]");

    // 습도 표시 LCD 출력
    lcd.setCursor (0,1); lcd.print("HDT");
    if (SettingMode == HUMIDITY_SETTING_MODE)
    {
      if (toggle) lcd.print("_");
      else lcd.print (" ");
    }
    else lcd.print (" ");
    lcd.print(Humdt);lcd.print("/");lcd.print(TargetHumdt);
    lcd.print("%[");
    if (Humdt < TargetHumdt) {
      digitalWrite (HUMI_PIN, LOW);  // 가습히터 ON
      if (toggle) lcd.print("*");
      else lcd.print(" ");
    }
    else {
      digitalWrite (HUMI_PIN, HIGH);  // 가습히터 OFF
      lcd.print(" ");
    }
    lcd.print ("]");

  }

  // 깜박임을 표시하기 위한 토글 플래그
  if (toggle) toggle = 0;
  else toggle = 1;

  // 로터리 엔코더 푸쉬버튼
  if (LOW == digitalRead (RT_SW_PIN))
  {
    if (SettingMode == HUMIDITY_SETTING_MODE)
    {
      SettingMode = TEMPERATE_SETTING_MODE;
      prevRotaryValue = RotaryValue = TargetTempe;
    }
    else 
    {
      SettingMode = HUMIDITY_SETTING_MODE;
      prevRotaryValue = RotaryValue = TargetHumdt;
    }
  }

  // 로터리엔코더 값이 변했는지 확인
  if (prevRotaryValue != RotaryValue)
  {
    if (SettingMode == HUMIDITY_SETTING_MODE)
    {
      if (RotaryValue >= HUMDT_MIN && RotaryValue <= HUMDT_MAX)
      {
        TargetHumdt = RotaryValue;
        prevRotaryValue = RotaryValue;
      }
    }
    else 
    {
      if (RotaryValue >= TEMPE_MIN && RotaryValue <= TEMPE_MAX)
      {
        TargetTempe = RotaryValue;
        prevRotaryValue = RotaryValue;
      }
    }
    //Serial.println(prevRotaryValue);
  }
  delay(10);
}


// 인코더 값을 읽어오기
//
void serviceEncoderInterrupt ()
{
  static int oldA = LOW;
  static int oldB = LOW;
  int rValue = 0;
 
  int A = digitalRead(RT_DATA_PIN);
  int B = digitalRead(RT_CLK_PIN);

 if (A != oldA || B != oldB)
 {
   if (oldA == LOW && A == HIGH)
   {
     if (oldB == HIGH) rValue = 1;
     else rValue = -1;
   }
 }
 oldA = A;
 oldB = B;

 if (rValue != 0) RotaryValue += rValue;
}
