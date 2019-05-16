#include "Arduino.h"
#include <LiquidCrystal_I2C.h>;
LiquidCrystal_I2C lcd(0x27,16,2);

#define Q1 6  //Khai báo chân của module MT8870
#define Q2 5
#define Q3 4
#define Q4 3
#define StQ 2 // Chân StQ báo trạng thái có tín hiệu mới từ module MT8870
#define start 7 // Khai báo chân cảm biến ánh sáng phát hiện có cuộc gọi tới
#define playTone 8 // Khai báo chân PlayE của module ISD1820

#define LM35 A0 // Khai báo chân cảm biến nhiệt độ

#define TB1 9  // Khai báo chân Rơ le
#define TB2 10
#define TB3 11
#define TB4 12

#define Sao 11   // Định nghĩa cho nút *
#define Thang 12 // Định nghĩa cho nút #

bool byte1 = 0; // biến lưu trạng thái của module MT8870
bool byte2 = 0;
bool byte3 = 0;
bool byte4 = 0;

bool state1 = false; // Lưu trạng thái của thiết bị
bool state2 = false;
bool state3 = false;
bool state4 = false;

int Mode = 0;
int look;
 
int password[4] = {1,2,3,4};  // Password 
int keyboard[4]; // Mảng lưu giá trị mật khẩu được nhập vào từ bàn phím

int control;

int count = 0;
bool nhap = true;
int lanSai = 0;

bool flagRead = false;
int val;

void readDTMF() // Hàm ngắt đọc giá trị gửi về từ module MT8870
{
  byte1 = digitalRead(Q1);
  byte2 = digitalRead(Q2);
  byte3 = digitalRead(Q3);
  byte4 = digitalRead(Q4);
  //Serial.println("Read new data");
  //  Serial.print(byte4 + String(" - "));
  //  Serial.print(byte3 + String(" - "));
  //  Serial.print(byte2 + String(" - "));
  //  Serial.println(byte1);
  val = giaima(byte1, byte2, byte3, byte4);
  flagRead = true;
}

void setup() {
  pinMode(Q1, INPUT);
  pinMode(Q2, INPUT);
  pinMode(Q3, INPUT);
  pinMode(Q4, INPUT);
  pinMode(start, INPUT);
  pinMode(playTone, OUTPUT);
  pinMode(TB1, OUTPUT);
  pinMode(TB2, OUTPUT);
  pinMode(TB3, OUTPUT);
  pinMode(TB4, OUTPUT);

  digitalWrite(TB1, LOW);
  digitalWrite(TB2, LOW);
  digitalWrite(TB3, LOW);
  digitalWrite(TB4, LOW);

  lcd.begin();       //Khởi động màn hình. Bắt đầu cho phép Arduino sử dụng màn hình
  lcd.backlight();   //Bật đèn nền

  Serial.begin(9600);
  Serial.println("Read DTMF");

  lcd.setCursor(0,0);
  lcd.print("");

  attachInterrupt(0, readDTMF, RISING); // Khai báo ngắt cho chương trình đọc dữ liệu của module MT8870 (chân StQ nối với chân D2)
}

void loop() {
  if(digitalRead(start) == LOW){ // Cảm biến ánh sáng phát hiện có cuộc gọi
    switch(Mode){
      case 0:{ // Thông báo
        digitalWrite(playTone, HIGH); // Kích hoạt phát âm thanh mời nhập mật khẩu
        delay(500);
        digitalWrite(playTone, LOW);
        Serial.println("Moi ban nhap mat khau");
        Mode = 1;
        Serial.println("Chuyen qua che do nhap mat khau");
      }
      break;
      case 1:{ // Nhập mật khẩu
        if(flagRead == true){
          unlock();
          flagRead = false;
        }
        if(look == 3){
          Serial.println("Sai mat khau nhieu lan. Tam thoi khoa may");
          while(1){
            Serial.println("Treo");
            delay(2000);
          }
        }
        else if(look == 1){
          Serial.println("Mat khau chinh xac! Chuyen qua che do dieu khien");
          Mode = 2;
        }
      }
      break;
      case 2:{ // Chế độ điều khiển thiết bị
        if(flagRead == true){
          dieukhien();
          flagRead = false;
        }
      }
      break;
    }
  }
}

int giaima(bool n1, bool n2, bool n3, bool n4){ // Hàm chuyển đổi BIN sang DEC
  int number = (n1 + (n2 * 2) + (n3 * 4) + (n4 * 8));
  return number;
}

void unlock(){ // Hàm mở khóa
  if(val == Thang){ // nếu nút # được nhấn thì kiểm tra mật khẩu đúng hay không
    if(keyboard[0] == password[0] && keyboard[1] == password[1] && keyboard[2] == password[2] && keyboard[3] == password[3]){ // Nếu mk đúng
      Serial.println("Mat khau chinh xac");
      clearKeyboard();
      count = 0;
      look = 1; // mật khẩu đúng thì trả về kết quả bằng 1
    }
    else{ // Nếu mk sai
      Serial.println("Sai mat khau. Moi nhap lai");
      clearKeyboard();
      count = 0;
      lanSai++; 
      look = 0; // mật khẩu sai, cho phép tiếp tục nhập mật khẩu
      if(lanSai >= 3){ // Nếu mật khẩu sai quá 3 lần thì trả kết quả về bằng 3 và vô hiệu hóa chương trình
        Serial.println("Sai mat khau 3 lan. Khoa may!");
        lanSai = 0;
        look = 3; 
        // Chờ đến khi màn hình điện thoại tắt thì chương trình mới hoạt động trở lại            
      }
    }
  }
  else if(count == 4){ count = 0;}
  else{
    keyboard[count] = val;
    Serial.print(keyboard[count]);
    count++;
  }
}
void clearKeyboard(){
  for(int i=0; i<4; i++){ // clear keyboard
    keyboard[i] = 0;
  }
}

void dieukhien(){ // Hàm điều khiển thiết bị
  if(val == Sao){
    if(control == 1){
      Serial.println("Bat/Tat thiet bi so 1");
      thietbi1();
    }
    else if(control == 2){
      Serial.println("Bat/Tat thiet bi so 2");
      thietbi2();
    }
    else if(control == 3){
      Serial.println("Bat/Tat thiet bi so 3");
      thietbi3();
    }
    else if(control == 4){
      Serial.println("Bat/Tat thiet bi so 4");
      thietbi4();
    }
    else if(control == 5){
      Serial.println("Bat tat ca cac thiet bi");
      onAll();
    }
    else if(control == 6){
      Serial.println("Tat tat ca cac thiet bi");
      offAll();
    }
  }
  else{
    control = val;
    Serial.println(control);
  }
}

void thietbi1(){
  if(!state1){
    digitalWrite(TB1, HIGH);
    state1 = true;
  }
  else {
    digitalWrite(TB1, LOW);
    state1 = false;
  }
}
void thietbi2(){
  if(!state2){
    digitalWrite(TB2, HIGH);
    state2 = true;
  }
  else {
    digitalWrite(TB2, LOW);
    state2 = false;
  }
}
void thietbi3(){
  if(!state3){
    digitalWrite(TB3, HIGH);
    state3 = true;
  }
  else {
    digitalWrite(TB3, LOW);
    state3 = false;
  }
}
void thietbi4(){
  if(!state4){
    digitalWrite(TB4, HIGH);
    state4 = true;
  }
  else {
    digitalWrite(TB4, LOW);
    state4 = false;
  }
}
void onAll(){
  digitalWrite(TB1, HIGH);
  digitalWrite(TB2, HIGH);
  digitalWrite(TB3, HIGH);
  digitalWrite(TB4, HIGH);
}
void offAll(){
  digitalWrite(TB1, LOW);
  digitalWrite(TB2, LOW);
  digitalWrite(TB3, LOW);
  digitalWrite(TB4, LOW);
}
void readTemp(){
  int reading = analogRead(LM35); // Đọc giá trị từ cảm biến LM35
  float voltage = reading * 5.0 / 1024.0; // Tính ra hiệu điện thế 
  float temp = voltage * 100.0; // Tính ra nhiệt độ

  //Serial.println(String("Nhiet do: ") + temp);
  lcd.setCursor(0,0);
  lcd.print("Nhiet do:");
  lcd.setCursor(10,0);
  lcd.print(temp);
}
