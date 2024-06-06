#include <Arduino.h>
#include <Ticker.h>
/* Displays Libraries */
#include <TM1637Display.h>
#include <TM1637TinyDisplay6.h>
#include <U8g2lib.h>
/* User Libraries */
#include "definitions.h"

/* ESP Tools */
Ticker ticker2Hz;
Ticker ticker10Hz;

/* Global Variables */
Txtmng Var, Var_0;
bool emergency_led_state = true;
bool boolean1HZ = true;
bool boolean10HZ = false;
//bool Mode_Sport = false;
uint16_t pot;
uint8_t data_arr[sizeof(Txtmng)];     // array that receives data in Bits from the front ECU (MMI)

/* Interrupts routine */
void ButtonInterruptISR();
//void SwitchInterruptISR();
void ticker2HzISR();
void ticker10HzISR();
/* Setup Functions */
void SetupPacket();
void Pinconfig();
/* General functions */
void Receiver_Data(); // Receiver Function
void Leds_State();
void Comfort_Animation(); // Comfort design animation Function
void LedFuel();
void LedEmergency();
void FourDigits();
void SixDigits();
void Transform_time_current(Time *T);
void Battery_box(uint8_t cor);

void setup()
{
  Serial.begin(115200);      // Define and start serial monitor
  // Define and start Receiver serial port
  Receiver.begin(115200, SERIAL_8N1, Receiver_Txd_pin/*In RX wire*/, Receiver_Rxd_pin/*In Tx wire*/);   

  // Setup Six digits display
  Six.setBrightness(BRIGHT_HIGH);

  // Setup OLED
  u8g2.begin();
  u8g2.enableUTF8Print();

  SetupPacket();
  Pinconfig();
  
  //Switch.buttonState = digitalRead(Switch.Pin);

  six_digits_state = digitalRead(Switch.Pin) & STOPWATCH;

  //if(!six_digits_state)
  //  Mode_Sport = false;
  //else 
  //  Mode_Sport = true;

  delay(10);
}

void loop() 
{
  pot = analogRead(POTENTIOMETER);   // Function to read the potentiometer

  if(!digitalRead(Switch.Pin))
  {
    six_digits_state = ENDURANCE_TIMER;
    //Mode_Sport = false;
    Lap.seconds = 0;
    Lap.minutes = 0;
    Lap.hours = 0;
    Lap.time_current = millis();
  }

  else
  {
    six_digits_state = STOPWATCH;
    //Mode_Sport = true;
  }

  // Here I had to put it in an if else because a bug appeared when placing the receiver() in the same loop as the animation()
  if(Receiver.available() > 0)
  {    
    // Receives serial data from the front ECU
    Receiver_Data();      
  } 
  
  else
  {
    Leds_State();      // Function that contains all LEDs
    Comfort_Animation();       // Oled animation
  }

  Var_0 = Var;
}

/* Setup functions */
void SetupPacket()
{
  Var.speed      = 0;
  Var.rpm        = 0;
  Var.battery    = 0;
  Var.level      = 0;
  Var.temp_cvt   = 0;
  Var.temp_motor = 0;
  Var.sot        = 0;
}

void Pinconfig()
{
  pinMode(LED_BUILTIN, OUTPUT);

  // Leds
  pinMode(CVTtemp_led, OUTPUT);
  pinMode(Battery_led, OUTPUT);
  pinMode(MotorTemp_led, OUTPUT);

  pinMode(combust_1, OUTPUT);
  pinMode(combust_2, OUTPUT);
  pinMode(combust_3, OUTPUT);
  pinMode(combust_4, OUTPUT);
  pinMode(combust_5, OUTPUT);
    
  // Buttons and potentiometers
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(SWITCH, INPUT);
  pinMode(POTENTIOMETER, INPUT);

  // Tickers
  ticker2Hz.attach(0.5, ticker2HzISR);
  ticker10Hz.attach(0.1, ticker10HzISR);

  // Interruptions
  attachInterrupt(digitalPinToInterrupt(BUTTON), ButtonInterruptISR, RISING);
  //attachInterrupt(digitalPinToInterrupt(SWITCH), SwitchInterruptISR, CHANGE);

  return;
}

/* General Functions */
void Receiver_Data()
{
  uint8_t byteCount = 0;
  
  memset(data_arr, 0, sizeof(Txtmng));  // the array is cleared with the value 0

  while(byteCount < sizeof(Txtmng))
  {
    data_arr[byteCount] = Receiver.read(); //then the data in bits that arrives from the serial is placed in the array
    //Serial.printf("\r\nData = %d => c = %d\r\n", data_arr[byteCount], byteCount);

    //if(byteCount==0) Serial.printf("speed = %d\r\n", Var.speed);
    //if(byteCount==1) Serial.printf("rpm = %d\r\n", Var.rpm);
    //if(byteCount==2) Serial.printf("SOC = %d\r\n", Var.battery);
    //if(byteCount==3) Serial.printf("fuel level = %d\r\n", Var.level);
    //if(byteCount==4) Serial.printf("temp motor = %d\r\n", Var.temp_motor);
    //if(byteCount==5) Serial.printf("temp CVT = %d\r\n", Var.temp_cvt);
    //if(byteCount==6) Serial.printf("SOT = %d\r\n", Var.sot);

    byteCount++;
  }
  
  //Serial.println("\n");
  memcpy(&Var, (Txtmng *)&data_arr, sizeof(Txtmng)); // then the bit values ​​of the array are placed in the Struct Var to be used
}

void Leds_State()
{
  if(boolean10HZ)
  {
    //LedFuel();
    LedEmergency();
    FourDigits();
    SixDigits();
    boolean10HZ = false;
  }
}

void Comfort_Animation()
{
  // Remember that if you draw many objects on the Oled it will become increasingly slower to execute                                                                                                                                                                        
  uint8_t Background = 0x00;
  static char Speed[6];
  static char T_cvt[6];
  static char T_motor[6];

  //debounceSpeed();
  strcpy(Speed, u8x8_u8toa(Var.speed, 2));         // Speed variable that will be displayed on the Oled with 2 digits
  strcpy(T_cvt, u8x8_u8toa(Var.temp_cvt, 3));      // CVT temperature variable that will be displayed on the OLED with 3 digits
  strcpy(T_motor, u8x8_u8toa(Var.temp_motor, 3));  // Motor temperature variable that will be displayed on the OLED with 3 digits

  u8g2.firstPage();
  do {

    u8g2.setFontMode(1);                // Dark background color I don't recommend changing (this is already the best aesthetic combination)
    u8g2.setDrawColor(Background);              
    u8g2.drawBox(0, 0, DisplayWidth+4, DisplayHight);
    u8g2.setDrawColor(2);

    //Display engine temperature and CVT temperature
    u8g2.setFont(u8g2_font_crox2c_tn);          // Font 13 pixels high
    u8g2.drawStr(79, DisplayHight, T_motor);    // Motor value
    u8g2.drawStr(0, DisplayHight, T_cvt);       // CVT value

    u8g2.setFont(u8g2_font_5x8_tf);             //Font
    u8g2.drawStr(10, 50, "CVT");
    u8g2.drawStr(83, 50, "Motor");

    //graus celcius
    u8g2.setFont(u8g2_font_6x12_me);
    u8g2.setCursor(110, DisplayHight);
    u8g2.print(GRAUS_SYM);                      // Degree symbol
    u8g2.setCursor(30, DisplayHight);
    u8g2.print(GRAUS_SYM);
    u8g2.setFont(u8g2_font_7x13_t_cyrillic);   // Font 9 pixels high
    u8g2.drawStr(117, DisplayHight, "C");      // Motor
    u8g2.drawStr(37, DisplayHight, "C");       // CVT
    u8g2.drawStr(86, 33, "Km/h");
    
    if((Var.sot & 0x01)==0x01) 
    {
      // Conectivity symbol
      u8g2.setFont(u8g2_font_open_iconic_all_2x_t);
      u8g2.setCursor(10,18);
      u8g2.print(WIFI_SYM);             
    }

    else if((Var.sot & 0x04)==0x04)
    {
      // Conectivity error symbol 
      u8g2.setFont(u8g2_font_crox4h_tf);
      //u8g2.setCursor(10,18);
      u8g2.drawStr(10-7, 16, "!");
      u8g2.setFont(u8g2_font_open_iconic_all_2x_t);
      u8g2.setCursor(10,18);
      u8g2.print(WIFI_SYM);
    }
      
    else
    {   
      u8g2.drawStr(10, 20, "  "); 
    } 

    u8g2.setFont(u8g2_font_7x13_t_cyrillic);

    if((Var.sot & 0x02)==0x02)
      u8g2.drawStr(6, 33, "4x4");
    else
      u8g2.drawStr(20, 20, "   "); 
        
    // Speedometer
    u8g2.setFont(u8g2_font_inb30_mn);           // Font 27 pixels high
    u8g2.drawStr((DisplayWidth-u8g2.getStrWidth(Speed))/2, 35, Speed);
  
    Battery_box(!Background);

  } while(u8g2.nextPage());
}

// Led functions
void LedFuel()
{
  uint8_t intensity_led_brightness = map(pot, 0, 4095, 1, 255); // Emergency LED brightness control;
  // (*) -> ON
  // ( ) -> OFF
  // (*| ) -> emergency light flashes

  if(Var.level <= 100 && Var.level > 75)
  {
    // (*)(*)(*)(*)(*)
    analogWrite(combust_1, intensity_led_brightness);
    analogWrite(combust_2, intensity_led_brightness);
    analogWrite(combust_3, intensity_led_brightness);
    analogWrite(combust_4, intensity_led_brightness);
    analogWrite(combust_5, intensity_led_brightness);
  }

  else if(Var.level <= 75 && Var.level > 50)
  {
    // ( )(*)(*)(*)(*)
    analogWrite(combust_1, LOW);
    analogWrite(combust_2, intensity_led_brightness);
    analogWrite(combust_3, intensity_led_brightness);
    analogWrite(combust_4, intensity_led_brightness);
    analogWrite(combust_5, intensity_led_brightness); 
  }

  else if(Var.level <= 50 && Var.level > 30)
  {
    // ( )( )(*)(*)(*)
    analogWrite(combust_1, LOW);
    analogWrite(combust_2, LOW);
    analogWrite(combust_3, intensity_led_brightness);
    analogWrite(combust_4, intensity_led_brightness);
    analogWrite(combust_5, intensity_led_brightness);
  }

  else if(Var.level <= 30 && Var.level > 20)
  {
    // ( )( )( )(*)(*)
    analogWrite(combust_1, LOW);
    analogWrite(combust_2, LOW);
    analogWrite(combust_3, LOW);
    analogWrite(combust_4, intensity_led_brightness);
    analogWrite(combust_5, intensity_led_brightness);
  }

  else
  {
    // ( )( )( )( )(*| )
    analogWrite(combust_1, LOW);
    analogWrite(combust_2, LOW);
    analogWrite(combust_3, LOW);
    analogWrite(combust_4, LOW);
    analogWrite(combust_5, intensity_led_brightness*emergency_led_state);
  }
}

void LedEmergency()
{
  uint8_t intensity_led_brightness = map(pot, 0, 4095, 1, 255); // Emergency LED brightness control;
  
  // Control of the Engine temperature emergency light     
  if(Var.temp_motor > Alert_TempMOT)
    analogWrite(MotorTemp_led, emergency_led_state*intensity_led_brightness);
  else 
    digitalWrite(MotorTemp_led, LOW);

  // Control of the CVT temperature emergency light
  if(Var.temp_cvt > Alert_TempCVT) 
    analogWrite(CVTtemp_led, emergency_led_state*intensity_led_brightness);
  else 
    digitalWrite(CVTtemp_led, LOW);

  // Battery emergency light control
  if(Var.battery <= 20)
    analogWrite(Battery_led, emergency_led_state*intensity_led_brightness);
  else
    analogWrite(Battery_led, LOW); //digitalWrite(Battery_led, LOW);
}

// Display Segments Functions
void FourDigits() 
{
  uint8_t intensity_led_brightness = map(pot, 0, 4095, 1, 7); // Emergency LED brightness control;

  // Control of the 4-digit and 7-segment display to display the RPM
  Four.setBrightness(intensity_led_brightness);     // Display brightness control
  //Four.showNumberDecEx(Var.rpm*6, 0, true);       // Value displayed on the display true is for if 0s are to be considered
  Four.showNumberDecEx(Var.rpm, 0, true);
}

void SixDigits()
{
  uint8_t intensity_led_brightness = map(pot, 0, 4095, 1, 7); // Emergency LED brightness control;

  /*
    These are the values ​​of the digit positions and 7 segments, which is the last value placed in the showNumberDec() function

     5   4   3    2   1   0        
     _   _   _    _   _   _
    |_| |_| |_|  |_| |_| |_|
    |_| |_| |_|  |_| |_| |_|

    
  */

  Six.setBrightness(intensity_led_brightness);    // Display brightness control

  // the dots variable is to control the dots on the display
  //I don't really know how it works but I know it turned out the way I wanted these combinations I made

  uint8_t dots;      

  switch(six_digits_state) 
  {  
    case ENDURANCE_TIMER:
      dots = 0b01000000; 

      // Six digits display loop
      Six.showNumberDec(Endurance.hours, dots, true, 2, 0);
      Six.showNumberDec(Endurance.minutes, dots, true, 2, 2);
      Six.showNumberDec(Endurance.seconds, 0, true, 2, 4);

      break;
      
    case STOPWATCH:
      dots = 0b10000000; 

      //if(Button.mode >= 3 && (millis() - Button.lastDebounceTime) < 30000) break; 
      
      Transform_time_current(&Lap);

      //Six.showNumber(0, false, 2, 0);
      //Six.showNumberDec(Volta.minutes, dots, true , 2, 1);
      //Six.showNumberDec(Volta.seconds, dots, true , 2, 3);
      //Six.showNumberDec(0, dots, false, 2, 5);

      //Six.showNumberDec(0, 0, false, 3, 0);
      Six.showNumberDec((int)Lap.minutes/10, 0, false, 3, 0);
      Six.showNumberDec(Lap.minutes%10, dots, true, 1, 3);
      Six.showNumberDec(Lap.seconds, 0, true, 2, 4);

      break;

    case DELTA_CRONOMETRO:
      // Compare between two turns
      //dots = 0b01010100;
  
      //if(ultima_volta.time_lap_ms >= penultima_volta.time_lap_ms)
      //{ 
      //  Delta.time_lap_ms = ultima_volta.time_lap_ms - penultima_volta.time_lap_ms; 
      //  
      //  Six.showNumberDec(penultima_volta.minutes, dots, true , 1, 0);
      //  Six.showNumberDec(penultima_volta.seconds, dots, true , 2, 1);
      //
      //  Delta.seconds = Delta.time_lap_ms%60;

      //  Six.showNumber(0, false, 1, 3);
      //  Six.showNumberDec(Delta.seconds, dots, true , 2, 4);
      //}

      //else if(ultima_volta.time_lap_ms < penultima_volta.time_lap_ms)
      //{  
      //  Delta.time_lap_ms = penultima_volta.time_lap_ms - ultima_volta.time_lap_ms; 

      //  //Six.showNumber(Delta.seconds, true ,3,3);

      //  Six.showNumberDec(penultima_volta.minutes, dots, true, 1, 0);
      //  Six.showNumberDec(penultima_volta.seconds, dots, true , 2, 1);
      //
      //  Delta.seconds = Delta.time_lap_ms%60;
      //  
      //  Six.showString("-", 1, 3, 0);
      //  Six.showNumberDec(Delta.seconds, dots, true, 2, 4);
      //}      
      //six_digits_state = STOPWATCH;
      break;
  }
}

void Transform_time_current(Time *T)
{
  // Transforms time into millis and separates it into seconds, minutes and hours
  uint8_t corrector = ((millis() - T->time_current)/100)%10;

  T->time_lap_ms = (millis() - T->time_current)/1000;

  if(corrector > 4) 
    T->seconds = T->time_lap_ms%60 + 1;
  else 
    T->seconds = T->time_lap_ms%60;
  

  T->hours = T->time_lap_ms/3600;
  T->minutes = (T->time_lap_ms/60)%60;
  T->seconds = T->time_lap_ms%60;
}

void Battery_box(uint8_t cor)
{  
  uint8_t x = 55;
  uint8_t y = 3;

  //battery icon
  u8g2.setDrawColor(cor);
  u8g2.drawBox(x+6, DisplayHight-y-4*(y+1)-4, DisplayWidth - 2*(x+5),2);
  u8g2.drawBox(x-1, DisplayHight-y-4*(y+1)-2, DisplayWidth - 2*x+4, 5*(y+1)+2);
    
  u8g2.setDrawColor(!cor);
  u8g2.drawBox(x, DisplayHight-y-4*(y+1)-1, DisplayWidth - 2*x+2, 5*(y+1));
  
  // Dashed battery icon
  if(Var.battery >= 0)
  {      
    u8g2.setDrawColor(cor);
    
    if(Var.battery >= 20)
    {
      if(Var.battery >= 40)
      {
        if(Var.battery >= 60)
        {
          if(Var.battery >= 80)
          {  
            u8g2.drawBox(x+1, DisplayHight-y-4*(y+1), DisplayWidth - 2*x, y);                
          }
          u8g2.drawBox(x+1, DisplayHight-y-3*(y+1), DisplayWidth - 2*x, y); 
        }
        u8g2.drawBox(x+1, DisplayHight-y-2*(y+1), DisplayWidth - 2*x, y);
      }
      u8g2.drawBox(x+1, DisplayHight-y-(y+1), DisplayWidth - 2*x, y);
    }
    u8g2.drawBox(x+1, DisplayHight-y, DisplayWidth - 2*x, y);
  }
  u8g2.setDrawColor(2);  
}

/* Interrupts routine */
void ButtonInterruptISR()
{
  if(six_digits_state==STOPWATCH)
  {  
    if((millis() - lastDebounceTime) > debounceDelay-50) 
    {
      //Button.mode |= 0x01;
      
      //penultima_volta = ultima_volta;
      //ultima_volta = Volta;
        Lap.seconds = 0;
        Lap.minutes = 0;
        Lap.hours = 0;
        Lap.time_current = millis();
    
      //if(Button.mode >= 2)
      //{
        //six_digits_state = DELTA_CRONOMETRO;
      //} 
      
      //else 
      //{
        six_digits_state = STOPWATCH;
      //}
    } 

    lastDebounceTime = millis();   
  }
}

/*void SwitchInterruptISR()
{
  if(millis() - Switch.lastDebounceTime > debounceDelay/2)
  {
    Switch.lastButtonState = true;
  }

  Switch.lastDebounceTime = millis();
}*/

void ticker2HzISR() 
{
  emergency_led_state = !emergency_led_state;

  /* This flag is activated after the ticker 0.5 is called 2 times (2*0.5 == 1) */
  emergency_led_state==true ? boolean1HZ |= 0x01 : 0;

  // Endurance Timer
  if(boolean1HZ)
  {
    Endurance.seconds++;
    
    if(Endurance.seconds >= 60)
    {
      Endurance.seconds = 0;
      
      Endurance.minutes++;

      if(Endurance.minutes > 60)
      {

        Endurance.minutes = 0;
  
        Endurance.hours++;
            
        if(Endurance.hours > 24)
        {
          Endurance.hours = 0;
        }
      }
    }
    boolean1HZ &= ~0x01;
  }
}

void ticker10HzISR()
{
  boolean10HZ = true;
}
