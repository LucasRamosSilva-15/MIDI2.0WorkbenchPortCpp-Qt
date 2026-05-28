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
#include "midi/IMidiInputBackend.h"

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

private:
    void setupUi();
    void logMessage(const QString& msg);
    void updateDiagnostics();
    
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
    QTextEdit* m_liveMidiLog;
    QTimer* m_liveMidiTimer;

    std::unique_ptr<IMidiInputBackend> m_midiBackend;
};
