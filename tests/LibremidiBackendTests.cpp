#include "LibremidiBackendTests.h"
#include "TestUtils.h"
#include "../src/midi/LibremidiBackend.h"
#include "../src/midi/FakeUmpInputBackend.h"
#include "../src/midi/RtMidiInputBackend.h"
#include "../src/midi/MidiInputController.h"
#include "../src/midi/Midi1LiveDecoder.h"
#include "../src/UmpParser.h"
#include <limits>
#include <iostream>
#include <thread>
#include <vector>

void runLibremidiBackendTests() {
    std::cout << "\nStarting LibremidiBackend & UMP Pipeline Test Suite\n" << std::endl;

    // =========================================================================
    // 1. LibremidiBackend - Robustez de Portas e Ciclo de Vida
    // =========================================================================
    {
        LibremidiBackend backend;

        // 1.1 Estado inicial
        assertTest("Libremidi Initial State Closed", !backend.isOpen());
        assertTest("Libremidi Initial Event Queue Empty", backend.pollUmpEvents().empty());
        assertTest("Libremidi Backend Name", backend.backendName() == "libremidi (Cross-Platform UMP)");

        // 1.2 Tentativa de abertura com índices inválidos garantindo retorno false sem crash
        assertTest("Libremidi Open Negative Index -1", !backend.openInputPort(-1));
        assertTest("Libremidi Open Negative Index -9999", !backend.openInputPort(-9999));
        assertTest("Libremidi Open Large Out-of-Bounds 99999", !backend.openInputPort(99999));
        assertTest("Libremidi Open Int Max Index", !backend.openInputPort(std::numeric_limits<int>::max()));
        assertTest("Libremidi Open Int Min Index", !backend.openInputPort(std::numeric_limits<int>::min()));
        assertTest("Libremidi State Remains Closed After Invalid Opens", !backend.isOpen());
        assertTest("Libremidi Event Queue Still Empty After Invalid Opens", backend.pollUmpEvents().empty());

        // 1.3 Chamadas consecutivas de closeInputPort() sem efeitos colaterais
        backend.closeInputPort();
        assertTest("Libremidi Consecutive Close 1 Safe", !backend.isOpen());
        backend.closeInputPort();
        assertTest("Libremidi Consecutive Close 2 Safe", !backend.isOpen());
        backend.closeInputPort();
        assertTest("Libremidi Consecutive Close 3 Safe", !backend.isOpen());
        assertTest("Libremidi Queue Safe After Multiple Closes", backend.pollUmpEvents().empty());

        // 1.4 Listagem de portas segura
        bool listingThrew = false;
        QStringList ports;
        try {
            ports = backend.listInputPorts();
        } catch (...) {
            listingThrew = true;
        }
        assertTest("Libremidi listInputPorts Safe", !listingThrew);

        // 1.5 Validação dos prefixos de categorização visual em portas reais (se existirem)
        for (int i = 0; i < ports.size(); ++i) {
            bool validPrefix = ports[i].startsWith("[UMP] ") || ports[i].startsWith("[MIDI 1.0] ");
            assertTest(std::string("Libremidi Real Port Prefix Check #") + std::to_string(i), validPrefix);
        }

        // 1.6 Validação determinística de getPortDisplayName com mocks estruturados
        {
            libremidi::input_port umpPort;
            umpPort.api = libremidi::API::ALSA_SEQ_UMP;
            umpPort.display_name = "Test Native UMP Endpoint";
            QString nameUmp = LibremidiBackend::getPortDisplayName(umpPort, 0);
            assertTest("Libremidi Mock Port UMP Prefix", nameUmp.startsWith("[UMP] "));
            assertTest("Libremidi Mock Port UMP Name Content", nameUmp.contains("Test Native UMP Endpoint"));

            libremidi::input_port midi1Port;
            midi1Port.api = libremidi::API::ALSA_SEQ;
            midi1Port.display_name = "Standard MIDI 1 Keyboard";
            QString nameMidi1 = LibremidiBackend::getPortDisplayName(midi1Port, 0);
            assertTest("Libremidi Mock Port MIDI 1.0 Prefix", nameMidi1.startsWith("[MIDI 1.0] "));
            assertTest("Libremidi Mock Port MIDI 1.0 Content", nameMidi1.contains("Standard MIDI 1 Keyboard"));

            // Nomes genéricos (ex: "input", "capture") combinados com device_name
            libremidi::input_port devPort;
            devPort.api = libremidi::API::ALSA_SEQ;
            devPort.device_name = "USB Midi Device";
            devPort.port_name = "input";
            QString nameDev = LibremidiBackend::getPortDisplayName(devPort, 0);
            assertTest("Libremidi Mock Port Generic Name Normalized", nameDev.contains("USB Midi Device"));

            // Fallback quando não há nome algum
            libremidi::input_port emptyPort;
            emptyPort.api = libremidi::API::PIPEWIRE;
            QString nameFallback = LibremidiBackend::getPortDisplayName(emptyPort, 4);
            assertTest("Libremidi Mock Port Fallback Formatted", nameFallback == "[MIDI 1.0] Port 5");
        }

        // 1.7 Abertura e fechamento de porta real disponível (se o host possuir portas no sistema)
        if (!ports.isEmpty()) {
            bool opened = backend.openInputPort(0);
            assertTest("Libremidi Open Discovered Port 0", opened);
            assertTest("Libremidi Port Is Open Verified", backend.isOpen());
            backend.closeInputPort();
            assertTest("Libremidi Closed Verified", !backend.isOpen());
        }
    }

    // =========================================================================
    // 2. Conversão MIDI 1.0 -> UMP (Pipeline de Entrada)
    // =========================================================================
    {
        // 2.1 Channel Voice -> UMP MT 0x2 (Formato 0x20000000 | (group<<24) | (status<<16) | (d1<<8) | d2)
        uint32_t noteOnWord = LibremidiBackend::convertMidi1BytesToUmpWord({0x90, 0x3C, 0x64}, 0);
        assertTest("Convert MIDI 1.0 Note On to UMP MT 0x2", noteOnWord == 0x20903C64);

        uint32_t noteOffWord = LibremidiBackend::convertMidi1BytesToUmpWord({0x80, 0x3C, 0x40}, 0);
        assertTest("Convert MIDI 1.0 Note Off to UMP MT 0x2", noteOffWord == 0x20803C40);

        uint32_t polyAtWord = LibremidiBackend::convertMidi1BytesToUmpWord({0xA1, 0x3C, 0x50}, 0);
        assertTest("Convert MIDI 1.0 Poly Aftertouch Ch 2", polyAtWord == 0x20A13C50);

        uint32_t ccWord = LibremidiBackend::convertMidi1BytesToUmpWord({0xB2, 0x07, 0x7F}, 0);
        assertTest("Convert MIDI 1.0 CC Ch 3 to UMP MT 0x2", ccWord == 0x20B2077F);

        uint32_t pcWord = LibremidiBackend::convertMidi1BytesToUmpWord({0xC3, 0x05}, 0);
        assertTest("Convert MIDI 1.0 Program Change Ch 4 (2 bytes)", pcWord == 0x20C30500);

        uint32_t chAtWord = LibremidiBackend::convertMidi1BytesToUmpWord({0xD4, 0x28}, 0);
        assertTest("Convert MIDI 1.0 Channel Pressure Ch 5 (2 bytes)", chAtWord == 0x20D42800);

        uint32_t pbWord = LibremidiBackend::convertMidi1BytesToUmpWord({0xE5, 0x00, 0x40}, 0);
        assertTest("Convert MIDI 1.0 Pitch Bend Ch 6", pbWord == 0x20E50040);

        uint32_t pbMaxWord = LibremidiBackend::convertMidi1BytesToUmpWord({0xEF, 0x7F, 0x7F}, 0);
        assertTest("Convert MIDI 1.0 Pitch Bend Max Ch 16", pbMaxWord == 0x20EF7F7F);

        // Teste de Grupos UMP (0 a 15)
        uint32_t group1Word = LibremidiBackend::convertMidi1BytesToUmpWord({0x90, 0x3C, 0x64}, 1);
        assertTest("Convert MIDI 1.0 with Group 1", group1Word == 0x21903C64);

        uint32_t group5Word = LibremidiBackend::convertMidi1BytesToUmpWord({0x90, 0x3C, 0x64}, 5);
        assertTest("Convert MIDI 1.0 with Group 5", group5Word == 0x25903C64);

        uint32_t group15Word = LibremidiBackend::convertMidi1BytesToUmpWord({0x90, 0x3C, 0x64}, 15);
        assertTest("Convert MIDI 1.0 with Max Group 15", group15Word == 0x2F903C64);

        // 2.2 System Real-Time / Common -> UMP MT 0x1 (Formato 0x10000000 | (group<<24) | (status<<16) | (d1<<8) | d2)
        uint32_t clockWord = LibremidiBackend::convertMidi1BytesToUmpWord({0xF8}, 0);
        assertTest("Convert Timing Clock to UMP MT 0x1", clockWord == 0x10F80000);

        uint32_t startWord = LibremidiBackend::convertMidi1BytesToUmpWord({0xFA}, 0);
        assertTest("Convert Start to UMP MT 0x1", startWord == 0x10FA0000);

        uint32_t continueWord = LibremidiBackend::convertMidi1BytesToUmpWord({0xFB}, 0);
        assertTest("Convert Continue to UMP MT 0x1", continueWord == 0x10FB0000);

        uint32_t stopWord = LibremidiBackend::convertMidi1BytesToUmpWord({0xFC}, 0);
        assertTest("Convert Stop to UMP MT 0x1", stopWord == 0x10FC0000);

        uint32_t sensingWord = LibremidiBackend::convertMidi1BytesToUmpWord({0xFE}, 0);
        assertTest("Convert Active Sensing to UMP MT 0x1", sensingWord == 0x10FE0000);

        uint32_t resetWord = LibremidiBackend::convertMidi1BytesToUmpWord({0xFF}, 0);
        assertTest("Convert System Reset to UMP MT 0x1", resetWord == 0x10FF0000);

        uint32_t sppWord = LibremidiBackend::convertMidi1BytesToUmpWord({0xF2, 0x10, 0x20}, 0);
        assertTest("Convert Song Position Pointer to UMP MT 0x1", sppWord == 0x10F21020);

        uint32_t songSelectWord = LibremidiBackend::convertMidi1BytesToUmpWord({0xF3, 0x05}, 0);
        assertTest("Convert Song Select to UMP MT 0x1", songSelectWord == 0x10F30500);

        uint32_t tuneReqWord = LibremidiBackend::convertMidi1BytesToUmpWord({0xF6}, 0);
        assertTest("Convert Tune Request to UMP MT 0x1", tuneReqWord == 0x10F60000);

        uint32_t mtcWord = LibremidiBackend::convertMidi1BytesToUmpWord({0xF1, 0x7F}, 0);
        assertTest("Convert MTC Quarter Frame to UMP MT 0x1", mtcWord == 0x10F17F00);

        uint32_t sysGroupWord = LibremidiBackend::convertMidi1BytesToUmpWord({0xF8}, 4);
        assertTest("Convert System Message with Group 4", sysGroupWord == 0x14F80000);

        // 2.3 Casos inválidos e de borda
        uint32_t invalidStatusWord = LibremidiBackend::convertMidi1BytesToUmpWord({0x7F, 0x00}, 0);
        assertTest("Convert Invalid Status Byte (<0x80) Returns 0", invalidStatusWord == 0);

        uint32_t emptyBytesWord = LibremidiBackend::convertMidi1BytesToUmpWord({}, 0);
        assertTest("Convert Empty Bytes Returns 0", emptyBytesWord == 0);

        // 2.4 Tratamento de Note On com velocity zero
        uint32_t noteOnZeroVelWord = LibremidiBackend::convertMidi1BytesToUmpWord({0x90, 0x3C, 0x00}, 0);
        assertTest("Note On Velocity Zero Preserves UMP Format", noteOnZeroVelWord == 0x20903C00);

        auto decRaw = Midi1LiveDecoder::decodeDetailed({0x90, 0x3C, 0x00}, false);
        assertTest("Decoder Velocity Zero Without Flag Is Note On", decRaw.messageType == "Note On");

        auto decTreated = Midi1LiveDecoder::decodeDetailed({0x90, 0x3C, 0x00}, true);
        assertTest("Decoder Velocity Zero With Flag Is Note Off", decTreated.messageType == "Note Off");
        assertTest("Decoder Velocity Zero Description Contains Note Off", decTreated.description.contains("Note Off"));

        auto decPositive = Midi1LiveDecoder::decodeDetailed({0x90, 0x3C, 0x40}, true);
        assertTest("Decoder Positive Velocity Remains Note On", decPositive.messageType == "Note On");

        // 2.5 Validação dos pacotes UMP convertidos através do UmpParser
        auto parsedNoteOn = UmpParser::parseMessage({noteOnWord});
        assertTest("UmpParser Recognizes Converted Note On as MT 0x2", parsedNoteOn.messageType == 2);
        assertTest("UmpParser Status Check Note On (0x9)", parsedNoteOn.status == 0x9);
        assertTest("UmpParser Channel Check Note On (0-index Ch 1)", parsedNoteOn.channel == 0);

        auto parsedClock = UmpParser::parseMessage({clockWord});
        assertTest("UmpParser Recognizes Converted Clock as MT 0x1", parsedClock.messageType == 1);
        assertTest("UmpParser Description Check Clock", parsedClock.description == "System Real Time / System Common");

        auto parsedZeroVel = UmpParser::parseMessage({noteOnZeroVelWord});
        assertTest("UmpParser Recognizes Converted Zero Velocity as MT 0x2", parsedZeroVel.messageType == 2);
    }

    // =========================================================================
    // 3. MidiInputController - Teste de Estresse de Hot-Swap
    // =========================================================================
    {
        MidiInputController controller;

        assertTest("HotSwap Stress Initial Backend is FakeUmp", controller.getActiveBackendType() == UmpBackendType::FakeUmp);
        assertTest("HotSwap Stress Initial Port Closed", !controller.isPortOpen());

        // Teste de estresse: 50 alternâncias contínuas e rápidas entre FakeUmp e Libremidi
        bool allSwitchesSucceeded = true;
        const int iterations = 50;

        for (int i = 0; i < iterations; ++i) {
            // Alterna para Libremidi
            controller.switchBackend(UmpBackendType::Libremidi);
            if (controller.getActiveBackendType() != UmpBackendType::Libremidi) {
                allSwitchesSucceeded = false;
                break;
            }
            if (controller.getActiveBackendName() != "libremidi (Cross-Platform UMP)") {
                allSwitchesSucceeded = false;
                break;
            }
            // Consultas de portas e polling
            auto portsL = controller.getAvailablePorts();
            auto eventsL = controller.pollNewEvents();

            // Alterna de volta para FakeUmp
            controller.switchBackend(UmpBackendType::FakeUmp);
            if (controller.getActiveBackendType() != UmpBackendType::FakeUmp) {
                allSwitchesSucceeded = false;
                break;
            }
            if (controller.getActiveBackendName() != "Fake UMP Backend") {
                allSwitchesSucceeded = false;
                break;
            }
            auto portsF = controller.getAvailablePorts();
            auto eventsF = controller.pollNewEvents();
        }
        assertTest("HotSwap Loop 50 Iterations Completed Without Deadlock", allSwitchesSucceeded);

        // Estado final consistente
        assertTest("HotSwap Final Backend Type is FakeUmp", controller.getActiveBackendType() == UmpBackendType::FakeUmp);

        // Troca redundante para o mesmo backend (sem reinicializações desnecessárias)
        controller.switchBackend(UmpBackendType::FakeUmp);
        assertTest("HotSwap Redundant Switch Stays FakeUmp", controller.getActiveBackendType() == UmpBackendType::FakeUmp);

        // Teste com porta aberta: abrir porta no FakeUmp e alternar para Libremidi
        bool openedFake = controller.openPort(0);
        assertTest("HotSwap Open Port on FakeUmp", openedFake);
        assertTest("HotSwap FakeUmp Port Is Open", controller.isPortOpen());

        controller.switchBackend(UmpBackendType::Libremidi);
        assertTest("HotSwap Switching Closes Previous Port Cleanly", !controller.isPortOpen());
        assertTest("HotSwap New Backend Active After Open Port Switch", controller.getActiveBackendType() == UmpBackendType::Libremidi);

        // Retorno limpo e reabertura de porta no FakeUmp
        controller.switchBackend(UmpBackendType::FakeUmp);
        assertTest("HotSwap Restored to FakeUmp", controller.getActiveBackendType() == UmpBackendType::FakeUmp);
        assertTest("HotSwap Reopen Port on FakeUmp", controller.openPort(0));
        assertTest("HotSwap FakeUmp Port Is Reopened", controller.isPortOpen());
        controller.closePort();
        assertTest("HotSwap Controller Closed Cleanly", !controller.isPortOpen());
    }

    // =========================================================================
    // 4. RtMidiInputBackend - Dependência Obrigatória e Robustez
    // =========================================================================
    {
        RtMidiInputBackend rtBackend;

        // 4.1 Identificação e estado inicial
        assertTest("RtMidiBackend Name", rtBackend.backendName() == "RtMidi (Real Hardware)");
        assertTest("RtMidiBackend Initially Closed", !rtBackend.isOpen());
        assertTest("RtMidiBackend Empty Events When Closed", rtBackend.pollEvents().empty());

        // 4.2 Listagem de portas segura
        bool rtListingSucceeded = true;
        QStringList rtPorts;
        try {
            rtPorts = rtBackend.listInputPorts();
        } catch (...) {
            rtListingSucceeded = false;
        }
        assertTest("RtMidiBackend listInputPorts Exception Safety", rtListingSucceeded);

        // 4.3 Tentativa de abertura com índices inválidos
        assertTest("RtMidiBackend Open Negative Port", !rtBackend.openInputPort(-1));
        assertTest("RtMidiBackend Open Out-of-Bounds Port", !rtBackend.openInputPort(99999));
        assertTest("RtMidiBackend Remains Closed After Invalid Opens", !rtBackend.isOpen());

        // 4.4 Fechamento seguro e idempotente
        rtBackend.closeInputPort();
        assertTest("RtMidiBackend Close When Closed Safe", !rtBackend.isOpen());
        assertTest("RtMidiBackend Queue Empty After Close", rtBackend.pollEvents().empty());
    }
}
