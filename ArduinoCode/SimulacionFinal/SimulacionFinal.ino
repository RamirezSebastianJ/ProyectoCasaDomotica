
#include <LiquidCrystal.h>
#include <Servo.h>
#include <Keypad.h>


Servo myservo;

//Constantes para la pantalal LCD
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


int outDigital = 13;//Para trabajar sobre el led
int sensorPin = 12; //Recibe Datos del Sensor PIR
int rele = 11; //Control de Relé
int servoPin = 10; //Para Controlar el Servomotor

//Variables Para Keypad
int i=0;
int flag_clave_incorrecta=0;
int flag_clave_correcta=0;

//Constantes Para Keypad
const byte ROWS = 4; //four rows
const byte COLS = 3; //four columns
char clave[]={'2','4','6'};
char ingreso[3];

//Teclas del KEYPAD
char hexaKeys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {A0, A1, A2, A3}; //filas
byte colPins[COLS] = {A8, A9, A10}; //columnas

// creamos el objeto CustomKeypad
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);


int lecturaSensorPir = 0; //sensor PIR sin detectar movimiento
int pos = 0;    // variable para controlar la posicion del Servomotor

void setup() {

  myservo.attach(servoPin);  // une el servo en el pin 9 al objeto servo
  myservo.write(0);          //indicca al servomotor cual es su posicion inicial

  
  lcd.begin(16, 2); // Configura el numero de filas y columnas que tendrá la pantalla LCD
  
  // declarar el pin de conexion 13 del led como salida
  pinMode(outDigital, OUTPUT);

  //declarar el Pin de conexion 11 del Rele como una Entrada
  pinMode(rele, OUTPUT);
  
  // declarar el Pin de conexion 12 del sensor Pir como una Entrda
  pinMode(sensorPin, INPUT);
  Serial.begin(9600); //Inicia la comunicacion serial con la computadora con una velocidad de 9600 bits

  //Utilizamos un for para calibrar el sensor 
  for(int i = 0; i > 30; i++){
    delay(1000);
  }
  delay(50);

}


//===================================Funcion para controlar el sistema de corriente por medio del rele
void ControlDeCorriente(){

  //Este codigo es para evidenciar el funcionamiento del rele para controlar una corriente
  digitalWrite(rele, HIGH);
  delay(1000);
  digitalWrite(rele, LOW);
  delay(1000); 
  
}
//===================================


//=========================================Controlar el ServoMotor
void Abrir(){
  
  for (pos; pos <= 180; pos += 1) {   // va de 0° a 180° en pasos de 1°
    myservo.write(pos);              // Decirle al servo que debe ir a la posicion (pos)
    delay(15);                       // espera 15 ms para que el servo alcance la posicion
  }
}


void Cerrar(){
  for (pos; pos >= 0; pos -= 1) {    // va de 180° a 0° en pasos de 1°
    myservo.write(pos);              // Decirle al servo que debe ir a la posicion (pos)
    delay(15);                       // espera 15 ms para que el servo alcance la posicion
  }
}
//=========================================


//=========================================Indicaciones del Servomotor por medio del KEYPAD
void AbrirCerradura(){

  while(flag_clave_correcta==0){  
    lcd.setCursor(0, 0);      
    lcd.print("INGRESE CLAVE:");
    lcd.setCursor(5, 1);
    
    i=0;
    while(i<3){
      char customKey = customKeypad.getKey(); //Tomar las pulsacones del KEYPAD
      if (customKey != NO_KEY){
        ingreso[i]=customKey;                 //Guardar las pulsaciones
        i++;
        lcd.print("* ");
      }    
    }
       
    i=0;
    while((flag_clave_incorrecta==0) &&(i<3)){
      if(clave[i]!=ingreso[i]){               //Comparar Clave ingresada con la autorizada en el Sistema
        flag_clave_incorrecta=1;
        delay(200);
        lcd.setCursor(0, 1);
        lcd.println("clave incorrecta");        
        Cerrar();
        delay(500);
        lcd.clear();
      }else{
        i++;
      }
    }
      
    if(i==3){                               //Si i es igual 3 entonces la clave es correcta
      flag_clave_correcta=1;
      delay(200);
      lcd.setCursor(0, 1);
      lcd.println("clave correcta") ;
      digitalWrite(13,HIGH);
      delay(500);
      Abrir();                              //Se abre la Cerradura ( SERVO --> 0° a 180°)
      lcd.setCursor(0, 0);
      lcd.clear();
      lcd.println("Bienvenido") ;
    }
  }
}
//=========================================



void loop() {
  
  ControlDeCorriente(); //Inciar el control de corrinte por medio del Relé

  
  lecturaSensorPir = digitalRead(sensorPin);  //Tomar Datos del Sensor PIR
  
  if(lecturaSensorPir == HIGH){               //Detección e Movimiento
    digitalWrite(outDigital, HIGH);           //Encender el led
    AbrirCerradura();                         //Gestionar la Cerradura por medio del KEYPAD.( SERVO --> 0° a 180°)
    
  }else{                                      //No Hay Detección e Movimiento
    digitalWrite(outDigital, LOW);            //Apagar el Led
    Cerrar();                                 //Cerrar el la Cerradurra. (SERVO --> 180° a 0°)
  }
  
}
