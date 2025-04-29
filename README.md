
**Plataforma de Medição de Empuxo e Torque para Motores BLDC**

Este repositório contém o código-fonte do projeto de iniciação científica voltado para o desenvolvimento de uma plataforma física capaz de medir o empuxo e o torque gerados por motores BLDC (Brushless DC). O sistema utiliza sensores de carga e eletrônica embarcada para aquisição e posterior processamento dos dados.

 **Descrição do Projeto**

A plataforma desenvolvida possui 6 células de carga estrategicamente posicionadas para permitir a medição precisa das forças e torques gerados por motores BLDC em teste. A aquisição dos dados é realizada por um microcontrolador STM32, que se comunica com os sensores por meio do conversor HX711, especializado na leitura de sinais de baixa amplitude provenientes das células de carga.

O projeto envolve:

  - Construção de uma estrutura física para fixação de motores e sensores;

  - Leitura e digitalização dos sinais provenientes das células de carga através de módulos HX711;

  - Comunicação entre o STM32 e um computador host;

  - Armazenamento e processamento dos dados obtidos.

**Status do Projeto**

Em desenvolvimento – Novas funcionalidades e melhorias estão sendo adicionadas continuamente.

 **Autores**
 
Este projeto é parte de um trabalho de Iniciação Científica desenvolvido por Diogo Moraes, Diogo Cajueiro e João Neto pela UFPE, sob orientação de Professor Saulo Dornellas.
