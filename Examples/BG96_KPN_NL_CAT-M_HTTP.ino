#include <SoftwareSerial.h> // for UART communication with the Quectel BG96 module 

SoftwareSerial BG96(7, 8);
const int PWR = 6;
const int RST = 5;
int x, y, z;
char RXchar;
int receive_done = 0;
String Data = "";

void setup()
{
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  Serial.write("starting script\r\n");

  pinMode(PWR, OUTPUT);
  pinMode(RST, OUTPUT);

  Init();
  
  Serial.write("\r\n");
  Serial.write("start of script\r\n");

  /////////////////BASIC DATA/////////////////
  BG96.write("AT\r");
  ShowSerialData();
  BG96.write("AT+GMM\r");
  ShowSerialData();
  BG96.write("AT+GMR\r");
  ShowSerialData();
  BG96.write("AT+QAPPVER\r");
  ShowSerialData();

  //////////////////NETWORK SETTINGS/////////////////////
  BG96.write("AT+CFUN=0\r");
  ShowSerialData();
  BG96.write("AT+QCFG=\"BAND\",1,80000,1,1\r");
  ShowSerialData();
  BG96.write("AT+QCFG=\"NWSCANMODE\",3,1\r");
  ShowSerialData();
  BG96.write("AT+QCFG=\"NWSCANSEQ\",020103,1\r");
  ShowSerialData();
  BG96.write("AT+QCFG=\"IOTOPMODE\",2,1\r");
  ShowSerialData();
  BG96.write("AT+CFUN=1\r");
  ShowSerialData();

  /////////////////CONNECTING TO NETWORK///////////////////
  delay(200);
  BG96.write("AT+CIMI\r");
  ShowSerialData();
  BG96.write("AT+CGDCONT=1,\"IP\",\"internet.m2m\"\r");
  ShowSerialData();
  delay(200);
  BG96.write("AT+COPS=1,2,\"20408\",8\r");
  ShowSerialData();
  delay(1000);
  BG96.write("AT+CSQ\r");
  ShowSerialData();
  BG96.write("AT+CSQ\r");
  ShowSerialData();
  BG96.write("AT+QNWINFO\r");
  ShowSerialData();

  //////////////SENDING DATA//////////////////////////////
  BG96.write("AT+QIACT=1\r");
  ShowSerialData();
  BG96.write("AT+QIACT?\r");
  ShowSerialData();
  BG96.write("AT+QHTTPURL=29,80\r");
  ShowSerialDataOpen();
  BG96.write("https://postman-echo.com/post\n\r");
  ShowSerialData();
  BG96.write("AT+QHTTPPOST=16\r");
  ShowSerialDataOpen();
  BG96.write("test successful!");
  ShowSerialDataOpen();
  BG96.write("AT+QHTTPREAD=80\r");
  ShowSerialDataOpen();
  BG96.write("AT+QIDEACT=1\r");
  ShowSerialData();
  delay(500);

  ///////////////ENTERING IN PSM////////////////////////
  BG96.write("AT+QCFG=\"psm/enter\",1\r");
  ShowSerialData();
  BG96.write("AT+CPSMS=1,,,\"10100001\",\"00000001\"\r");
  ShowSerialData();

  ShowSerialData();
}

void loop()
{

}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

void Init()
{
  BG96.begin(9600);
  Startup();

  BG96.write("AT+IPR=9600\r");
  delay(200);
  ShowSerialData1(1);

  BG96.end();
  delay(100);
  BG96.begin(9600);

  BG96.write("AT+CPSMS=0\r");
  ShowSerialData1(1);
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

void Startup()
{
  BG96.write("AT\r");

  if (ShowSerialData1(1) == 0) //if no response
  {
    BG96.end();
    BG96.begin(115200);
    BG96.write("AT\r");

    if (ShowSerialData1(1) == 0) //if no response
    {
      Serial.write("TIMEOUT -> RST \n\r");
      digitalWrite(RST, HIGH);
      delay(200);
      digitalWrite(RST, LOW);
      Serial.write("RST done \n\r");

      if (ShowSerialData1(20) == 0) // if no response
      {
        Serial.write("TIMEOUT ->PWR\n\r");
        digitalWrite(PWR, HIGH);
        delay(500);
        digitalWrite(PWR, LOW);
        Serial.write("PWR done\n\r");

        ShowSerialData1(10);
      }
    }
  }
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

int ShowSerialData()
{
  receive_done = 0;
  x = 0;

  while (receive_done == 0)
  {
    if (BG96.available() != 0)
    {
      RXchar = BG96.read();
      Serial.write(RXchar);
      Data.concat(RXchar);

      if (Data[x - 2] == 'R' && Data[x - 1] == 'O' && Data[x] == 'R') //ERROR
      {
        receive_done = 1;
        Data = "";
      }

      if (Data[x -1 ] == 'O' && Data[x] == 'K') //OK
      {
        receive_done = 1;
        Data = "";
      }

      if (Data[x - 2] == 'R' && Data[x - 1] == 'D' && Data[x] == 'Y') //RDY
      {
        receive_done = 1;
        Data = "";
      }

      x++;
    }

  }
  Serial.write("\n");
  return receive_done;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

void ShowSerialDataOpen()
{
  receive_done = 0;
  x = 0;

  while (receive_done == 0)
  {
    if (BG96.available() != 0)
    {
      RXchar = BG96.read();
      Serial.write(RXchar);
      Data.concat(RXchar);

      if (Data[x - 2] == 'R' && Data[x - 1] == 'O' && Data[x] == 'R') //ERROR
      {
        receive_done = 1;
        Data = "";
      }

      if (Data[x - 3] == '0' && Data[x - 2] == ',' && Data[x - 1] == '0' && Data[x] == '\r') //OPEN
      {
        receive_done = 1;
        Data = "";
      }

      if (Data[x - 3] == ',' && Data[x - 2] == '3' && Data[x - 1] == '4' && Data[x] == '1') //POST
      {
        receive_done = 1;
        Data = "";
      }
      
      if (Data[x - 3] == ',' && Data[x - 2] == '3' && Data[x - 1] == '4' && Data[x] == '1') //POST
      {
        receive_done = 1;
        Data = "";
      }
      
      if (Data[x - 2] == 'E' && Data[x - 1] == 'C' && Data[x] == 'T') //CONNECT
      {
        receive_done = 1;
        Data = "";
      }

      if ( Data[x - 2] == '0' && Data[x - 1] == ',' && Data[x] == '5') //QIOPEN reponse
      {
        receive_done = 1;
        Data = "";
      }

      x++;

    }
  }
  Serial.write("\n");
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

int ShowSerialData1(int value)
{
  int val;
  x = 0;
  y = 0;
  z = 0;
  val = 0;
  receive_done = 0;

  while (receive_done == 0)
  {
    if (BG96.available() != 0)
    {
      RXchar = BG96.read();
      Data.concat(RXchar);

      if (Data[x - 1] == 'O' && Data[x] == 'K') //OPEN
      {
        val = 1;
        Data = "";
        receive_done = 1;
        Serial.write("reponse was OK\n\r");
      }

      if (Data[x - 2] == 'R' && Data[x - 1] == 'D' && Data[x] == 'Y') //OPEN
      {
        val = 1;
        Data = "";
        receive_done = 1;
        Serial.write("reponse was RDY\n\r");
      }

      x++;
    }

    if (y == 32767)
    {
      y = 0;
      if (z == value)
      {
        receive_done = 1;
        val = 0;
      }
      z++ ;
    }
    y++;
  }
  return val;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
