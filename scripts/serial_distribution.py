import serial
import pandas as pd
import re
import time
import matplotlib.pyplot as plt

# Parâmetros da serial
porta_serial = 'COM3'  # Ajuste conforme seu sistema
baud_rate = 115200
ser = serial.Serial(
    porta_serial, baud_rate, timeout=1)


# Lista de pesos a testar
pesos = [400, 500, 600]  # Ajuste conforme NUM_PESOS
NUM_MEDICOES = 300 #DEVE SER IGUAL AO QUE CONSTA NO CÓDIGO DO ARDUINO getDataDist.ino
NUM_PESOS = 3 #DEVE SER IGUAL AO QUE CONSTA NO CÓDIGO DO ARDUINO getDataDist.ino

dados = []

def esperar_por(msg_esperada):
    while True:
        linha = ser.readline().decode().strip()
        if linha:
            print(f"[Arduino] {linha}")
            if msg_esperada in linha:
                return

def enviar_peso(peso):
    esperar_por("PESOS_READY")
    input(f"\n[Python] Coloque agora o peso de {peso} g na balança e pressione ENTER para continuar...")
    ser.write(f"{peso}\n".encode())
    print(f"[Python] Enviado peso: {peso}")

def ler_medicoes():
    esperar_por("MEDICOES_READY")
    peso_atual = None

    while True:
        linha = ser.readline().decode().strip()
        if not linha:
            continue

        if linha.startswith("Peso"):
            match = re.search(r"Peso (\d+)", linha)
            if match:
                peso_atual = int(match.group(1))

        elif re.match(r"^-?\d", linha):
            valores = [int(v.strip()) for v in linha.split(",") if v.strip().lstrip('-').isdigit()]
            if peso_atual is not None:
                for v in valores:
                    dados.append({"peso_g": peso_atual, "palavra_digital": v})
            if len([d for d in dados if d['peso_g'] == peso_atual]) >= NUM_MEDICOES:
                print(f"[Python] Coletadas {NUM_MEDICOES} medições para {peso_atual}g.")
                if len(set(d['peso_g'] for d in dados)) >= NUM_PESOS:
                    break

# Roda todo o protocolo
for peso in pesos:
    enviar_peso(peso)

ler_medicoes()
print(dados)
ser.close()

# Cria DataFrame
df = pd.DataFrame(dados)
print("\nAmostra dos dados:")
print(df.head())

# Salva CSV
df.to_csv("dados_sensor.csv", index=False)



#histogramas
fig, axs = plt.subplots(1, 3, figsize=(15, 4), sharey=True)  # 1 linha, 3 colunas
for i, peso in enumerate(pesos):
    subset = df[df["peso_g"] == peso]["palavra_digital"]
    axs[i].hist(subset, bins=15, color='skyblue', edgecolor='black')
    axs[i].set_title(f"{peso} g")
    axs[i].set_xlabel("Palavra Digital")
    axs[i].grid(True)

axs[0].set_ylabel("Frequência")
fig.suptitle("Distribuição das Palavras Digitais por Peso")
plt.tight_layout(rect=[0, 0, 1, 0.95])
plt.show()