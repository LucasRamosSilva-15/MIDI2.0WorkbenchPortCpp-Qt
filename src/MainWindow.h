#pragma once
#include <memory>
#include <QMainWindow>
#include <QTableWidget>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QDir>
#include <QLabel>
#include <QTimer>
#include <QVector>
#include <QMap>
#include "midi/IMidiInputBackend.h"
#include "midi/MidiInputController.h"

struct LiveMidiLogEntry {
    QString timestamp;
    QString bytesHex;
    QString description;
    QString umpPreview;
};

struct LiveMidiRecordedEvent {
    QString timestamp;
    QString bytesHex;
    QString description;
    QString messageType;
    int channel = -1;
    QString umpPreview;
};

struct LiveMidiStats {
    quint64 received = 0;
    quint64 displayed = 0;
    quint64 byTypeNoteOn = 0;
    quint64 byTypeNoteOff = 0;
    quint64 byTypeControlChange = 0;
    quint64 byTypeProgramChange = 0;
    quint64 byTypePitchBend = 0;
    quint64 byTypePolyAftertouch = 0;
    quint64 byTypeChannelAftertouch = 0;
    quint64 byTypeSystem = 0;
    quint64 byTypeUnknown = 0;
    quint64 byChannel[16] = {0};
    quint64 noChannel = 0;

    QString lastMessageType;
    int lastChannel = -1;
    int lastNote = -1;
    int lastVelocity = -1;
    int lastCc = -1;
    int lastCcValue = -1;
    int lastProgram = -1;
    int lastPitchBend = -1;
};

struct LiveMidiSessionSummary {
    int totalEvents = 0;
    int umpSupported = 0;
    int umpUnsupported = 0;
    QMap<QString, int> byType;
    int byChannel[16] = {0};
    int noChannel = 0;
    QString firstTimestamp;
    QString lastTimestamp;
    double approximateDurationSeconds = 0.0;

    bool hasFirstNoteOn = false;
    LiveMidiRecordedEvent firstNoteOn;
    bool hasFirstNoteOff = false;
    LiveMidiRecordedEvent firstNoteOff;
    bool hasFirstControlChange = false;
    LiveMidiRecordedEvent firstControlChange;
    bool hasFirstProgramChange = false;
    LiveMidiRecordedEvent firstProgramChange;
    bool hasFirstPitchBend = false;
    LiveMidiRecordedEvent firstPitchBend;
};

struct FakeUmpRecordedEvent {
    QString timestamp;
    QString backend;
    QString port;
    QString umpWords;
    QString bits;
    QString messageType;
    QString group;
    QString status;
    QString channel;
    QString size;
    QString description;
};

struct FakeUmpSessionSummary {
    int totalEvents = 0;
    QMap<QString, int> byMessageType;
    QMap<QString, int> byGroup;
    QMap<QString, int> byStatus;
    QMap<QString, int> byChannel;
    int channelMessages = 0;
    int nonChannelMessages = 0;
    QString firstTimestamp;
    QString lastTimestamp;
    bool hasFirstEvent = false;
    bool hasLastEvent = false;
    FakeUmpRecordedEvent firstEvent;
    FakeUmpRecordedEvent lastEvent;
    QMap<QString, FakeUmpRecordedEvent> firstExampleByMessageType;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

private slots:
    void interpretClicked();
    void openFileClicked();
    void saveLogClicked();
    void exportCsvClicked();
    void copyTableClicked();
    void clearClicked();
    void loadExamplesClicked();
    void filterTable(const QString& text);
    void refreshMidiPortsClicked();
    void openMidiPortClicked();
    void closeMidiPortClicked();
    void pauseLiveMidiClicked();
    void clearLiveMidiLogClicked();
    void exportLiveTxtClicked();
    void exportLiveCsvClicked();
    void startSessionRecording();
    void stopSessionRecording();
    void clearSessionRecording();
    void exportSessionTxt();
    void exportSessionCsv();
    void exportSessionSummaryClicked();

    void umpBackendChanged(int index);
    void refreshFakeUmpPortsClicked();
    void openFakeUmpPortClicked();
    void closeFakeUmpPortClicked();
    void startFakeUmpPollingClicked();
    void stopFakeUmpPollingClicked();
    void clearFakeUmpClicked();
    void exportExperimentalUmpTxtClicked();
    void exportExperimentalUmpCsvClicked();
    
    void startFakeUmpRecordingClicked();
    void stopFakeUmpRecordingClicked();
    void clearFakeUmpSessionClicked();
    void exportFakeUmpSessionTxtClicked();
    void exportFakeUmpSessionCsvClicked();
    void exportFakeUmpSessionSummaryClicked();

    void pollFakeUmpBackend();
    void logFakeUmpMessage(const QString& message);
    void updateFakeUmpStatus();
    void addFakeUmpEventRow(const struct UmpRawEvent& event);
    
private:
    void setupUi();
    void logMessage(const QString& msg);
    void updateDiagnostics();
    void updateLiveMidiStatus();
    void pollLiveMidi();
    void updateLiveMidiStats(const struct Midi1DecodedMessage& decoded, bool displayed);
    void refreshLiveMidiStatsUi();
    void resetLiveMidiStats();
    
    void addLiveUmpPreviewRow(const QString& timestamp, const QString& midiBytes, const struct Midi1ToUmpPreviewResult& result, const struct Midi1DecodedMessage& decoded);
    void clearLiveUmpPreviewTable();
    
    LiveMidiSessionSummary buildLiveMidiSessionSummary() const;
    QString formatLiveMidiSessionSummaryReport(const LiveMidiSessionSummary& summary) const;
    QString formatRecordedExample(const QString& title, bool hasEvent, const LiveMidiRecordedEvent& event) const;
    
    FakeUmpSessionSummary buildFakeUmpSessionSummary() const;
    QString formatFakeUmpSessionSummaryReport(const FakeUmpSessionSummary& summary) const;
    
    QString getUmpStatusLabel(uint8_t status) const;
    QString getUmpSizeLabel(uint8_t messageType) const;
    QString csvEscape(const QString& value) const;
    QString csvTextCellForSpreadsheet(const QString& value) const;

    QString m_currentFile;
    QString m_lastOperation;
    
    QPlainTextEdit* m_inputField;
    QPushButton* m_interpretBtn;
    QPushButton* m_openFileBtn;
    QPushButton* m_saveLogBtn;
    QPushButton* m_exportCsvBtn;
    QPushButton* m_copyTableBtn;
    QPushButton* m_clearBtn;
    QPushButton* m_adjustColsBtn;
    QPushButton* m_loadExamplesBtn;
    
    QLineEdit* m_filterField;
    QLabel* m_statsLabel;
    QLabel* m_diagnosticsLabel;
    
    QTableWidget* m_tableWidget;
    QTextEdit* m_logPanel;
    QComboBox* m_samplesCombo;
    QString m_samplesPath;

    QPushButton* m_refreshMidiPortsBtn;
    QComboBox* m_liveMidiPortsCombo;
    QPushButton* m_openMidiPortBtn;
    QPushButton* m_closeMidiPortBtn;
    QPushButton* m_pauseLiveMidiBtn;
    QPushButton* m_clearLiveMidiLogBtn;
    QPushButton* m_exportLiveTxtBtn;
    QPushButton* m_exportLiveCsvBtn;
    
    QPushButton* m_startSessionBtn;
    QPushButton* m_stopSessionBtn;
    QPushButton* m_clearSessionBtn;
    QPushButton* m_exportSessionTxtBtn;
    QPushButton* m_exportSessionCsvBtn;
    QPushButton* m_exportSessionSummaryBtn;
    QLabel* m_sessionStatusLabel;
    QLabel* m_sessionCountLabel;
    
    QComboBox* m_liveMidiTypeFilterCombo;
    QComboBox* m_liveMidiChannelFilterCombo;
    class QCheckBox* m_treatNoteOnZeroAsOffCb;
    
    QLabel* m_liveMidiStatusLabel;
    QLabel* m_liveMidiCountersLabel;
    QLabel* m_liveMidiStatsLabel;
    
    class QCheckBox* m_umpPreviewCb;
    QLabel* m_umpPreviewLabel;
    
    QTableWidget* m_liveUmpPreviewTable;
    QTextEdit* m_liveMidiLog;
    QTimer* m_liveMidiTimer;

    std::unique_ptr<IMidiInputBackend> m_midiBackend;

    bool m_isLiveMidiPaused = false;
    QVector<LiveMidiLogEntry> m_liveMidiEvents;
    
    LiveMidiStats m_liveMidiStats;
    qint64 m_liveMidiSessionStartTimeMs = 0;
    
    QVector<LiveMidiRecordedEvent> m_liveMidiRecording;
    bool m_isLiveMidiRecording = false;
    quint64 m_liveMidiRecordedCount = 0;
    
    std::unique_ptr<MidiInputController> m_umpController;
    QComboBox* m_umpBackendTypeCombo = nullptr;
    QTimer* m_fakeUmpTimer = nullptr;
    QComboBox* m_fakeUmpPortCombo = nullptr;
    QLabel* m_fakeUmpStatusLabel = nullptr;
    QLabel* m_fakeUmpCounterLabel = nullptr;
    QTableWidget* m_fakeUmpTable = nullptr;
    QTextEdit* m_fakeUmpLog = nullptr;
    quint64 m_fakeUmpReceivedCount = 0;
    
    QVector<FakeUmpRecordedEvent> m_fakeUmpRecording;
    bool m_isFakeUmpRecording = false;
    quint64 m_fakeUmpRecordedCount = 0;
    QLabel* m_fakeUmpRecordingStatusLabel = nullptr;
    QLabel* m_fakeUmpRecordingCounterLabel = nullptr;
};
