//Variables for the relays and startup button.
int BatRelay = A4; //Pin for the activation of the relay that closes the giant battery relay. (Relay 1)
int SparkRelay = A5;                                                                                                                                                                                                                                                                                                                                                                                                                                                                         ; // Pin for the activation of the Antispark (AS) Relay.  (Relay 2, AS)
int MainSwitch = 4; // Pin for the main ON/OFF switch. (toggle button in my case, but it works like a switch).
bool MainSwitchState = LOW;
bool ChargeState = LOW; //This info records the state of the charge in the capacitors. If an anti spark routine has been performed, it is HIGH. If it is low, there needs to be an anti spark routine performed.


//Variables for thermistor I use NTC 3950 for 3D printers and therlike. Source: https://learn.adafruit.com/thermistor/using-a-thermistor

#define THERMISTORPIN1 A0         // which analog pin to connect the thermistor leg 1 ESC
#define THERMISTORPIN2 A1         // which analog pin to connect the thermistor leg 1 MOTOR
#define THERMISTORPIN3 A2         // which analog pin to connect the thermistor leg 1 Battery
#define THERMISTORPIN4 A3         // which analog pin to connect the thermistor leg 1 Con.

#define THERMISTORNOMINAL 100000      // resistance at 25 degrees C You bought a 100k Termistor, so you enter 100,000
#define TEMPERATURENOMINAL 24   // temp. for nominal resistance (almost always 25 C)
#define NUMSAMPLES1 3 // how many samples to take and average, more takes longer but is more 'smooth' 5
#define NUMSAMPLES2 3 // how many samples to take and average, more takes longer but is more 'smooth' 
#define NUMSAMPLES3 3 // how many samples to take and average, more takes longer but is more 'smooth' 
#define NUMSAMPLES4 3 // how many samples to take and average, more takes longer but is more 'smooth' 
#define BCOEFFICIENT 3950 // The beta coefficient of the thermistor (usually 3000-4000)  You find it in the name or data sheet
#define SERIESRESISTOR 100000    // the value of the 'other' resistor This resistor has to be placed on the proto board and should be close in size to that of your thermistor.
uint16_t samples1[NUMSAMPLES1]; //Some variables for calculations
uint16_t samples2[NUMSAMPLES2]; //Some variables for calculations
uint16_t samples3[NUMSAMPLES3]; //Some variables for calculations
uint16_t samples4[NUMSAMPLES4]; //Some variables for calculations

uint8_t i;
float average1;
float average2;
float average3;
float average4;
uint32_t LastTempCheckTime = 0;


//Variables for 1602 Display from https://howtomechatronics.com/tutorials/arduino/lcd-tutorial/
#include <LiquidCrystal.h> // includes the LiquidCrystal Library 
LiquidCrystal lcd(5, 6, 7, 8, 9, 10); // Creates an LC object. Parameters: (rs pin 5, enable pin 6, d4 pin 7, d5 pin 8, d6 pin9, //F:xxwiring change herexxx //F: changed this back to pin 10 in order to free pin 1 for serial comminications on arduino nano TX == D1 and RX == D0, pay attention to pinout


//F: the TRMH20 lib for nrf24 appears to be used. It compiles fine with this lib, but I can not test this due to total lack of nrf24 hardware.
//Stuff for Wireless Communications Adapted from user MaB from efoil.builders. Thank you!
//Good Tutorial for RF24: https://howtomechatronics.com/tutorials/arduino/arduino-wireless-communication-nrf24l01-tutorial/
//#include <Servo.h> //Using servo library to control ESC //F:we should not need this anymore, as we use serial communication to the vesc for control now.
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(2, 3); // CE, CSN They are connected to pin D2 and D3 respectively.
int gotByte = 3;
const uint64_t pipe = 0xE8E8F0F0E1LL;
//bool recievedData = false;
uint32_t lastTimeReceived = 0;
int timeoutMax = 1000;
//Servo esc; //Creating a servo class that is called esc, because ESCs are controlled just like Servos. //F:we should not need this anymore, as we use serial communication to the vesc for control now.

//F: patched all of this out, again this functionality shall be provided via serial comm now.
//int escpin = 10; // esc pwm pin This is the only one i had left on my board...
//int timer = 0; // esc shutoff
//int val = 0;
int text; //F: this is used to receive data from the nrf24 wireless device, reactivated this global var 
////Min and max pulse
//int minPulseRate = 1500;
//int maxPulseRate = 2000;
//#
void setup()
{
  lcd.clear();
  ///Serial.begin(9600); //needed for the seial monitor Why is this disabled? I needed the D1 pin for the display, because I ran out of pins and needed D10 for PWM for the signal to the ESC. So I use D1 which is ususually used for serial communications, If you want to debug this code, make this line live and change the pin used for the ESC . 
  pinMode(BatRelay, OUTPUT); //Defines the pin as an output
  pinMode(SparkRelay, OUTPUT); //Defines the pin as an output
  digitalWrite(SparkRelay, HIGH);
  digitalWrite(BatRelay, HIGH);
  pinMode (MainSwitch, INPUT); //Defines the pin as an input
  lcd.begin(16, 2); // Initializes the interface to the LCD screen, and specifies the dimensions (width and height) of the display

  Serial.println("Initial Setup Completed"); //Displays this message in the Seial Monitor.
  lcd.print("Initial Setup"); // Prints "Initial Setup Completed" on the LCD
  lcd.setCursor(0, 1);
  lcd.print("Completed");
  delay (1000);


  //Setup stuff for wireless communications adapted from user MaB from efoil.builders.
  //F: patching all classic esc communication via PWM out of the arduino firmware.
    //esc.attach(escpin, minPulseRate, maxPulseRate);
    //esc.write(0); // init esc with 0 value
  radio.begin();
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(111);
  radio.setAutoAck(true);
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.openReadingPipe(1, pipe);
  radio.startListening();
  radio.setRetries(5, 5);
}

void loop()
{
  MainSwitchState = digitalRead(MainSwitch); //This stuff will check if the main switch on the board is tuned on. If it is, it will perform anti spark routine, but only if the anti spark was not already performed.


  if (MainSwitchState == HIGH && ChargeState == LOW) //Checks if the Start Switch is ON or OFF (High means ON) & if the anti spark state is low, so the capacitors are not charged.
  {
    Anti_Spark_Routine();// Calls (runs) the anti spark routine in the second tab.
    Serial.println("Calling Anti Spark Routine");
    Serial.println(MainSwitchState); //Displays this message in the Seial Monitor.
  }



  if (MainSwitchState == LOW && ChargeState == HIGH)
  {
    digitalWrite(BatRelay, HIGH); //This opens the StartRelay which in turn opens the battery relay and disconnects the batteries.
    digitalWrite(SparkRelay, HIGH); //This opens the AS_Relay just to make sure.
    ChargeState = LOW; //Resets the anti spark relay so on the next loop, a new anti spark routine is performed.
    Serial.println("Battery is disconnected."); //Displays this message in the Seial Monitor.
    Serial.println(MainSwitchState); //Displays this message in the Seial Monitor.
    Serial.println(ChargeState); //Displays this message in the Seial Monitor.

  }
  if ((millis() - LastTempCheckTime) >= 2000)  //if the current time from Arduiono startup, called millis(), minus the Last Check Temp Time is bigger or equal than 2 second, read the temperature sensors.
  {
    Read_Temperature(); //performs a temperature reading as found in the third tab.
  }


  // THE FOLLOWING IS FOR WIRELESS COMMUNICATION:

  // If transmission is available
  if (radio.available())
  {
    // The next time a transmission is received on pipe, the data in gotByte will be sent back in the acknowledgement (this could later be changed to data from VESC!)
    radio.writeAckPayload(pipe, &gotByte, sizeof(gotByte));
    radio.read(&text, sizeof(text)); //orignal
    //recievedData = true;

    //if (recievedData == true)

    // A speed is received from the transmitter (remote).

    lastTimeReceived = millis();
    //recievedData = false;

    // Write the PWM signal to the ESC (0-255).
    //esc.write(int(text)); //F:patched out classic PWM esc/vesc control
    Serial.print("Throtle: ");
    Serial.println(text); //original
    lcd.setCursor(0, 0);
    lcd.print(text);
  }

  else if ((millis() - lastTimeReceived) > timeoutMax)
  {
    // No speed is received within the timeout limit.
    //esc.write(0); //F: patching out PWM esc/vesc control
    Serial.println("Connection Lost");
    Serial.println((millis() - lastTimeReceived));
  }

}
