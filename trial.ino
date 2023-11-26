// Libraries for Keypad, Display and Fingerprint scanner
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <Adafruit_Fingerprint.h>    
#include <SoftwareSerial.h>
#include <Servo.h>

#define Password_Length 5
#define En_pin 8
#define Rs_pin 6
#define D4_pin 2
#define D5_pin 3
#define D6_pin 4
#define D7_pin 5

// To enter a new pin, press "A", to continue press "B"
// After entering new pin, the system asks for new fingerprint. Place the finger on the scanner and follow the instructions shown on the display. 


LiquidCrystal lcd(Rs_pin,En_pin,D4_pin,D5_pin,D6_pin,D7_pin); 
uint8_t id;
int pos = 0;
SoftwareSerial mySerial(9, 10); 
char Data[Password_Length];
char Master[Password_Length] = "1234";
byte data_count = 0, master_count = 0;
Servo myservo;
bool Pass_is_good;
bool door = false;
char customKey;


/*---preparing keypad---*/
String Names[] = { "suraj", "prajwal", "SURAJ","other","other","other"};
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {12, 13, 19, 18};
byte colPins[COLS] = {17, 16, 15, 14};

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial); 
Keypad customKeypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);


/*--- Main Action ---*/
void setup()
{
  finger.begin(57600);  
  finger.getTemplateCount();
  Serial.begin(9600);
  myservo.attach(11, 2000, 2400);
  pinMode(14,INPUT);
  pinMode(15,INPUT);
  pinMode(16,INPUT);
  pinMode(17,INPUT);
  pinMode(18,INPUT);
  pinMode(19,INPUT);
  pinMode(13,INPUT);
  pinMode(12,INPUT);
  pinMode(7,OUTPUT);
  //digitalWrite(7,HIGH);
  ServoClose();
  lcd.begin(16, 2);
  lcd.print("Protected Door");
  loading("Loading");
  lcd.clear();
  lcd.print("A = NEW PIN");
  lcd.setCursor(0, 1);
  lcd.print("B = OPEN DOOR");
  customKey = customKeypad.getKey();
  while (!customKey){customKey = customKeypad.getKey();}
  Serial.print(customKey);
  if (customKey == 'A')
  {
      lcd.clear();
      getPin();
  }else if (customKey == 'B') {}
  else{
    lcd.clear();
    lcd.print("INVALID KEY");
    delay(1000);
    setup();
  }

  lcd.clear();
}

uint8_t readnumber(void) {
  lcd.clear();
  lcd.print("ENTER ID");
  lcd.setCursor(0, 1);
  lcd.print("2 to 5");
  uint8_t num = 0;
  num = customKeypad.getKey();
  while(!num){
    num = customKeypad.getKey();
  }
  return (num - 48);
}



void getFinger()
{

  if (finger.verifyPassword()) {
    Serial.print("Found fingerprint sensor!");
  } else {
    Serial.print("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  // Serial.println(F("Reading sensor parameters"));
  // finger.getParameters();
  // Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  // Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  // Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  // Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  // Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  // Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  // Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);
  // Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
  id = readnumber();
  Serial.println("ID");
  Serial.println(id);  
  if (id == 0) {// ID #0 not allowed, try again!
     return;
  }
   Serial.print("Enrolling ID #");
  // Serial.println(id);

  while (!  getFingerprintEnroll() );
}

uint8_t getFingerprintEnroll(){

  int p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    lcd.clear();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      lcd.print("READY TO SCAN");
      //Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcd.clear();
      lcd.print("ERROR");
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      lcd.clear();
      lcd.print("ERROR");
      Serial.println("Imaging error");
      break;
    default:
      lcd.clear();
      lcd.print("ERROR");
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  p = -1;
  lcd.clear();
  lcd.print("RE-PLACE FINGER");
  Serial.println("Place same finger");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");  
  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  lcd.clear();
  lcd.print("PRINT STORED");
  delay(1000);
  lcd.clear();
  lcd.print("REMOVE FINGER");
  Serial.println("Remove finger");
  delay(2000);
  return true;
}

void loop()
{
  if (door == true)
  {
    customKey = customKeypad.getKey();
    if (customKey == '#')
    {
      lcd.clear();
      ServoClose();
      digitalWrite(8,HIGH);
      lcd.print("Door is closed");
      delay(3000);
      door = false;
    }
  }
  else
    Open();
    //getFingerprintIDez();

}

void getPin(){
  lcd.setCursor(0, 0);
  lcd.print("Enter Pin");
  int count=0;
  customKey = customKeypad.getKey();
  while (!customKey){
    customKey = customKeypad.getKey();
  }
  while (count < 4)
  {
    Master[count] = customKey;
    lcd.setCursor(count, 1);
    lcd.print(Master[count]);
    customKey = customKeypad.getKey();
    while (!customKey && count < 3){
      customKey = customKeypad.getKey();      
    }
    count++;
  }
  lcd.clear();
  lcd.print("PIN TAKEN");
  getFinger();  
}

void loading (char msg[]) {
  lcd.setCursor(0, 1);
  lcd.print(msg);

  for (int i = 0; i < 3; i++) {
    delay(1000);
    lcd.print(".");
  }
}

void clearData()
{
  while (data_count != 0)
  { 
    Data[data_count--] = 0;
  }
  return;
}
void ServoClose()
{
  for (pos = 90; pos >= 0; pos -= 10) { 
    myservo.write(pos);
  }
}

void ServoOpen()
{
  for (pos = 0; pos <= 90; pos += 10) {
    myservo.write(pos);  
  }
}
int getFingerprintIDez() {
  uint8_t p = finger.getImage();  
      //Image scanning
  while (p == FINGERPRINT_NOFINGER){
    //Serial.println("No Finger");
    p = finger.getImage();
    delay(100);
  }
    //Serial.println("Finger");
  
  //lcd.clear();
  
  if (p != FINGERPRINT_OK)  {
    lcd.clear();
    lcd.println("Error 1");
    return -1;
  }  

  p = finger.image2Tz();               //Converting
  if (p != FINGERPRINT_OK)   {
    lcd.clear();
    lcd.println("Error 2");
    return -1;
  }  

  p = finger.fingerFastSearch();     //Looking for matches in the internal memory
  if (p != FINGERPRINT_OK)   {
    lcd.clear();
    lcd.println("Error 3");
    return -1;
  }  
  
  // found a match!
  lcd.clear();
  lcd.print("Found match !");
  lcd.setCursor(0,1);
  lcd.print(finger.fingerID);           //If a match is found it shows the ID of the finger
  lcd.setCursor(2,1);
  lcd.print(Names[finger.fingerID-1]); //Then the name we gave to it and the -1 is to remove the shift
  delay(3000);
  lcd.clear();
  ServoOpen();
  lcd.print(" Door is Open ");
  digitalWrite(7,LOW);
  door = true;
  delay(5000);
  loading("Waiting");
  lcd.clear();
  digitalWrite(7,HIGH);
  lcd.print(" Time is up! ");
  delay(1000);
  ServoClose();
  door = false;
  return finger.fingerID; 
}
void Open()
{
  lcd.setCursor(0, 0);
  lcd.print("Enter Password");
  
  customKey = customKeypad.getKey();
  if (customKey)
  {
    Data[data_count] = customKey;
    lcd.setCursor(data_count, 1);
    lcd.print(Data[data_count]);
    data_count++;
  }

  if (data_count == Password_Length - 1)
  {
    if (!strcmp(Data, Master))
    {
      lcd.clear();
      lcd.println("READY TO SCAN");
      //ServoOpen();
      getFingerprintIDez();
      
    }
    else
    {
      lcd.clear();
      lcd.print(" Wrong Password ");
      door = false;
    }
    delay(1000);
    lcd.clear();
    clearData();
  }
}
