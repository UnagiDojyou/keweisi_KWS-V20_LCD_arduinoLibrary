#include "HT1621_KWS-V20.h"
HT1621 ht(4,5,6); //ht(CS,WR,DATA);
int n=0;
int NV = 0;
int NA = 0;
long NT = 0;
long Nh = 0;

/*　a 「TS119-5」の点灯 bit番地 a～h
*　 ー
*b |　 | e
*　 ー 　<-f
*c |　 | g
*　 ー 　　　. <-h 　　 h は様々な記号や点
* 　d
*
* a b c d e f g h
* 1 1 1 1 1 1 1 1 (all on)
*   
*
*/

void setup() {
  // put your setup code here, to run once:
  ht.begin();

  ht.sendCommand(HT1621::RC256K);
  ht.sendCommand(HT1621::BIAS_THIRD_4_COM);
  ht.sendCommand(HT1621::SYS_EN);
  ht.sendCommand(HT1621::LCD_ON);

  //全灯点灯
  for(int i=0; i<15; i++){
    ht.writeV20(i, 0b11111111);
  }
  delay(1000);
 
  //全灯消灯
  for(int i=0; i<15; i++)
    ht.write(i,0);

  //0から順に表示
  for (int j=0; j<14; j++){
    for (int i=0;i<15; i++){
      ht.writeCharV20(i,j,0);
    }
    delay(500);
  }

  //全灯消灯
  for(int i=0; i<15; i++)
    ht.write(i,0);

  //HELLO
  ht.writeV20(10,0b01101110);
  ht.writeV20(11,0b11110100);
  ht.writeV20(12,0b01110000);
  ht.writeV20(13,0b01110000);
  ht.writeV20(14,0b11111010);
  delay(5000);

  //全灯消灯
  for(int i=0; i<15; i++)
    ht.write(i,0);

}

void loop(){
  //Vのテスト
  if(NV<1000){
    ht.writeV20voltage(NV,0,1); //0V~999V
  }
  if(NV>999&&NV<2000){
    ht.writeV20voltage((NV-1000)/10.0,1,1); //0V~99.9V
  }
  if(NV>1999&&NV<3000){
    ht.writeV20voltage((NV-2000)/100.0,2,1); //0V~9.99V
  }
  if(NV>2999&&NV<4000){
    ht.writeV20voltage((NV-3000),0,0); //0~999
  }
  if(NV>3999&&NV<5000){
    ht.writeV20voltage((NV-4000)/10.0,1,0); //0~99.9
  }
  if(NV>4999&&NV<6000){
    ht.writeV20voltage((NV-5000)/100.0,2,0); //0~9.99
  }
  if(NV > 5999){
    NV = 0;
  }
  else{
    NV++;
  }
  
  //Aのテスト
  if(NA<1000){
    ht.writeV20current(NA,0,1); //0A~999A
  }
  if(NA>999&&NA<2000){
    ht.writeV20current((NA-1000)/100.0,2,1); //0A~9.99A
  }
  if(NA>1999&&NA<3000){
    ht.writeV20current((NA-2000),0,0); //0~999
  }
  if(NA>2999&&NA<4000){
    ht.writeV20current((NA-3000)/100.0,2,0); //0~9.99
  }
  if(NA > 3999){
    NA = 0;
  }
  else{
    NA++;
  }

  //Tのテスト
  if(NT<10000){
    int NTh = NT / 100;
    int NTm = NT - (NTh * 100);
    ht.writeV20time(NTh,NTm,1,1); //00:00T~99:99T
  }
  if(NT>9999&&NT<20000){
    int NTh = (NT-10000) / 100;
    int NTm = (NT-10000) - (NTh * 100);
    ht.writeV20time(NTh,NTm,1,0); //00:00~99:99
  }
  if(NT>19999&&NT<30000){
    int NTh = (NT-20000) / 100;
    int NTm = (NT-20000) - (NTh * 100);
    ht.writeV20time(NTh,NTm,0,1); //0000T~9999T
  }
  if(NT>29999&&NT<40000){
    int NTh = (NT-30000) / 100;
    int NTm = (NT-30000) - (NTh * 100);
    ht.writeV20time(NTh,NTm,0,0); //0000~9999
  }
  if(NT > 39999){
    NT = 0;
  }
  else{
    NT++;
  }

  //mAhのテスト
  if(Nh<100000){
    ht.writeV20mAh(Nh,1); //0mAh~99999mAh
  }
  if(Nh>99999&&Nh<200000){
    ht.writeV20mAh(Nh-100000,0); //0~99999
  }
  if(Nh > 199999){
    Nh = 0;
  }
  else{
    Nh++;
  }
  
  delay(100);
  
  // 1セグメントずつ表示させる。
  /*
  ht.writeV20(n, 0b11111111);
  delay(1000);
  ht.writeV20(n, 0b00000000);
  ht.writeV20(n, 0b10000000);
  delay(1000);
  ht.writeV20(n, 0b01000000);
  delay(1000);
  ht.writeV20(n, 0b00100000);
  delay(1000);
  ht.writeV20(n, 0b00010000);
  delay(1000);
  ht.writeV20(n, 0b00001000);
  delay(1000);
  ht.writeV20(n, 0b00000100);
  delay(1000);
  ht.writeV20(n, 0b00000010);
  delay(1000);
  ht.writeV20(n, 0b00000001);
  delay(1000);
  ht.writeV20(n, 0b00000000);  
  if (n==14){
    n=0;}
  else{
    n++;}
  */
}
