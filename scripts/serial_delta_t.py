import serial
from openpyxl import Workbook
from datetime import datetime
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Configuração da porta serial (ajuste conforme necessário)
porta_serial = 'COM3'      # No Linux/Mac: '/dev/ttyUSB0' ou '/dev/ttyACM0'
baud_rate = 115200           # Deve ser igual ao do seu código no STM32

# Nome do arquivo Excel (com data/hora para evitar sobrescrita)
nome_arquivo = f"dados_celulas_{datetime.now().strftime('%Y%m%d_%H%M%S')}.xlsx"

# Cria um novo arquivo Excel e uma planilha ativa
wb = Workbook()
ws = wb.active
ws.title = "Dados das Células"
ws.append(["cel_med1", "tempo1", "cel_med2", "tempo2"])  # Cabeçalho

# Abre a porta serial
ser = serial.Serial(porta_serial, baud_rate, timeout=1)

print(f"Coletando dados e salvando em {nome_arquivo}...")
print("Pressione Ctrl+C para parar.")

try:
    while True:
        if ser.in_waiting > 0:
            linha = ser.readline().decode('utf-8').strip()
            if linha:
                dados = linha.split(',')
                if len(dados) == 4:
                    try:
                        # Converte os valores para números
                        cel_med1 = float(dados[0])  # ou int(dados[0]) se for inteiro
                        tempo1 = float(dados[1])
                        cel_med2 = float(dados[2])
                        tempo2 = float(dados[3])
                        
                        # Adiciona os valores numéricos ao Excel
                        ws.append([cel_med1, tempo1, cel_med2, tempo2])
                        print(f"Dados numéricos: {[cel_med1, tempo1, cel_med2, tempo2]}")
                        
                    except ValueError as e:
                        print(f"Erro na conversão: {e}. Linha ignorada: {linha}")
                else:
                    print(f"Formato inválido (ignorado): {linha}")

except KeyboardInterrupt:
    print("\nColeta encerrada.")
finally:
    ser.close()
    wb.save(nome_arquivo)  # Salva o arquivo

# Carregar dados do Excel
df = pd.read_excel(nome_arquivo)


# 2. Extrair colunas com alta precisão
tempos1 = df["tempo1"].values
celulas1 = df["cel_med1"].values
tempos2 = df["tempo2"].values
celulas2 = df["cel_med2"].values

# 3. Configurar o gráfico
plt.figure(1, figsize=(12, 6))
# Plotar Célula 1 (pontos azuis)
plt.scatter(
    tempos1, 
    celulas1, 
    label="Célula 1", 
    color="blue", 
    s=20, 
    marker= 'o', 
    alpha=1
)

# Plotar Célula 2 (pontos vermelhos)
plt.scatter(
    tempos2, 
    celulas2, 
    label="Célula 2", 
    color="red", 
    s=20, 
    marker='s', 
    alpha= 1
)

# 4. Ajustar eixos para alta resolução
plt.title("Palavra Digital Medida pelas Células")
plt.xlabel("Tempo (ms) - Escala de milisegundos")
plt.ylabel("Palavra digital (decimal)")
plt.legend()
plt.grid(True)

all_times = np.concatenate([tempos1, tempos2])
print(all_times)

plt.xticks(all_times, rotation=90)

# 5. Aplicar zoom em um intervalo específico (ex: 0.1s de dados)
tempo_inicial = min(tempos1[0], tempos2[0])  # Começo dos dados


plt.xlim(tempo_inicial - 10, tempo_inicial + 5000)  # Zoom nos primeiros 100ms
plt.ylim(0, 800000)

for t1, t2, y1, y2 in zip(tempos1, tempos2, celulas1, celulas2): 
    delta_t = abs(t2-t1)
    plt.annotate(f'Δt={delta_t}ms', 
                 xy=((t1 + t2)/2, (y1 + y2)/2),  # Posição da anotação (ponto médio)
                 xytext=(5, 5), 
                 textcoords='offset points',
                 fontsize=8,
                 bbox=dict(boxstyle='round,pad=0.5', fc='yellow', alpha=0.3))

print(df.head())
# 7. Salvar e mostrar
#plt.savefig("grafico_alta_resolucao.png", dpi=300)
plt.show()

h = 0.1024 #tempo de amostragem em segundos


#cria vetor de tempos de amostragem
T = np.arange(0, h, len(tempos1))

coef_1 = np.polyfit(tempos1, celulas1, deg=2)
regressao_1 = np.poly1d(coef_1)

coef_2 = np.polyfit(tempos2, celulas2, deg=2)
regressao_2 = np.poly1d(coef_2)

forcas_interp_1 = regressao_1(T)
forcas_interp_2 = regressao_2(T)

print(coef_1)
print(coef_2)
