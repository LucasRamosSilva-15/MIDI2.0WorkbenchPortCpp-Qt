# Bateria de Testes MVP UMP Parser (Offline)

Abaixo estão exemplos manuais segmentados por nível de confiabilidade. Copie e cole os valores hexadecimais no campo "Hex UMP" do aplicativo e clique em "Interpretar".

## 🟢 Testes Confirmados (Decodificação 100% Precisa)

### 1. Mensagem UMP de 32 bits (MIDI 1.0 Note On)
**Entrada:** `20904000`
**Comportamento Esperado:**
- Size: 32 bits
- Type: MIDI 1.0 (0x2)
- Description: Note On (Note: 64, Vel: 0)

### 2. Mensagem UMP de 64 bits (MIDI 2.0 Note On)
**Entrada:** `40904000 40000000`
**Comportamento Esperado:**
- Size: 64 bits
- Type: MIDI 2.0 (0x4)
- Description: Note On (Note: 64, Vel 16b: 16384, Atributos: não detalhados)

### 3. Mensagem UMP de 32 bits (MIDI 1.0 Control Change)
**Entrada:** `20B0077F`
**Comportamento Esperado:**
- Size: 32 bits
- Type: MIDI 1.0 (0x2)
- Description: Control Change (CC: 7, Val: 127)

### 4. Mensagem UMP de 64 bits (MIDI 2.0 Control Change)
**Entrada:** `40B00700 7FFFFFFF`
**Comportamento Esperado:**
- Size: 64 bits
- Type: MIDI 2.0 (0x4)
- Description: Control Change (CC: 7, Val 32b: 2147483647)

### 5. Mensagem UMP de 64 bits (MIDI 2.0 Pitch Bend)
**Entrada:** `40E00000 80000000`
**Comportamento Esperado:**
- Size: 64 bits
- Type: MIDI 2.0 (0x4)
- Description: Pitch Bend (Val 32b: 2147483648)

---

## 🟡 Testes Parciais ou Não Detalhados (Foco em Segurança)

### 6. Mensagem UMP de 64 bits (MIDI 2.0 Program Change)
**Entrada:** `40C00000 05000000`
**Comportamento Esperado:**
- Description: Program Change (parcial)
*(O parser identifica que é um PC, mas se recusa a extrair o número do banco por não ter validação da especificação inteira).*

### 7. Mensagem UMP de 64 bits (MIDI 2.0 Per-Note Controller)
**Entrada:** `40004000 00000000`
**Comportamento Esperado:**
- Description: Per-Note Message (parcial/não detalhado)

### 8. Mensagem UMP de 64 bits (SysEx7)
**Entrada:** `30000000 11223344`
**Comportamento Esperado:**
- Type: SysEx7 (0x3)
- Description: Data Message (SysEx7, Payload bruto: 11223344)

### 9. Mensagem UMP de 128 bits (SysEx8 / MDS)
**Entrada:** `50000000 11223344 55667788 99AABBCC`
**Comportamento Esperado:**
- Type: SysEx8/MDS (0x5)
- Description: Data Message (SysEx8/MDS, Payload bruto: 11223344 55667788 99AABBCC)

### 10. Mensagem UMP de 128 bits (Flex Data)
**Entrada:** `D0000000 11223344 55667788 99AABBCC`
**Comportamento Esperado:**
- Type: Flex Data (0xD)
- Description: Flex Data (parcial/não detalhado, Payload bruto: 11223344 55667788 99AABBCC)

### 11. Mensagem UMP de 128 bits (Endpoint / Stream)
**Entrada:** `F0000000 11223344 55667788 99AABBCC`
**Comportamento Esperado:**
- Type: Stream/Endpoint (0xF)
- Description: UMP Stream Message (Status bruto: 0x000, Payload bruto: 11223344 55667788 99AABBCC, parcial/não detalhado)

### 12. Mensagem UMP de 96 bits (Reservada / Futura)
**Entrada:** `B0000000 00000000 00000000`
**Comportamento Esperado:**
- Type: Reserved (0xB)
- Status/Channel/Group: - (Não aplicáveis)
- Description: Reserved / Future Use (Não interpretado)

---

## 🔴 Testes de Tratamento de Erros

### 13. Entrada Incompleta (Buffer sob fluxo)
**Entrada:** `40904000` (Um pacote de 64 bits cortado na metade)
**Comportamento:** O pacote é descartado para proteger o parser e o Log reporta: `Erro de empacotamento UMP: O Message Type 0x4 determina um pacote...`

### 14. Entrada Inválida (Corrupção de texto)
**Entrada:** `LIXO_AQUI_TESTE`
**Comportamento:** O Log aborta o parse dizendo: `Erro Crítico: A string 'LIXO_AQU' contém texto ou caracteres hexadecimais inválidos.`
