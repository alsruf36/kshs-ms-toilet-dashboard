#include <Wire.h>
#include <Adafruit_INA219.h>
#include <SoftwareSerial.h>

#define WaterLevelSensor1 5
#define WaterLevelSensor2 6
#define RX 8
#define TX 9
#define ID 5

Adafruit_INA219 ina219;

String AP ="kwsh"; 
String PASS ="khsh9700#"; 
String HOST = "158.247.231.126";
String PORT = "8088";

int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
String Data ="";
SoftwareSerial esp8266(RX,TX); 

void setup() {
  pinMode(WaterLevelSensor1, INPUT);
  pinMode(WaterLevelSensor2, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(9600);
  
  uint32_t currentFrequency;
  ina219.begin();
  esp8266.begin(9600);

  sendCommand("AT",5,"OK",false);
  ConnectToWifi();
}

float getVoltage(){
  float shuntvoltage = 0;

  float busvoltage = 0;

  float current_mA = 0;

  float loadvoltage = 0;

  float power_mW = 0;



  shuntvoltage = ina219.getShuntVoltage_mV();

  busvoltage = ina219.getBusVoltage_V();

  current_mA = ina219.getCurrent_mA();

  power_mW = ina219.getPower_mW();

  loadvoltage = busvoltage + (shuntvoltage / 1000);

  return loadvoltage;
}

void loop() {
  int s1 = digitalRead(WaterLevelSensor1);
  int s2 = digitalRead(WaterLevelSensor2);
  float v = getVoltage();

  Serial.print("D5 : ");
  Serial.print(s1);
  Serial.print(" / D6 : ");
  Serial.print(s2); 
  Serial.print(" / Current : ");
  Serial.println(v);

  SendData(ID, s2);
  
  delay(0.1);
}

void SendData(int id, int blocked){
  String blocked_str;
  
  if(blocked == 1) blocked_str = "True";
  else blocked_str = "False";
  
  String getData=
  "POST /toilets/?id=" + String(id) + "&flush=" + blocked_str + " HTTP/1.1\n" +
  "Host: " + HOST + String(":") + PORT + "\n" +
  "Content-Length: 0\n";

  sendCommand("AT+CIPMUX=1",1,"OK",false);
  sendCommand("AT+CIPSTART=4,\"TCP\",\""+ HOST +"\","+ PORT,2,"OK",false);
  sendCommand("AT+CIPSEND=4," +String(getData.length()+4),2,">",false);
  sendCommand(getData,1,"OK",true);
  sendCommand("AT+CIPCLOSE=0",1,"OK",false);
}

bool ConnectToWifi(){
  for (int a=0; a<15; a++)
  {
    sendCommand("AT",5,"OK",false);
    sendCommand("AT+CWMODE=1",5,"OK",false);
    boolean isConnected = sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK",false);
    if(isConnected)
    {
      return true;
    }
  }
}

bool sendCommand(String command, int maxTime, char readReplay[],boolean isGetData) {
  boolean result=false;
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);
    if(esp8266.find(readReplay))//ok
    {   
      if(isGetData)
      {      
        if(esp8266.find(readReplay))
        {
          Serial.println("Success : Request is taken from the server");
        }
        while(esp8266.available())
        {
            char character = esp8266.read();
            Data.concat(character); 
            if (character == '\n')
             {
             Serial.print("Received: ");
             Serial.println(Data);
             delay(50);
             Data = "";
        } 
        }         
      }
      result = true;
      break;
    }
    countTimeCommand++;
  }
  
  if(result == true)
  {
    Serial.println("Success");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(result == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
  return result;
 }
