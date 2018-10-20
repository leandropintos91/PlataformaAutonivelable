int pulsador1 =2;
 int pulsador2=3;
 int pulsador3= 4; 
 int led = 3;
int estado1 = LOW;
int estado2 = LOW;
int estado3 = LOW;

void setup() {
  // put your setup code here, to run once:
  pinMode(pulsador,INPUT);
 

}

void loop() {
  // put your main code here, to run repeatedly:

     estado1 = digitalRead(pulsador1);  
     estado2 = digitalRead(pulsador2);  
     estado3 = digitalRead(pulsador3);  



    return estado1 * estado2 * estado3;
     
     

 
  }
 
}
