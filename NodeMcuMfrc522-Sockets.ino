

#include <ESP8266WiFi.h>

///
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN D2
#define RST_PIN D1
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.

int readsuccess;
byte readcard[4];
char str[32] = "";
String StrUID;
String Rfid = "vazio";
String teste;

///

#define NAME "LoginDoWifi"
#define PASS "SenhaDoWifi"

const char* host = "192.168.0.100";


WiFiClient client;

void setup() {
  Serial.begin(115200);

  pinMode(D8,INPUT);
  pinMode(D3,OUTPUT);
  pinMode(D0,OUTPUT);
//
SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  Serial.println("Scan PICC to see UID...");

  //
  Serial.println();

  /* Set Client up as station */
  WiFi.mode(WIFI_STA);

  WiFi.begin(NAME, PASS);

  /* Connect to the network */
  Serial.print("Connecting");
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  
}

void handleUIDResult() {
  readsuccess = getid();
 
  if(readsuccess) {
    
    //client.println("text/plane", StrUID); //--> Send Temperature value only to client ajax request
    
    Serial.println(StrUID);
    Serial.println(""); 
    Serial.println("Please tag a card or keychain to see the UID !");
    Serial.println("");  
    //digitalWrite(ON_Board_LED, HIGH); //--> Led on board is turned off as an indicator that the UID has been printed
  }
}


int getid() {  
  if(!mfrc522.PICC_IsNewCardPresent()) {
    return 0;
  }
  if(!mfrc522.PICC_ReadCardSerial()) {
    return 0;
  }
 
  digitalWrite(D0,HIGH);
  Serial.println("THE UID OF THE SCANNED CARD IS:");
  
  for(int i=0;i<4;i++){
    readcard[i]=mfrc522.uid.uidByte[i]; //storing the UID of the tag in readcard
    array_to_string(readcard, 4, str);
    StrUID = str;
  }
  digitalWrite(D0,LOW);
  digitalWrite(D3,LOW);
  mfrc522.PICC_HaltA();
  return 1;
}

void loop() {
//
readsuccess = getid();
 
  if(readsuccess) {
    Serial.println(StrUID);
    Serial.println("");   
  }
if(digitalRead(D8)==0)
    {

    }
    else
    {
      digitalWrite(D3,HIGH);
      Serial.println("Button pressed");
      teste = "teste";
    }

///
  if (client.connect(host, 3090))
  {
    
    Serial.print("Connected to: ");
    Serial.println(host);
    
    /* Send "connected" to the server so it knows we are ready for data */
    if(StrUID != Rfid){
      Rfid = StrUID;
       client.println(StrUID);
    }else if(digitalRead(D8)!=0){
      client.println(teste);
    }else{
      client.println();
    }
    //client.println(StrUID); //USE client.println()!!
    Serial.println("Host message: \n");

    /* Wait for data for 5 seconds at most before timing out */
    unsigned long timeout = millis();
    while(client.available() == 0)
    {
      if(millis() - timeout > 5000)
      {
        Serial.println("Timeout to server!");
        break;
      }
    }

    /* Read in the data in the stream */
    while(client.available() > 0)
    {
      Serial.println(client.readStringUntil('\n'));
    }
    client.stop();
  }
  else
  {
    client.stop();
  }
  delay(1000);
}


// --------------------------------------------------------------------
void array_to_string(byte array[], unsigned int len, char buffer[]) {
    for (unsigned int i = 0; i < len; i++)
    {
        byte nib1 = (array[i] >> 4) & 0x0F;
        byte nib2 = (array[i] >> 0) & 0x0F;
        buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
        buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
    }
    buffer[len*2] = '\0';
}
