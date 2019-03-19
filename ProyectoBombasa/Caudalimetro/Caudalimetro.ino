//Caudalímetro Contador
int caudalimetro = 8;

//Variables para la función leer caudal
int contador = 0;
int ultimo_estado = 0;
int estado;
int tiempo;
int ultimo_tiempo = 0;
float f;
int conteo = 0;
int variacion;




void setup(){
  Serial.begin(9600);
  pinMode(caudalimetro, INPUT);
}

void loop(){
  
  f=leerCaudal();

  //Ajustar el valor de f para su caudal correspondiente
  //Serial.println("Frecuencia: ");
  //Serial.println(f);
}


float leerCaudal(){
  float frecuencia;
  

  tiempo=millis();

  estado=digitalRead(caudalimetro);

  
  if(estado == HIGH){
    Serial.println("Hola");
    
    variacion = tiempo - ultimo_tiempo;
    frecuencia = 1/variacion;

    Serial.print("freq:");
    Serial.println(frecuencia);
    Serial.print("estado: ");
    Serial.println(estado);
    Serial.print("contador: ");
    Serial.println(contador);
    contador++;

  }
  ultimo_estado = estado;
  ultimo_tiempo = tiempo;
  
  
  return frecuencia;
  
}
