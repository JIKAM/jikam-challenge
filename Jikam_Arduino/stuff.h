// definindo sentido do carinho
#define EM_FRENTE 1
#define GIRAR_DIREITA 2
#define PARA_TRAS 3
#define GIRAR_ESQUERDA 4
#define PEGAR_CARGA 5


#define POTENCIA_ROTACAO  100

// definindo sentidos do motor
#define SENTIDO_HORARIO "SENTIDO_HORARIO"
#define SENTIDO_ANTI_HORARIO "SENTIDO_ANTI_HORARIO"

struct SensorLinha {
  int direita;
  int meioDireita;
  int meio;
  int meioEsquerda;
  int esquerda;
};

struct Motores {
  int sentidoHorario;
  int sentidoAntiHorario;
  int potencia;
};
