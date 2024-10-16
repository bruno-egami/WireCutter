#include <AccelStepper.h>

// Definição dos estados do sistema
enum ETAPAS {ETAPA_00, ETAPA_01, ETAPA_02, ETAPA_03, ETAPA_04, ETAPA_05, ETAPA_06, ETAPA_07, ETAPA_08, ETAPA_09, ETAPA_10, ETAPA_TEST_M1, ETAPA_TEST_M2, ETAPA_TEST_M3, ETAPA_IDLE};

// Definição dos pinos
const int M1_PIN_DIR = 2; // Direção do motor 1
const int M1_PIN_STP = 3; // Passos do motor 1
const int M2_PIN_DIR = 4; // Direção do motor 2
const int M2_PIN_STP = 5; // Passos do motor 2
const int M3_PIN_DIR = 8; // Direção do motor 3
const int M3_PIN_STP = 9; // Passos do motor 3
const int BUTTON_PIN = A0; // Botão para iniciar o programa

// Parâmetros configuráveis
const int SPEED = 200; // Velocidade dos motores
const int QTD_CABOS = 3; // Quantidade de cabos a serem processados
const int DIST_CARREGAMENTO = 800; // Distância para carregamento inicial do cabo
const int DIST_CORTE = 800; // Distância de movimento da lâmina para cortar
const int DIST_PONTA = 800; // Distância de movimento para a ponta do cabo
const int DIST_DECAPAGEM = 800; // Distância de movimento da lâmina para marcar a decapagem
const int DIST_COMPRIMENTO_CABO = 800; // Distância de movimento para o comprimento do cabo
const int DIST_REMOCAO = 800; // Distância de movimento para remoção do cabo

// Configuração dos motores
AccelStepper m1(AccelStepper::FULL2WIRE, M1_PIN_STP, M1_PIN_DIR);
AccelStepper m2(AccelStepper::FULL2WIRE, M2_PIN_STP, M2_PIN_DIR);
AccelStepper m3(AccelStepper::FULL2WIRE, M3_PIN_STP, M3_PIN_DIR);

ETAPAS proxEtapa;
int conta = 0;
char msg[100];

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Configuração dos motores
  configureMotor(m1);
  configureMotor(m2);
  configureMotor(m3);

  proxEtapa = ETAPA_IDLE;
  Serial.begin(9600);
  Serial.println("Sistema inicializado. Pressione o botão para começar.");
}

void configureMotor(AccelStepper &motor) {
  motor.setMaxSpeed(100.0);
  motor.setAcceleration(100.0);
  motor.setSpeed(SPEED);
}

void moverMotores1e3(int passos) {
  m1.move(passos);
  m3.move(passos);
  while (m1.distanceToGo() || m3.distanceToGo()) {
    m1.run();
    m3.run();
  }
}

void etapa0() {
  Serial.println("ETAPA 0 - Calibração: Setando posição inicial dos motores");
  m1.runToNewPosition(0);
  m2.runToNewPosition(0);
  m3.runToNewPosition(0);
}

void etapa1() {
  Serial.println("ETAPA 1 - Avanço inicial de carregamento do cabo");
  m1.runToNewPosition(DIST_CARREGAMENTO);
}

void etapa2() {
  Serial.println("ETAPA 2 - Corte inicial");
  m2.runToNewPosition(DIST_CORTE);
  m2.runToNewPosition(0);
}

void etapa3() {
  Serial.println("ETAPA 3 - Avanço do comprimento da ponta 1");
  moverMotores1e3(DIST_PONTA);
}

void etapa4() {
  Serial.println("ETAPA 4 - Decapagem da ponta 1");
  m2.runToNewPosition(DIST_DECAPAGEM);
  m2.runToNewPosition(0);
}

void etapa5() {
  Serial.println("ETAPA 5 - Avanço do comprimento do cabo");
  moverMotores1e3(DIST_COMPRIMENTO_CABO);
}

void etapa6() {
  Serial.println("ETAPA 6 - Decapagem da ponta 2");
  m2.runToNewPosition(DIST_DECAPAGEM);
  m2.runToNewPosition(0);
}

void etapa7() {
  Serial.println("ETAPA 7 - Avanço do comprimento da ponta 2");
  moverMotores1e3(DIST_PONTA);
}

void etapa8() {
  Serial.println("ETAPA 8 - Corte final do cabo");
  m2.runToNewPosition(DIST_CORTE);
  m2.runToNewPosition(0);
}

void etapa9() {
  Serial.println("ETAPA 9 - Remoção do cabo");
  m3.runToNewPosition(DIST_REMOCAO);
  m3.runToNewPosition(0);
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    while (digitalRead(BUTTON_PIN) == LOW); // Debounce
    proxEtapa = ETAPA_00;
  }

  switch (proxEtapa) {
    case ETAPA_00:
      etapa0();
      proxEtapa = ETAPA_01;
      break;

    case ETAPA_01:
      etapa1();
      proxEtapa = ETAPA_02;
      break;

    case ETAPA_02:
      etapa2();
      proxEtapa = ETAPA_03;
      break;

    case ETAPA_03:
      etapa3();
      proxEtapa = ETAPA_04;
      break;

    case ETAPA_04:
      etapa4();
      proxEtapa = ETAPA_05;
      break;

    case ETAPA_05:
      etapa5();
      proxEtapa = ETAPA_06;
      break;

    case ETAPA_06:
      etapa6();
      proxEtapa = ETAPA_07;
      break;

    case ETAPA_07:
      etapa7();
      proxEtapa = ETAPA_08;
      break;

    case ETAPA_08:
      etapa8();
      proxEtapa = ETAPA_09;
      break;

    case ETAPA_09:
      etapa9();
      proxEtapa = ETAPA_10;
      break;

    case ETAPA_10:
      conta++;
      sprintf(msg, "ETAPA 10 - Cabo %d de %d pronto.", conta, QTD_CABOS);
      Serial.println(msg);
      if (conta == QTD_CABOS) {
        proxEtapa = ETAPA_IDLE;
        Serial.println("Todos os cabos processados. Pressione o botão para reiniciar.");
      } else {
        proxEtapa = ETAPA_03;
      }
      break;

    case ETAPA_IDLE:
      // Nada a fazer, esperando o botão ser pressionado
      break;

    default:
      Serial.println("Estado inválido. Reiniciando...");
      proxEtapa = ETAPA_IDLE;
      break;
  }
}
