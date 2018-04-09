#include <SD.h>

#include <SoftwareSerial.h>
#define errorHalt(msg) {Serial.println(F(msg)); while(1);}
#define RX 2
#define TX 3
String AP = "DukeOpen";       // CHANGE ME
String PASS = ""; // CHANGE ME
String API = "Z29L3NWYLD7JILHY";   // CHANGE ME
String HOST = "api.thingspeak.com";
String PORT = "80";
String field = "field1";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
String valSensor = "";
const int cs = 10;

SoftwareSerial esp8266(RX, TX); 
 
  
void setup() { 
  Serial.begin(9600);
  esp8266.begin(9600);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");

  pinMode(10, OUTPUT);


  // see if the card is present
  if (!SD.begin(cs)) 
  {
    Serial.println("Card failed to initialize, or not present");
  
    return;
  }
  Serial.println("card initialized.");
  
  File myfile = SD.open("ADC~4BE3.CSV");
  
  Serial.println("open");
  
  // if the file is available, read the file
  if (myfile) 
  {
    myfile.seek(0);
      size_t n;      // Length of returned field with delimiter.
  char str[20];  // Must hold longest field with delimiter and zero byte.
  int count=0;
    while (myfile.available())
    {

       // Read the file and print fields.
  while (true) {
    n = readField(&myfile, str, sizeof(str), ",\n");

    // done if Error or at EOF.
    if (n == 0) break;

    // Print the type of delimiter.
    if (str[n-1] == ',' || str[n-1] == '\n') {
     // Serial.print(str[n-1] == ',' ? F("comma: ") : F("endl:  "));
      // Remove the delimiter.
      str[n-1] = 0;
      count++;
    } else {
      // At eof, too long, or read error.  Too long is error.
      Serial.print(myfile.available() ? F("error: ") : F("eof:   "));
    }
    // Print the field.
    if ((count%2)==0){
    Serial.println(str);
    valSensor = str;

   String getData = "GET /update?api_key="+ API +"&"+ field +"="+String(str);
   sendCommand("AT+CIPMUX=1",5,"OK");
   sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
   sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
   esp8266.println(getData);delay(15000);countTrueCommand++;
   sendCommand("AT+CIPCLOSE=0",5,"OK");
      }
    }
  }




    
    myfile.close();
    Serial.write("Closed");
}
}
void loop() {

}
int getSensorData(){
  return random(1000); // Replace with the SD card file 
  
}
void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }


size_t readField(File* file, char* str, size_t size, char* delim) {
  char ch;
  size_t n = 0;
  while ((n + 1) < size && file->read(&ch, 1) == 1) {
    // Delete CR.
    if (ch == '\r') {
      continue;
    }
    str[n++] = ch;
    if (strchr(delim, ch)) {
        break;
    }
  }
  str[n] = '\0';
  return n;
}
