#include <LCD4Bit_mod.h>
#include "Arduino.h"
extern "C" {
  #include <string.h> //needed for strlen()
}

LCD4Bit_mod lcd = LCD4Bit_mod(2);

int button_pin = 0;
int backlight_pin = 10;
String serial_data = "";

//Key message
char msgs[5][15] = {"RIGHT", 
                    "UP", 
                    "DOWN", 
                    "LEFT", 
                    "SELECT" };
int  adc_key_val[5] ={30, 150, 360, 535, 760 };
int NUM_KEYS = 5;
int adc_key_in;
int key=-1;
int oldkey=-1;


void setup() {
  Serial.begin(9600);
  Serial.write("STARTING...\n");

  pinMode(10, OUTPUT);   // select backlight

  lcd.init();
  lcd.clear();
//  lcd.commandWrite(0b00001111);
  lcd.cursorTo(1, 0);  // line 1
  lcd.printIn("Serial to LCD");

  
  Serial.write("OK\n");
}




void loop() {
  
  // check serial communication 
  comm();
  
  // read key pad 
  read_keys();
}


void comm() {
  // read the serial mesage
  while (Serial.available() > 0)
  {
    char recieved = Serial.read();
    serial_data += recieved; 

    if (recieved == '\n')
    {
      if(serial_data == "/1\n") toLine(1);
      else if(serial_data == "/2\n") toLine(2);
      else if(serial_data == "/cls\n") ClearScreen();
      else if(serial_data == "/backlight on\n") backlight(1);
      else if(serial_data == "/backlight off\n") backlight(0);
      else Shout(serial_data);

      serial_data = "";
    }
  }
}

// set the position of the line cursor and clean it
void toLine(int pos) {
  lcd.cursorTo(pos, 0);// clear the line
  lcd.printIn("                ");
  
  lcd.cursorTo(pos, 0);
  
  Serial.print("LINE ");
  Serial.print(pos);
  Serial.println(" OK");
}

// Clear all in LCD Screen
void ClearScreen() {
  lcd.clear(); 
  Serial.println("OK");
}

void Shout(String message) {
  char caption[50];
  String lcd_message = message;
  
  lcd_message.trim();
  lcd_message.toCharArray(caption, 50);
  
  lcd.printIn(caption);
  
  Serial.print("RX \"");
  Serial.print(lcd_message);
  Serial.print("\"");
  Serial.println("OK");
}


// switch the backlight
void backlight(int mode){
  if(mode){
    digitalWrite(backlight_pin, HIGH);
    Serial.println("BACKLIGHT ON");
  } else {
    digitalWrite(backlight_pin, LOW);
    Serial.println("BACKLIGHT OFF");
  }
  Serial.println("OK");
}

void send_comm(char* message){
  if(message>0) {
    Serial.println(message);
  }
}


void read_keys(){
  
  adc_key_in = analogRead(0);    // read the value from the sensor  
  key = get_key(adc_key_in);		        // convert into key press	
  
  if (key != oldkey)				    // if keypress is detected
  {
    delay(50);		// wait for debounce time
    adc_key_in = analogRead(0);    // read the value from the sensor  
    key = get_key(adc_key_in);		        // convert into key press
    if (key != oldkey)				
    {			
      oldkey = key;
      if (key >=0){
        send_comm(msgs[key]);
      }
    }
  }
}

int get_key(unsigned int input) {
  int k;
  
  for (k = 0; k < NUM_KEYS; k++) {
    if (input < adc_key_val[k]) {         
      return k;
    }
  }
  
  if (k >= NUM_KEYS) k = -1;     // No valid key pressed
  return k;
}
