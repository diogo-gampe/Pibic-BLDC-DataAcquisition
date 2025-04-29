

import serial
import csv
from datetime import datetime
import time
import os

# Configura a porta serial
porta = 'COM7'  # Altere para a porta correta (ex: '/dev/ttyUSB0' no Linux)
baud_rate = 115200
timeout = 1  # tempo de espera para leitura

# Abre a porta serial
ser = serial.Serial(porta, baud_rate, timeout=timeout)

# Nome do arquivo CSV
arquivo_csv = 'dados_serial.csv'

tempo_inicial = time.time()

arquivo_existe = os.path.exists(arquivo_csv)
arquivo_vazio = not arquivo_existe or os.path.getsize(arquivo_csv) == 0

with open(arquivo_csv, mode='a', newline='') as arquivo:
    escritor = csv.writer(arquivo)
    escritor.writerow(['Timestamp', 'Dado'])

    print('Lendo dados da porta serial... Pressione Ctrl+C para parar.')

    try:
        while True:
            if ser.in_waiting:
                linha = ser.readline().decode('utf-8').strip()
                tempo_atual = time.time()
                timestamp = round(tempo_atual - tempo_inicial, 2)
                print(f"{timestamp} - {linha}")

                escritor.writerow([timestamp, linha])
    except KeyboardInterrupt:
        print('\nLeitura encerrada pelo usuário.')
    finally:
        ser.close()