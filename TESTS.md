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

### 11. Mensagem UMP de 128 bits (Endpoint Discovery)
**Entrada:** `F0000000 11223344 55667788 99AABBCC`
**Comportamento Esperado:**
- Type: Stream/Endpoint (0xF)
- Description: UMP Stream: Endpoint Discovery (Form: Complete, Status bruto: 0x000, Payload bruto: 11223344 55667788 99AABBCC, parcial/não detalhado)

### 12. Mensagem UMP de 128 bits (Validação Específica de Endpoint Info Notification)
**Nota:** Este teste valida os bits internos que só existem em notificações de informações de Endpoint (Status 0x001), e não em mensagens de Stream genéricas.
**Entrada:** `F0010101 82000303 00000000 00000000`
**Comportamento Esperado:**
- Type: Stream/Endpoint (0xF)
- Description: UMP Stream: Endpoint Info Notification [UMP v1.1, FuncBlocks: 2 (Static: 1), M2: 1, M1: 1, JRR: 1, JRT: 1] (Form: Complete, Status bruto: 0x001, Payload bruto: 82000303 00000000 00000000, parcial/não detalhado)

### 13. Mensagem UMP de 128 bits (Validação Específica de Device Identity Notification)
**Nota:** Este teste garante a visibilidade dos blocos hexadecimais brutos de Fabricante, Família, Modelo e Revisão contidos em Status 0x002.
**Entrada:** `F0020000 00414243 44454647 48494A4B`
**Comportamento Esperado:**
- Type: Stream/Endpoint (0xF)
- Description: UMP Stream: Device Identity Notification [MFR: 41-42-43, FAM: 44-45, MOD: 46-47, REV: 48-49-4A-4B] (Form: Complete, Status bruto: 0x002, Payload bruto: 00414243 44454647 48494A4B, parcial/não detalhado)

### 14. Mensagem UMP de 128 bits (Validação de Máscara de 7 bits em Device Identity)
**Nota:** Testa o filtro 0x7F. Embora a entrada possua bytes 0xFF, eles devem ser limitados a 0x7F na seção extraída, mas continuar 0xFF no Payload bruto.
**Entrada:** `F0020000 00FFFFFF FFFFFFFF FFFFFFFF`
**Comportamento Esperado:**
- Type: Stream/Endpoint (0xF)
- Description: UMP Stream: Device Identity Notification [MFR: 7F-7F-7F, FAM: 7F-7F, MOD: 7F-7F, REV: 7F-7F-7F-7F] (Form: Complete, Status bruto: 0x002, Payload bruto: 00FFFFFF FFFFFFFF FFFFFFFF, parcial/não detalhado)

### 15. Mensagem UMP de 128 bits (Stream Configuration Request)
**Entrada:** `F0050203 11223344 55667788 99AABBCC`
**Comportamento Esperado:**
- Type: Stream/Endpoint (0xF)
- Description: UMP Stream: Stream Configuration Request [Protocol: 2 (MIDI 2.0), JR_RX: 1, JR_TX: 1] (Form: Complete, Status bruto: 0x005, Payload bruto: 11223344 55667788 99AABBCC, parcial/não detalhado)

### 16. Mensagem UMP de 128 bits (Stream Configuration Notification)
**Entrada:** `FC060100 00000000 00000000 00000000`
**Comportamento Esperado:**
- Type: Stream/Endpoint (0xF)
- Description: UMP Stream: Stream Configuration Notification [Protocol: 1 (MIDI 1.0), JR_RX: 0, JR_TX: 0] (Form: End, Status bruto: 0x006, Payload bruto: 00000000 00000000 00000000, parcial/não detalhado)

### 17. Mensagem UMP de 128 bits (Function Block Discovery)
**Entrada:** `F010FF03 00000000 00000000 00000000`
**Comportamento Esperado:**
- Type: Stream/Endpoint (0xF)
- Description: UMP Stream: Function Block Discovery [FB#: 255, ReqName: 1, ReqInfo: 1] (Form: Complete, Status bruto: 0x010, Payload bruto: 00000000 00000000 00000000, parcial/não detalhado)

### 18. Mensagem UMP de 128 bits (Function Block Info Notification)
**Entrada:** `F0118311 02040100 00000000 00000000`
**Comportamento Esperado:**
- Type: Stream/Endpoint (0xF)
- Description: UMP Stream: Function Block Info Notification [FB#: 3, Active: 1, Dir: 1, UI: 1, M1: 0 | Grp: 2, Len: 4, CI: 1, SysEx: 0] (Form: Complete, Status bruto: 0x011, Payload bruto: 02040100 00000000 00000000, parcial/não detalhado)

> **Nota sobre Extração de Nomes (Endpoint / Function Block):** O parser é estritamente *stateless* (sem acúmulo de estado). Ele não tentará remontar buffers de pacotes em pedaços (fragmentos). A saída textual extrairá puramente caracteres ASCII impressos contidos APENAS naquele pacote, adicionando a tag `(Fragmentado)` se aplicável. O `Payload bruto` nunca será alterado.

### 19. Mensagem UMP de 128 bits (Function Block Name Notification)
**Entrada:** `F0120100 4B6F7267 204B726F 6E6F7300`
**Comportamento Esperado:**
- Type: Stream/Endpoint (0xF)
- Description: UMP Stream: Function Block Name Notification [FB#: 1, ASCII filtrado: 'Korg Kronos'] (Form: Complete, Status bruto: 0x012, Payload bruto: 4B6F7267 204B726F 6E6F7300, parcial/não detalhado)

### 20. Mensagem UMP de 128 bits (Endpoint Name Notification)
**Entrada:** `F0034B6F 7267204B 726F6E6F 73000000`
**Comportamento Esperado:**
- Type: Stream/Endpoint (0xF)
- Description: UMP Stream: Endpoint Name Notification [ASCII filtrado: 'Korg Kronos'] (Form: Complete, Status bruto: 0x003, Payload bruto: 7267204B 726F6E6F 73000000, parcial/não detalhado)

### 21. Mensagem UMP de 96 bits (Reservada / Futura)
**Entrada:** `B0000000 00000000 00000000`
**Comportamento Esperado:**
- Type: Reserved (0xB)
- Status/Channel/Group: - (Não aplicáveis)
- Description: Reserved / Future Use (Não interpretado)

---

## 🔴 Testes de Tratamento de Erros

### 14. Entrada Incompleta (Buffer sob fluxo)
**Entrada:** `40904000` (Um pacote de 64 bits cortado na metade)
**Comportamento:** O pacote é descartado para proteger o parser e o Log reporta: `Erro de empacotamento UMP: O Message Type 0x4 determina um pacote...`

### 15. Entrada Inválida (Corrupção de texto)
**Entrada:** `LIXO_AQUI_TESTE`
**Comportamento:** O Log aborta o parse dizendo: `Erro Crítico: A string 'LIXO_AQU' contém texto ou caracteres hexadecimais inválidos.`
