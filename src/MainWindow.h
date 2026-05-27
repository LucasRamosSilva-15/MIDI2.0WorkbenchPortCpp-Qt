#pragma once
#include <QMainWindow>
#include <QTableWidget>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QPushButton>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

private slots:
    void interpretClicked();
    void openFileClicked();
    void saveLogClicked();
    void clearClicked();
    void loadExamplesClicked();

private:
    void setupUi();
    void logMessage(const QString& msg);
    
    QPlainTextEdit* m_inputField;
    QPushButton* m_interpretBtn;
    QPushButton* m_openFileBtn;
    QPushButton* m_saveLogBtn;
    QPushButton* m_clearBtn;
    QPushButton* m_loadExamplesBtn;
    
    QTableWidget* m_tableWidget;
    QTextEdit* m_logPanel;
};
