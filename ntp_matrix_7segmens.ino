/*
01:28:45.796 -> MOSI: 23
01:28:45.796 -> MISO: 19
01:28:45.796 -> SCK: 18
01:28:45.796 -> SS: 5
*/
#include <SPI.h>
  #define VSPI_MISO   MISO
  #define VSPI_MOSI   MOSI
  #define VSPI_SCLK   SCK
  #define VSPI_SS     SS
  #define LATCHPIN     22
static const int spiClk = 100000; // 100kHz
SPIClass * vspi = NULL;
uint8_t sevenSegBuff[5];

uint8_t font7seg[] = {
0x77,//0
0x14,//1
0xB3,//2
0xB6,//3
0xd4,//4
0xe6,//5
0xe7,//6
0x34,//7
0xf7,//8
0xf6,//9
0x83,//c
0x97,//d
0xc5,//h
0x41,//i
0x85,//n
0xf1//p
};
void sendSevenSeg ( void )
{
  vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  /*
  display is filled from left to right so in order to write to the left digit i have 
  to send out that last(before the dots because they are the last)
  */
  vspi->transfer(sevenSegBuff[3]);//rightmost digit
  vspi->transfer(sevenSegBuff[2]);//2nd from the right
  vspi->transfer(sevenSegBuff[1]);//3rd from the right
  vspi->transfer(sevenSegBuff[0]);//4th from the right
  vspi->transfer(sevenSegBuff[4]);//dots, from the top: 0b11100000, msb is the upper dot

  vspi->endTransaction();
  digitalWrite(LATCHPIN, LOW); 
  digitalWrite(LATCHPIN, HIGH);
  delay(1); 
  digitalWrite(LATCHPIN, LOW); 
}
void writeChar7Seg(char ch, uint8_t pos)
{
  if(pos > 4)
  {
    return;
  }
  sevenSegBuff[pos] = font7seg[ch-0x30];
  sendSevenSeg();
}
void dispTest ( void )
{
  for(int x = 0; x< 4; x++)
  {
    memset(sevenSegBuff, 0, sizeof(sevenSegBuff));
    writeChar7Seg('8', x);
    delay(500);
  }
  for(int i = 0;i<8;i++)
  {
  vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  vspi->transfer(1<<i);
  vspi->transfer(1<<i);
  vspi->transfer(1<<i);
  vspi->transfer(1<<i);
  vspi->transfer(1<<i);

  vspi->endTransaction();
  digitalWrite(LATCHPIN, LOW); 
  digitalWrite(LATCHPIN, HIGH);
  delay(1); 
  digitalWrite(LATCHPIN, LOW); 
  delay(500);
  }
}
void setup() {
  // put your setup code here, to run once:
  pinMode(LATCHPIN, OUTPUT); //VSPI SS
  vspi = new SPIClass(VSPI);
  Serial.begin(115200);
  ledcSetup(4, 5000, 8);
  ledcAttachPin(4, 4);
  ledcWrite(4, 128);
  vspi->begin();
  dispTest();

  
}




void loop() {
  for(int x = 0; x< 4; x++)
  {
    for(int i = 0; i< 16; i++)
    {
    writeChar7Seg(i+0x30, x);
    delay(100);
    }
  }
  
  delay(2000);
  for(int i = 255;i >0;i = i-5)
  {
    ledcWrite(4, i);
    delay(100);
  }
  for(int i = 0;i <255;i = i+5)
  {
    ledcWrite(4, i);
    delay(100);
  }
  delay(2000);
  // put your main code here, to run repeatedly:
}









