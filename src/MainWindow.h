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
#include "midi/IMidiInputBackend.h"

struct LiveMidiLogEntry {
    QString timestamp;
    QString bytesHex;
    QString description;
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
    void pollLiveMidi();
    void pauseLiveMidiClicked();
    void clearLiveMidiLogClicked();
    void exportLiveTxtClicked();
    void exportLiveCsvClicked();

private:
    void setupUi();
    void logMessage(const QString& msg);
    void updateDiagnostics();
    void updateLiveMidiStatus();
    
    void updateLiveMidiStats(const struct Midi1DecodedMessage& decoded, bool displayed);
    void refreshLiveMidiStatsUi();
    void resetLiveMidiStats();
    
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
    
    QComboBox* m_liveMidiTypeFilterCombo;
    QComboBox* m_liveMidiChannelFilterCombo;
    class QCheckBox* m_treatNoteOnZeroAsOffCb;
    
    QLabel* m_liveMidiStatusLabel;
    QLabel* m_liveMidiCountersLabel;
    QLabel* m_liveMidiStatsLabel;
    
    class QCheckBox* m_umpPreviewCb;
    QLabel* m_umpPreviewLabel;
    
    QTextEdit* m_liveMidiLog;
    QTimer* m_liveMidiTimer;

    std::unique_ptr<IMidiInputBackend> m_midiBackend;

    bool m_isLiveMidiPaused = false;
    QVector<LiveMidiLogEntry> m_liveMidiEvents;
    
    LiveMidiStats m_liveMidiStats;
    qint64 m_liveMidiSessionStartTimeMs = 0;
};
