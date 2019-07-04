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

  BG96.write("AT+QGPS=4,30,1000,0,1\r");
  ShowSerialData();
}

void loop()
{
  BG96.write("AT+QGPSLOC=2\r");
  ShowSerialData();
  delay(1000);
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

void Init()
{
  BG96.begin(9600);
  Startup();

  BG96.end();
  BG96.begin(115200);

  BG96.write("AT+IPR=9600\r");
  delay(200);
  ShowSerialData1(1);
  Serial.write("RDY\n\r");

  BG96.end();
  BG96.begin(9600);

  BG96.write("AT+CPSMS=0\r");
  ShowSerialData();
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

void Startup()
{
  BG96.write("AT\r");
  
  if (ShowSerialData1(1) == 0) // no response
  {
    Serial.write("RST\n\r");
    digitalWrite(RST, HIGH);
    delay(1000);
    digitalWrite(RST, LOW);
    Serial.write("RST done \n\r");
    
    if (ShowSerialData1(20) == 0) // no response
      {
     Serial.write("PWR\n\r");
     digitalWrite(PWR, HIGH);
     delay(1000);
     digitalWrite(PWR, LOW);
     Serial.write("PWR done\n\r");

      ShowSerialData1(10);
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

      if (Data[x - 2] == 'O' && Data[x - 1] == 'K' && Data[x] == '\r') //OK
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
        y = 0;
      }

      if (Data[x - 2] == 'R' && Data[x - 1] == 'D' && Data[x] == 'Y') //OPEN
      {
        val = 1;
        Data = "";
        receive_done = 1;
        Serial.write("reponse was RDY\n\r");
        y = 0; 
      }  
      
      x++;
    }
    
    if(y == 32767)
    {
      y = 0;
      if(z == value)
      {
        Serial.write("TIMEOUT -> ");
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
