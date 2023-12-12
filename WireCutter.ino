/**
Etapas: 

      Calibração: Corte inicial para garantir o início correto. Inserir a distância entre o rolete de tração e a lâmina de corte (considerar folga).
      01. Motor 1 avança; movimenta inicial do cabo
      02. Motor 2 avança e retrocede; corte inicial

      Avanço do comprimento da primeira ponta.
      03. Motor 1 e 3 avançam simultaneamente; avanço do cabo (comprimento da decapagem)
      04. Motor 2 avança e retrocede; marca decapagem

      Avanço do comprimento do cabo, sem as pontas.
      05. Motor 1 e 3 avançam simultaneamente; avanço do cabo (comprimento do cabo)
      06. Motor 2 avança e retrocede; marca decapagem

      Avanço do comprimento da segunda ponta.
      07. Motor 1 e 3 avançam simultaneamente; avanço do cabo (comprimento da decapagem);
      08. Motor 2 avança e retrocede; corte

      Remoção do cabo cortado/decapado
      09. Motor 3 avança; remoção do cabo

      Início do próximo cabo
      10. Repete a partir da ação 3 pela quantidade definida

*/

#define NEMA_17

#include <AccelStepper.h>

enum ETAPAS  {ETAPA_00, ETAPA_01, ETAPA_02, ETAPA_03, ETAPA_04, ETAPA_05, ETAPA_06, ETAPA_07, ETAPA_08, ETAPA_09, ETAPA_10, ETAPA_TEST_M1,ETAPA_TEST_M2,ETAPA_TEST_M3, ETAPA_IDLE};

// Definição dos pinos
#define M1_PIN_DIR 2 //Direção do motor 1
#define M1_PIN_STP 3 //Passos do motor 1

#define M2_PIN_DIR 4 //Direção do motor 2
#define M2_PIN_STP 5 //Passos do motor 2

#define M3_PIN_DIR 8 //Direção do motor 3
#define M3_PIN_STP 9 //Passos do motor 3

#define SPEED 200 //Definir velocidade
#define BUTTON_PIN A0 //Conectar pushbutton p/ dar início ao programa

#ifdef NEMA_17 //Definição do tipo de stepper
  
  AccelStepper m1(AccelStepper::FULL2WIRE, M1_PIN_STP,M1_PIN_DIR);
  AccelStepper m2(AccelStepper::FULL2WIRE, M2_PIN_STP,M2_PIN_DIR);
  AccelStepper m3(AccelStepper::FULL2WIRE, M3_PIN_STP,M3_PIN_DIR);
#else
  AccelStepper m1(4,6,8,7,9);
  AccelStepper m2(4,10,12,11,13);
  AccelStepper m3(4,2,4,3,5);
#endif

ETAPAS proxEtapa;
int qtd = 3; //Inserir a quantidade de cabos
int conta = 0;
char msg[100];

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

//Definição de velocidade e aceleração do motor 1
  m1.setMaxSpeed(100.0);
	m1.setAcceleration(100.0);
	m1.setSpeed(SPEED);

//Definição de velocidade e aceleração do motor 2
  m2.setMaxSpeed(100.0);
	m2.setAcceleration(50.0);
	m2.setSpeed(SPEED);

//Definição de velocidade e aceleração do motor 3
  m3.setMaxSpeed(100.0);
	m3.setAcceleration(100.0);
	m3.setSpeed(SPEED);
 

  proxEtapa = ETAPA_IDLE;
  conta = 0;
  //proxEtapa = ETAPA_TEST_M1;
  Serial.begin(9600);
  Serial.println("");
  Serial.flush();

}
void etapa0(){ //Sequência da etapa 0 - Calibração. Seta a posição inicial dos 3 motores. Lembrar de posicionar a lâmina manualmente antes de ligar
  m1.runToNewPosition(0);
  m2.runToNewPosition(0);
  m3.runToNewPosition(0);
}
void etapa1(){ //Sequência da etapa 1 - Avanço inicial de carregamento do cabo  
  //m1.setCurrentPosition(0);
  m1.runToNewPosition(800); // Especificar a distância entre o rolo 1 e a lâmina
}


void etapa2(){ //Sequência da etapa 2 - Corte inicial 
  m2.runToNewPosition(800); //Especificar a distância de movimento da lâmina para cortar
  m2.runToNewPosition(0); //Retorna ao ponto inicial
}
void etapa3(){ //Sequência da etapa 3 - Avanço do comprimento da ponta 1
  //m1.setCurrentPosition(0);
  //m3.setCurrentPosition(0);
  int move = 800; //Especificar a distância de movimento para a ponta 1

  m1.move(move);
  m3.move(move); 

  while (m1.distanceToGo() ) 
  {
    //Serial.println(m1.distanceToGo());
  
    m1.run();
    m3.run();
  }
}
void etapa4(){ //Sequência da etapa 4 - Decapagem da ponta 1
  //m2.setCurrentPosition(0);
  m2.runToNewPosition(800); //Especificar a distância de movimento da lâmina para marcar a decapagem
  m2.runToNewPosition(0);

}
void etapa5(){ //Sequência da etapa 5 - Avanço do comprimento do cabo
  
  int move = 800; //Especificar a distância de movimento para delimitar o comprimento do cabo, sem considerar as pontas
 // m1.setCurrentPosition(0);
  //m3.setCurrentPosition(0);
  m1.move(move);
  m3.move(move); 

  while (m1.distanceToGo() ) 
  {
    //Serial.println(m1.distanceToGo());
  
    m1.run();
    m3.run();
  }
}
void etapa6(){ //Sequência da etapa 6 - Decapagem da ponta 2
  //m2.setCurrentPosition(0);
  m2.runToNewPosition(800); //Especificar a distância de movimento da lâmina para marcar a decapagem da ponta 2
  m2.runToNewPosition(0);
}

void etapa7(){ //Sequência da etapa 7 - Avanço do comprimento da ponta 2
  //m1.setCurrentPosition(0);
  //m3.setCurrentPosition(0);
  int move = 800; //Especificar a distância de movimento para a ponta 2
  m1.move(move);
  m3.move(move);

  while(m1.distanceToGo()){
    m1.run();
    m3.run();
  }

}
void etapa8(){ //Sequência da etapa 8 - Corte final do cabo
   //m2.setCurrentPosition(0);
   m2.runToNewPosition(800); //Especificar a distância de movimento da lâmina para cortar
   m2.runToNewPosition(0);
}
void etapa9(){ //Sequência da etapa 9 - Remoção do cabo
  m3.setCurrentPosition(0);
  m3.runToNewPosition(800);
  m3.runToNewPosition(0);
}

void loop() {
  
  if (digitalRead(BUTTON_PIN) == 0 )
  {
    while(digitalRead(BUTTON_PIN) == 0);
    proxEtapa = ETAPA_00;
  }

  switch (proxEtapa){
    case ETAPA_00 :

      Serial.println("PASSO 0 - Seta posição inicial dos motores");
      etapa0();
      proxEtapa = ETAPA_01;
      break;

    case ETAPA_01: 

      Serial.println("PASSO 1 - Avanço inicial do cabo p/ calibração do corte inicial");
      etapa1();
      proxEtapa = ETAPA_02;
      break;

    case ETAPA_02: 

      Serial.println("PASSO 2 - Corte inicial");
      etapa2();
      proxEtapa = ETAPA_03;
      break;

    case ETAPA_03: 

      Serial.println("PASSO 3 - Avanço da ponta 1");
      etapa3();
      proxEtapa = ETAPA_04;
      break;

    case ETAPA_04:

      Serial.println("PASSO 4 - Decapagem da ponta 1");
      etapa4();
      proxEtapa = ETAPA_05;  
      break;

    case ETAPA_05:

      Serial.println("PASSO 5 - Avanço do comprimento do cabo");
      etapa5();
      proxEtapa = ETAPA_06;
      break;

    case ETAPA_06: 

      Serial.println("PASSO 6 - Decapagem da ponta 2");
      etapa6();
      proxEtapa = ETAPA_07;
      break;

    case ETAPA_07: 

      Serial.println("PASSO 7 - Avanço do comprimento da ponta 2");
      etapa7();
      proxEtapa = ETAPA_08;
      break;

    case ETAPA_08: 

      Serial.println("PASSO 8 - Corte final do cabo");
      etapa8();
      proxEtapa = ETAPA_09;
      break;

    case ETAPA_09:

      Serial.println("PASSO 9 - Remoção do cabo");
      etapa9();
      proxEtapa = ETAPA_10; 
      break;

    case ETAPA_10: 
      conta++;
      sprintf(msg,"PASSO 10!!, Peca %i de %i pronta. ",conta,qtd);
      Serial.println(msg); 
      if (conta == qtd )
        proxEtapa = ETAPA_IDLE;
      else
        proxEtapa = ETAPA_03;  
      break;
      
    case ETAPA_IDLE:
      Serial.println("ESPERANDO..."); 
      proxEtapa = ETAPA_IDLE;
      break;
      
    case ETAPA_TEST_M3:
      Serial.println("test m3");
      m3.runToNewPosition(2000);
      m3.runToNewPosition(0);
      proxEtapa = ETAPA_TEST_M1;
      break;

    case ETAPA_TEST_M2:
      Serial.println("test m2");
      m2.runToNewPosition(4000);
      m2.runToNewPosition(0);
      proxEtapa = ETAPA_TEST_M3;
      break;

    case ETAPA_TEST_M1:
      Serial.println("test m1");
      m1.runToNewPosition(50000);
      m1.runToNewPosition(0);
      proxEtapa = ETAPA_TEST_M1;
      break;
  }

}

