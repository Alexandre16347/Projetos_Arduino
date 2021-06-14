//Libraries
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);  //LCD i2c adress sometimes 0x3f or 0x27

//Icons
uint8_t Battery[8]  = {0x0E, 0x1B, 0x11, 0x11, 0x1F, 0x1F, 0x1F, 0x1F};
uint8_t Panel[8]  = {0x1F, 0x15, 0x1F, 0x15, 0x1F, 0x15, 0x1F, 0x00};
uint8_t Pwm[8]  = {0x1D, 0x15, 0x15, 0x15, 0x15,0x15, 0x15, 0x17};
uint8_t Flash[8]  = {0x01, 0x02, 0x04, 0x1F, 0x1F, 0x02, 0x04, 0x08};


//Constants
#define bulk_voltage_max 14.5
#define bulk_voltage_min 13
#define absorption_voltage 14.7
#define float_voltage_max 13
#define battery_min_voltage 10
#define solar_min_voltage 19
#define charging_current 2.0
#define absorption_max_current 2.0
#define absorption_min_current 0.1
#define float_voltage_min 13.2
#define float_voltage 13.4
#define float_max_current 0.12
#define LCD_refresh_rate 1000
byte BULK = 0;        //Give values to each mode
byte ABSORPTION = 1;
byte FLOAT = 2;
byte mode = 0;        //We start with mode 0 BULK

//Inputs
#define solar_voltage_in A1
#define solar_current_in A0
#define battery_voltage_in A2

//Outputs
#define PWM_out 10
#define load_enable 2

//Variables
float bat_voltage = 0;
int pwm_value = 0;
float solar_current = 0;
float current_factor = 0.185;       //Value defined by manufacturer ACS712 5A
float solar_voltage = 0;
float solar_power = 0;
String load_status = "OFF";
int pwm_percentage = 0;
unsigned int before_millis = 0;
unsigned int now_millis = 0;
String mode_str = "BULK";


void setup(){
  pinMode(solar_voltage_in,INPUT);    //Set pins as inputs
  pinMode(solar_current_in,INPUT);
  pinMode(battery_voltage_in,INPUT);
  
  pinMode(PWM_out,OUTPUT);            //Set pins as OUTPUTS
  digitalWrite(PWM_out,LOW);          //Set PWM to LOW so MSOFET is off
  pinMode(load_enable,OUTPUT);
  digitalWrite(load_enable,LOW);      //Start with the relay turned off
  TCCR1B = TCCR1B & B11111000 | B00000001; //timer 1 PWM frequency of 31372.55 Hz
  Serial.begin(9600);

  lcd.init();                 //Init the LCD
  lcd.backlight();            //Activate backlight  

  lcd.createChar(0, Battery);
  lcd.createChar(1, Panel);
  lcd.createChar(2, Pwm);
  lcd.createChar(3, Flash);
  before_millis = millis;     //Used for LCD refresh rate
}

void loop(){
  solar_voltage = get_solar_voltage(15);
  bat_voltage =   get_battery_voltage(15);
  solar_current = get_solar_current(15);
  solar_power = bat_voltage * solar_current; 
  pwm_percentage = map(pwm_value,0,255,0,100);

  now_millis = millis();
  if(now_millis - before_millis > LCD_refresh_rate)
  {
    before_millis = now_millis;
    lcd.clear();
    lcd.setCursor(0,0);               //Column 0 row 0
    lcd.write(1);                     //Panel icon
    lcd.print(" ");                   //Empty space
    lcd.print(solar_voltage,2);       //Soalr voltage 
    lcd.print("V");                   //Volts
    lcd.print("    ");                //Empty spaces
    lcd.write(0);                     //Battery icon
    lcd.print(" ");                   //Empty space
    lcd.print(bat_voltage,2);         //Battery voltsge
    lcd.print("V");                   //Volts
  
    lcd.setCursor(0,1);               //Column 0 row 1
    lcd.print("  ");                  //Empty spaces
    lcd.print(solar_current,2);       //Solar current
    lcd.print("A");                   //Ampers
    lcd.print("     LOAD ");           //Print LOAD
    lcd.print(load_status);           //LOAD status
    
    lcd.setCursor(0,2);               //Column 0 row 2
    lcd.print("  ");                  //Empty spaces
    lcd.print(solar_power,2);         //Solar power
    lcd.print("W");                   //Watts
    lcd.print("     PWM ");           //Print PWM
    lcd.print(pwm_percentage);        //PWM value
    lcd.print("%");                   //Percentage

    lcd.setCursor(0,3);               //Column 0 row 3
    lcd.print(mode_str);              //Print the mode
    
  }
  
  if(bat_voltage < battery_min_voltage){
    digitalWrite(load_enable,LOW);        //We DISABLE the load if battery is undervoltage
    load_status  = "OFF";
  }
  else{
    digitalWrite(load_enable,HIGH);       //We ENABLE the load if battery charged
    load_status  = "ON";
  }

  ///////////////////////////FLOAT///////////////////////////
  ///////////////////////////////////////////////////////////
  if(mode == FLOAT){
    if(bat_voltage < float_voltage_min){
      mode = BULK;
      mode_str = "BULK"; 
    }
    
    else{
      if(solar_current > float_max_current){    //If we exceed max current value, we change mode
        mode = BULK;
        mode_str = "BULK"; 
      }//End if > 
  
      else{
        if(bat_voltage > float_voltage){
          pwm_value--;
          pwm_value = constrain(pwm_value,0,254);
        }
        
        else {
          pwm_value++;
          pwm_value = constrain(pwm_value,0,254);
        }        
      }//End else > float_max_current
      
      analogWrite(PWM_out,pwm_value);
    }
  }//END of mode == FLOAT



  //Bulk/Absorption
  else{
    if(bat_voltage < bulk_voltage_min){
      mode = BULK;
      mode_str = "BULK";   
    }
    else if(bat_voltage > bulk_voltage_max){
      mode_str = "ABSORPTION"; 
      mode = ABSORPTION;
    }
  
    ////////////////////////////BULK///////////////////////////
    ///////////////////////////////////////////////////////////
    if(mode == BULK){
      if(solar_current > charging_current){
        pwm_value--;
        pwm_value = constrain(pwm_value,0,254);
      }
      
      else {
        pwm_value++;
        pwm_value = constrain(pwm_value,0,254);
      }
      analogWrite(PWM_out,pwm_value);
    }//End of mode == BULK
  
  
  
    /////////////////////////ABSORPTION/////////////////////////
    ///////////////////////////////////////////////////////////
    if(mode == ABSORPTION){
      if(solar_current > absorption_max_current){    //If we exceed max current value, we reduce duty cycle
        pwm_value--;
        pwm_value = constrain(pwm_value,0,254);
      }//End if > absorption_max_current
  
      else{
        if(bat_voltage > absorption_voltage){
          pwm_value++;
          pwm_value = constrain(pwm_value,0,254);
        }
        
        else {
          pwm_value--;
          pwm_value = constrain(pwm_value,0,254);
        }
        
        if(solar_current < absorption_min_current){
          mode = FLOAT;
          mode_str = "FLOAT"; 
        }
      }//End else > absorption_max_current
      
      analogWrite(PWM_out,pwm_value);
    }// End of mode == absorption_max_current
    
  }//END of else mode == FLOAT

  
  

  



  
  //Serial.println(bat_voltage);  
}//End void loop





/////////////////////////FUNCTIONS/////////////////////////
///////////////////////////////////////////////////////////
float get_solar_voltage(int n_samples)
{
  float voltage = 0;
  for(int i=0; i < n_samples; i++)
  {
    voltage += (analogRead(solar_voltage_in) * (5.0 / 1023.0) * 8.0); 
  }
  voltage = voltage/n_samples;
  if(voltage < 0){voltage = 0;}
  return(voltage);
}



float get_battery_voltage(int n_samples)
{
  float voltage = 0;
  for(int i=0; i < n_samples; i++)
  {
    voltage += (analogRead(battery_voltage_in) * (5.0 / 1023.0) * 7.85);     
  }
  voltage = voltage/n_samples;
  if(voltage < 0){voltage = 0;}
  return(voltage);
}


float get_solar_current(int n_samples)
{
  float Sensor_voltage;
  float current =0;
  for(int i=0; i < n_samples; i++)
  {
    Sensor_voltage = analogRead(solar_current_in) * (5.0 / 1023.0);
    current = current + (Sensor_voltage-2.5)/current_factor; 
  }
  current = current/n_samples;
  if(current < 0){current = 0;}
  return(current);
}
