#include <SoftwareSerial.h>
#include <Keypad.h>
#include <Servo.h>
SoftwareSerial SIM900A(14, 15);
Servo servo;

int servoPin = 12;
bool lock = true;
int buzzer = 10;
int wrongCount = 0;

const char* password = "1234"; //create a password
int passPosition = 0; //keypad position
int state ; //bluetooth password 
int ledPin = 11;

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {2, 3, 4, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 7, 8, 9}; //connect to the column pinouts of the keypad

Keypad myKeypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


void setup()
{
  servo.attach(servoPin); // Steeing servo motor
  SIM900A.begin(9600); // Setting the baud rate of GSM Module

  servo.write(0); // Init servo position

  pinMode(10, OUTPUT); // Buzzer setup
  noTone(buzzer);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  Serial.begin(9600); // Setting the baud rate of Serial Monitor (Arduino)
  Serial.println("All setup done\n");

  delay(100);
}


void loop()
{
  checkPassword();

  delay(100);
}


void checkPassword() {

  if (Serial.available() > 0) {
    state = Serial.read();
    Serial.write(state);
    Serial.println();
  }

  char whichKey = myKeypad.getKey(); //define which key is pressed with getKey
  
  // Analysing pressed value
  if (whichKey == password [passPosition]) {
    Serial.println(whichKey);
    passPosition ++;
  }
  else if (whichKey != password [passPosition] && whichKey != '\0') {
    passPosition = 0;
    BuzzerFunction(2);
    wrongCount++ ;
    Serial.println(whichKey);
  }

  // If tried 3 unsuccessful attempts
  if (wrongCount == 3 ) {
    BuzzerFunction(5);
    wrongCount = 0;
  }

  // Matched password
  if (passPosition == 4 || state == 'a') {
    passPosition = 0;
    Serial.println("Password Matched");
    delay(1000);
    
    BuzzerFunction(1); // 1 Means successfully locked/unlocked
    
    if (lock){
      SendMessage();
    }
    servoMotor();
    delay(500);
    digitalWrite(ledPin, LOW);
  }
  
}


void SendMessage()
{
  Serial.println ("\nSending Message");
  SIM900A.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);
  SIM900A.println("AT+CMGS=\"+8801534312346\"\r"); //Mobile phone number to send message
  delay(1000);
  SIM900A.println("Cycle unlocked.");// Messsage content
  Serial.println ("Cycle unlocked.");

  delay(100);
  SIM900A.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
  Serial.println ("Message has been sent");
  Serial.println();
}


void servoMotor()
{
  if (lock) {
    digitalWrite(ledPin, HIGH);
    servo.write(90);
    lock = false;
  }

   else if(state == 'b' || lock == false){
    digitalWrite(ledPin, HIGH);
    servo.write(0);
    lock = true;
  }
}

// Done
void BuzzerFunction(int i) {
  for (int j = 0; j < i; j++) {
    tone(buzzer, HIGH);
    delay(500);
    noTone(buzzer);
    delay(500);
  }
  
  if (i == 5){
    Serial.println("Wait for 5 sec");
    FeedBackMessage();
    delay(5000);
  }
}


void FeedBackMessage()
{
  Serial.println ("\nSending Message");
  SIM900A.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);
  SIM900A.println("AT+CMGS=\"+8801534312346\"\r"); //Mobile phone number to send message
  delay(1000);
  SIM900A.println("Wrong Password attempt 3 times.");// Messsage content
  Serial.println ("Someone try to unlocke you device.");

  delay(100);
  SIM900A.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
  Serial.println ("Message has been sent");
  Serial.println();
}


/*void LEDPin(int state){
    if(state == 'a'){
    digitalWrite(ledPin,  HIGH);
    }
    else if(state == 'b'){
    digitalWrite(ledPin,  LOW);
  }
}*/
