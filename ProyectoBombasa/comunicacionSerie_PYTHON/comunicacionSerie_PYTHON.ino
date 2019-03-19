#include <Arduino.h>
#include <TM1637Display.h>

int valor = 0;            //Contiene el valor deseado por el usuario
int ultimoValor = 0;
int MIN = 0;
int MAX = 20;

//Pantalla 7 segmentos 4 dígitos
#define CLK 11
#define DIO 10

TM1637Display display(CLK, DIO);

void setup() {
  Serial.begin(9600);

}

void loop() {
  serialEvent();
      
      
}

void serialEvent(){
  
  while (Serial.available() > 0) {
    Serial.println("Introduzca un valor comprendido entre ");
    Serial.println(MIN);
    Serial.println(" y ");
    Serial.println(MAX);
    String str = Serial.readStringUntil('\n'); //lectura de la entrada de varios dígitos
    float valor = str.toFloat();

    if(valor != ultimoValor && valor>=MIN && valor<=MAX){
       
      MostrarPantalla(valor);
      ultimoValor = valor;
      
    }else{
      Serial.println("El valor introducido está fuera de los límites");
    }
  }
  
}

void MostrarPantalla(float valor){

  int entera = 0;
  int decimal = 0;

  //Ajuste de número real para separarlo en dos enteros de parte real e imaginaria
  entera = (int)(valor);
  decimal = 10 * (valor - entera); 

  //Representación en pantalla del número real
  display.setBrightness(0x0f); 
  display.showNumberDec(entera,false,2,0); 
  display.setBrightness(0x0f); 
  display.showNumberDec(decimal,false,1,3);
}
