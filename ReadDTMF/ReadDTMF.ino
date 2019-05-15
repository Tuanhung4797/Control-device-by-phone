#define Q1 6
#define Q2 5
#define Q3 4
#define Q4 3
#define StQ 2
#define start 7
#define playTone 8

#define LM35 A0

#define TB1 9
#define TB2 10
#define TB3 11
#define TB4 12

#define Sao 11
#define Thang 12

bool byte1 = 0;
bool byte2 = 0;
bool byte3 = 0;
bool byte4 = 0;

bool state1 = false;
bool state2 = false;
bool state3 = false;
bool state4 = false;

int Mode = 0;
int look;

int password[4] = {1,2,3,4};
int keyboard[4];

int control;

int count = 0;
bool nhap = true;
int lanSai = 0;

bool flagRead = false;
int val;

void readDTMF()
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

  Serial.begin(9600);
  Serial.println("Read DTMF");

  attachInterrupt(0, readDTMF, RISING);
}

void loop() {
  if(digitalRead(start) == LOW){ // Cảm biến ánh sáng phát hiện có cuộc gọi
    digitalWrite(playTone, HIGH); // Kích hoạt phát âm thanh mời nhập mật khẩu
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
//  if(flagRead == true){
//    unlock();
//    flagRead = false;
//  }
}

int giaima(bool n1, bool n2, bool n3, bool n4){
  int number = (n1 + (n2 * 2) + (n3 * 4) + (n4 * 8));
  return number;
}

void unlock(){
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
      if(lanSai >= 3){
        Serial.println("Sai mat khau 3 lan. Khoa may!");
        lanSai = 0;
        look = 3; // Nếu mật khẩu sai quá 3 lần thì trả kết quả về bằng 3 và vô hiệu hóa chương trình
                    // Chờ đến khi màn hình điện thoại tắt thì chương trình mới hoạt động trở lại
      }
    }
  }
  else if(count == 5){ count = 0;}
  else{
    keyboard[count] = val;
    Serial.print(keyboard[count]);
    count++;
  }
  return look;
}
void clearKeyboard(){
  for(int i=0; i<5; i++){ // clear keyboard
    keyboard[i] = 0;
  }
}

void dieukhien(){
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
