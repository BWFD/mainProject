/*
藍色 Vin
白色 GND
紫色 RX2
綠色 TX2

*/
#include<HardwareSerial.h>
HardwareSerial MySerial(1);

String getValue(String,char,int);

void setup() {
  Serial.begin(9600);
  //                             RX,TX
  MySerial.begin(9600,SERIAL_8N1,16,17);
  
}

void loop() {
  char c;
  String str="";
  String response="";
  String Latitude="default";
  String Longitude="default";
  
  if(Serial.available()) {
    //*
    c=Serial.read();
    MySerial.write(c);
    /*/
    str = Serial.readString();
    Serial.println("From Serial: "+str);
    MySerial.print(str+"\r\n");
    //*/
  }
  if(MySerial.available()) {
    response=MySerial.readString();
    Serial.println(response);

    
    Latitude=getValue(response,',',2);
    Longitude=getValue(response,',',3);
  }
  
  if(Latitude=="" || Longitude=="") {
    Serial.println("ERROR!!!!");
  }
  response="";
  Latitude="default";
  Longitude="default";

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
/*
AT+QIOPEN=1,0,"UDP",118.27.81.25,8190
AT+QISTATE?
AT+QISEND=0,8,3B017126
AT+QICLOSE=0
AT+CSQ

AT+QGNSSC?
AT+QGNSSAGPS?


AT+QGNSSRD="NMEA/GGA"
hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh
055148,2407.8945,N,12041.7649,E,1,00,1.0,155.2,M,16.6,M,X.X,xxxx,*47
055148 -> UTC
2407.8945,N -> Latitude 
12041.7649,E -> Longitude
1 -> GPS quality indicator (0=invalid; 1=GPS fix; 2=Diff. GPS fix)
00 -> Number of satellites in use
1.0 -> Horizontal dilution of position
155.2 -> Antenna altitude above/below mean sea level (geoid)
M -> Meters
16.6 -> Geoidal separation
M -> Meters
X.X -> Age in seconds since last update from diff. reference station
xxxx -> Diff. reference station ID# 
*47 -> Checksum

https://shyuanliang.blogspot.com/2009/08/nmea-gps.html
080719.07,,,,,0,00,99.99,,,,,,*78


範例資料
start@資料數@GPS,GPS@PH,PH,PH,PH@NTU,NTU,NTU,NTU@TDS,TDS,TDS,TDS@end
AT+QISEND=0,63,start@4@GPS,GPS@PH,PH,PH,PH@NTU,NTU,NTU,NTU@TDS,TDS,TDS,TDS@end


start@4@2407.8945,12041.7649@7.1,7.2,7.3,7.4@3.2,3.3,2.2,3.5@4.0,3.4,1.4,4.4@end
AT+QISEND=0,81,start@4@02407.8945,12041.7649@7.1,7.2,7.3,7.4@3.2,3.3,2.2,3.5@4.0,3.4,1.4,4.4@end
*/
