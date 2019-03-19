#include <Arduino.h>
#include <TM1637Display.h>

int valor = 0;            //Contiene el valor deseado por el usuario
int ultimoValor = 0;
int MIN = 0;
int MAX = 20;

//Pantalla 7 segmentos 4 dígitos
#define CLK 11
#define DIO 10


int i;

TM1637Display display(CLK, DIO);

void setup() {
  Serial.begin(9600);
  display.clear();
  display.setBrightness(0x0f);

}

void loop() {
  serialEvent();

      //for(i=0; i<100; i++){
        //Serial.write(i);
        //MostrarPantalla(i);
      //}
      
}

void serialEvent(){
  
  if (Serial.available() > 0) {
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
  decimal = round(100 * (valor - entera)); 

  //Representación en pantalla del número real
    display.clear(); //Limpia la pantalla
    display.setBrightness(0x0f); 
    display.showNumberDecEx(entera,(0x80 >> 1),false,2,0); 
    display.showNumberDecEx(decimal,(0x80 >> 1),false,2,2); 
}
