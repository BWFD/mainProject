#include<WiFi.h>
#include<BluetoothSerial.h>

const char*ssidAP="ESP32_WiFi";
const char*passwordAP="3B017126";

uint8_t slaveAdd[6] ={0x98,0xD3,0x61,0xF6,0xE1,0xB6};
String Name = "ESP32_Master";
bool connect;
BluetoothSerial BT;

WiFiServer server(80);
String Fullcmd="";
bool nanoIsWorking=false;


String getValue(String,char,int);
void BTListener();
void BTActive();

void setup() {
    Serial.begin(115200);
    WiFi.softAP(ssidAP,passwordAP);
    delay(500);
    server.begin();
}

void loop() {
    WiFiClient client = server.available();
    if(client) {
      Serial.println("Client connect!");
      if(client.available()) {
        Fullcmd=client.readString();
        Serial.print("-------");
        Serial.println("***\n"+Fullcmd);
      }
    }
    
    if(Fullcmd!="") {
      BTActive();
      if(Fullcmd=="test_connect") {
        String res = "True,";
        if(nanoIsWorking) res+"True";
        else  res+"False";
        client.print(res);
      }
      //client.stop();
    }
}

String getValue(String data, char separator, int index) { //分割命令
    long int found = 0;
    long int strIndex[] = { 0, -1 };
    long int maxIndex = data.length() - 1;
    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void BTActive() {
  BT.begin(Name,true);
  connect = BT.connect(slaveAdd);
  if(connect) Serial.println("succ");
  else {
    while(!connect){
      Serial.println("fail");
      connect = BT.connect(slaveAdd);
      delay(5000);
    }
    Serial.println("succ");
  }
  BTListener();
}
  
void BTListener() {
  String response;
  long int startTime;
  Serial.print("Send ");Serial.println(Fullcmd);
  BT.print(Fullcmd);

  if(Fullcmd=="test_conect") {
    startTime=millis();
    while(millis()-startTime<=(30*1000)) {
      if(BT.available()) {
        if(BT.readString()=="Test complete") {
          nanoIsWorking=true;
          Fullcmd="";
          return;
        }
      }
    }
  }

  long int sinkSec = getValue(Fullcmd,',',0).toInt();
  double ensureTime = sinkSec*1000/10.0;
  int counter=1;
  startTime = millis();

  while(millis()-startTime<=sinkSec*1000) {
    if(millis()-startTime>=ensureTime*counter) {
      BT.println("ensure Nano is OK");
      Serial.println(counter);
      counter++;
    }
  }
  Fullcmd="";
}



/*


吸水 -> 過程
?藍芽段連


吸滿了 ->等待固定時間 ==保證藍芽暢通

藍芽段連->上浮
*/











