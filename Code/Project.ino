#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

LiquidCrystal lcd(6, 7, 5, 4, 3, 2); 

char st[20];

int buzzer = 8;
int ledVerde = A0;  // LED verde para soma
int ledVermelho = A1;  // LED vermelho para subtração
int botaoPin = A2;  // Pino onde o botão de alternar entre soma e subtração está conectado
int botaoPagamentoPin = A3;  // Pino onde o botão de pagamento está conectado

float total = 0;
bool modoSoma = true;  // Inicia no modo de soma
unsigned long ultimaLeitura = 0;  // Para evitar múltiplas leituras rápidas do botão

// Array para controlar itens adicionados
float itemHistorico[10];  // Limite de 10 itens no histórico, você pode aumentar se necessário
int historicoIndex = 0;   // Índice do próximo item a ser adicionado ao histórico

// Estrutura para produtos
struct Produto {
  String uid;
  String nome;
  float preco;
};

Produto produtos[] = {
  {"04 52 F6 A4 6D 26 81", "Creatina Growth", 100.00},
  {"04 2C E9 A4 6D 26 81", "Gelatina Sab. Abacaxi", 1.50},
  {"04 E8 DB A4 6D 26 81", "Detergente 500ml", 1.79},
  {"04 14 E3 A4 6D 26 81", "Nescau em pó", 8.99},
  {"04 CC D5 A4 6D 26 81", "Goiabada Xavante", 16.90},
  {"04 B1 CE A4 6D 26 81", "Macarrão Espaguete", 10.00},
  {"04 8F CB A4 6D 26 81", "Produto 7", 5.00},  // Adicione seu produto
  {"04 C7 D8 A4 6D 26 81", "Produto 8", 20.00}   // Adicione seu produto
};

void setup() 
{
  Serial.begin(9600);   // Inicia a serial
  SPI.begin();      // Inicia SPI bus
  mfrc522.PCD_Init();   // Inicia MFRC522
  Serial.println("Aproxime o seu cartao do leitor...");
  Serial.println();
  lcd.begin(16, 2);  
  mensageminicial();

  pinMode(buzzer, OUTPUT);
  pinMode(ledVerde, OUTPUT);
  pinMode(ledVermelho, OUTPUT);  // Configura o LED vermelho como saída
  pinMode(botaoPin, INPUT_PULLUP);  // Configura o botão de soma/subtração como entrada com pull-up interno
  pinMode(botaoPagamentoPin, INPUT_PULLUP);  // Configura o botão de pagamento como entrada com pull-up interno
}

void loop() 
{
  // Verifica se o botão de modo foi pressionado (transição de HIGH para LOW)
  if (digitalRead(botaoPin) == LOW && millis() - ultimaLeitura > 300) {
    ultimaLeitura = millis();
    modoSoma = !modoSoma;  // Alterna o modo entre soma e subtração
    if (modoSoma) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Modo: Adicao");
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Modo: Subtracao");
    }
    delay(1000);  // Pausa para evitar múltiplos registros de pressionamento
  }

  // Verifica se o botão de pagamento foi pressionado
  if (digitalRead(botaoPagamentoPin) == LOW && millis() - ultimaLeitura > 300) {
    ultimaLeitura = millis();
    telaPagamento();  // Chama a função de pagamento
  }

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }

  // Mostra UID na serial
  Serial.print("UID da tag :");
  String conteudo = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  conteudo.toUpperCase();
  Serial.print("Mensagem : ");
  Serial.println(conteudo);

  // Loop pelos produtos para verificar se há correspondência
  for (int i = 0; i < 8; i++) {
    if (conteudo.substring(1) == produtos[i].uid) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(produtos[i].nome);

      if (modoSoma) {
        // Adiciona o item ao histórico e ao total
        if (historicoIndex < 10) {  // Verifica se há espaço no histórico
          itemHistorico[historicoIndex] = produtos[i].preco;  // Valor do produto
          historicoIndex++;
          total = total + produtos[i].preco;
          digitalWrite(ledVerde, HIGH);  // Acende o LED verde (soma)
          digitalWrite(ledVermelho, LOW);  // Desliga o LED vermelho
          delay(500);  // Mantém o LED verde aceso por 500 ms
          digitalWrite(ledVerde, LOW);  // Apaga o LED verde
        }
      } else {
        // Remover item, apenas se ele foi previamente adicionado e o total não for negativo
        if (historicoIndex > 0 && total > 0) {
          historicoIndex--;
          total = total - itemHistorico[historicoIndex];  // Subtrai o valor do item
          digitalWrite(ledVermelho, HIGH);  // Acende o LED vermelho (subtração)
          digitalWrite(ledVerde, LOW);  // Desliga o LED verde
          delay(500);  // Mantém o LED vermelho aceso por 500 ms
          digitalWrite(ledVermelho, LOW);  // Apaga o LED vermelho
        }
      }

      lcd.setCursor(0, 1);
      lcd.print("Total: $");
      lcd.print(total, 2);

      tone(buzzer, 1000);  // Frequência do bip (1000 Hz)
      delay(500);          // Duração do bip (500 ms)
      noTone(buzzer);

      delay(1500);
      mensagemseguinte();
      break;  // Sai do loop se um produto foi encontrado
    }
  }
}

// Função de exibição da tela de pagamento
void telaPagamento()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pagamento");

  // Mostra o total
  lcd.setCursor(0, 1);
  lcd.print("Total: $");
  lcd.print(total, 2);

  // Espera a leitura do cartão de pagamento fictício
  lcd.setCursor(0, 1);
  lcd.print("Passar Cartao");

  while (true) {
    // Look for the payment card (fictício)
    if ( ! mfrc522.PICC_IsNewCardPresent()) 
    {
      continue;
    }

    if ( ! mfrc522.PICC_ReadCardSerial()) 
    {
      continue;
    }

    String conteudo = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) 
    {
      conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    conteudo.toUpperCase();
    Serial.println("Cartao de pagamento detectado!");

    // Verifica se o cartão fictício foi passado (defina o UID como o do seu "cartão fictício")
    if (conteudo.substring(1) == "E3 21 C9 11") {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Pagamento OK!");
      lcd.setCursor(0, 1);
      lcd.print("Total: $");
      lcd.print(total, 2);
      delay(2000);  // Exibe a mensagem por 2 segundos

      bipDeConfirmacao();  // Toca o bip longo de confirmação

      encerramentoCaixa();  // Chama a função de encerramento
      return;
    }
  }
}

// Função para exibir a mensagem de encerramento do caixa
void encerramentoCaixa()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Fechamento Caixa");
  lcd.setCursor(0, 1);
  lcd.print("Total: $");
  lcd.print(total, 2);
  delay(3000);  // Exibe por 3 segundos
  total = 0;    // Reseta o total
  historicoIndex = 0;  // Reseta o histórico
}

// Função para tocar o bip longo de confirmação
void bipDeConfirmacao() {
  tone(buzzer, 1000);  // Toca o bip com 1000 Hz
  delay(1500);         // Duração do bip (1500 ms ou 1.5 segundos)
  noTone(buzzer);      // Para o som
}

// Função para exibir a mensagem inicial
void mensageminicial() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sistema de Caixa");
  lcd.setCursor(0, 1);
  lcd.print("Aproxime seu Cartao");
  delay(2000);
}

// Função para exibir a mensagem de próxima tela
void mensagemseguinte() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sistema de Caixa");
  lcd.setCursor(0, 1);
  lcd.print("Aproxime seu Cartao");
  delay(2000);
}
