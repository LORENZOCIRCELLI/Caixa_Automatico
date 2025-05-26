# 🛒 MicroCheckout – Sistema de Checkout com Arduino + Teclado Matricial

**MicroCheckout** é um projeto que simula um sistema de ponto de venda (PDV), como os usados em mercados reais. Utiliza um **Arduino Uno**, um **display LCD 16x2**, um **teclado matricial 4x4**, **buzzer**, **LEDs** e **botões físicos** para controlar e exibir a compra de produtos com base em códigos digitados.

---

## 🎯 Objetivo

Criar um sistema funcional de compras com:
- Entrada de produtos via teclado (em vez de RFID)
- Soma automática dos preços
- Opções de subtração, finalização e exibição do total
- Interface física intuitiva e barata para simular um checkout real

---

## 🧰 Componentes Utilizados

| Componente          | Função                                   |
|---------------------|-------------------------------------------|
| Arduino Uno         | Microcontrolador principal                |
| Teclado Matricial 4x4 | Entrada de dados (códigos dos produtos) |
| Display LCD 16x2 (I2C) | Exibição de informações                  |
| Buzzer              | Feedback sonoro                           |
| LEDs (verde/vermelho) | Status de operação                      |
| Botões físicos      | Remoção de item e finalização             |
| Protoboard e jumpers | Montagem física                          |

---

## 🛠 Funcionalidades Atuais

- Digitação do **código do produto** via teclado matricial
- Exibição do **nome** e **preço** no display LCD
- Cálculo automático do **total da compra**
- **Botão 1** para remover o último item digitado
- **Botão 2** para finalizar a compra (zera o total)
- Feedback com buzzer e LEDs em cada ação

---

## 💡 Como Funciona

1. Inicie o sistema.
2. Digite o código de um produto no teclado (ex: `101`).
3. O produto é buscado em uma lista pré-cadastrada no código.
4. Nome e preço aparecem no LCD e o total é atualizado.
5. Use os botões para remover produtos ou finalizar compra.

---

## 📁 Organização do Código

```c
struct Produto {
  int codigo;
  const char* nome;
  float preco;
};

Produto produtos[] = {
  {101, "Sabonete", 3.50},
  {102, "Arroz 5kg", 22.90},
  {103, "Leite 1L", 4.20},
  // ... adicione mais produtos
};

