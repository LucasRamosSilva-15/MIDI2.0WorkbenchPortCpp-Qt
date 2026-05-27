#include "MainWindow.h"
#include "UmpParser.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QHeaderView>
#include <QLabel>
#include <QStringList>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi();
}

void MainWindow::setupUi() {
    setWindowTitle("MIDI 2.0 UMP Analyzer (Offline MVP)");
    resize(900, 600);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // Área de Ações
    QHBoxLayout* actionsLayout = new QHBoxLayout();
    m_openFileBtn = new QPushButton("Abrir arquivo", this);
    m_saveLogBtn = new QPushButton("Salvar log", this);
    m_clearBtn = new QPushButton("Limpar", this);
    m_loadExamplesBtn = new QPushButton("Carregar exemplos", this);
    
    actionsLayout->addWidget(m_openFileBtn);
    actionsLayout->addWidget(m_saveLogBtn);
    actionsLayout->addWidget(m_clearBtn);
    actionsLayout->addWidget(m_loadExamplesBtn);
    actionsLayout->addStretch();
    mainLayout->addLayout(actionsLayout);

    // Área de Input
    QHBoxLayout* inputLayout = new QHBoxLayout();
    QLabel* inputLabel = new QLabel("Hex UMP (ex: 20904000 4090400040000000):", this);
    m_inputField = new QPlainTextEdit(this);
    m_inputField->setPlaceholderText("Cole pacotes UMPs em Hexadecimal (suporta múltiplas linhas)...");
    m_inputField->setFixedHeight(80);
    m_interpretBtn = new QPushButton("Interpretar", this);
    inputLayout->addWidget(inputLabel);
    inputLayout->addWidget(m_inputField);
    inputLayout->addWidget(m_interpretBtn);
    mainLayout->addLayout(inputLayout);

    // Área da Tabela
    m_tableWidget = new QTableWidget(this);
    m_tableWidget->setColumnCount(8);
    m_tableWidget->setHorizontalHeaderLabels({"#", "Words (Hex)", "Size", "Type", "Group", "Status", "Channel", "Description"});
    m_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mainLayout->addWidget(m_tableWidget, 2);

    // Área de Log
    QLabel* logLabel = new QLabel("Log:", this);
    m_logPanel = new QTextEdit(this);
    m_logPanel->setReadOnly(true);
    mainLayout->addWidget(logLabel);
    mainLayout->addWidget(m_logPanel, 1);

    connect(m_interpretBtn, &QPushButton::clicked, this, &MainWindow::interpretClicked);
    
    connect(m_openFileBtn, &QPushButton::clicked, this, &MainWindow::openFileClicked);
    connect(m_saveLogBtn, &QPushButton::clicked, this, &MainWindow::saveLogClicked);
    connect(m_clearBtn, &QPushButton::clicked, this, &MainWindow::clearClicked);
    connect(m_loadExamplesBtn, &QPushButton::clicked, this, &MainWindow::loadExamplesClicked);
    
    logMessage("Sistema inicializado. Aguardando pacotes UMP em formato hexadecimal.");
}

void MainWindow::logMessage(const QString& msg) {
    m_logPanel->append(msg);
}

void MainWindow::interpretClicked() {
    QString input = m_inputField->toPlainText().trimmed();
    if (input.isEmpty()) {
        logMessage("Erro: Entrada vazia.");
        return;
    }

    logMessage(QString("Processando %1 bytes de texto...").arg(input.length()));

    // Remove espaços, quebras de linha e tabs para formar um buffer contínuo de palavras hexadecimais
    QString cleanInput = input.remove(" ").remove("\n").remove("\r").remove("\t");
    
    if (cleanInput.length() % 8 != 0) {
        logMessage(QString("Erro de integridade UMP: O tamanho da entrada (%1 caracteres) não é múltiplo de 8. Faltam dados ou a estrutura foi truncada.").arg(cleanInput.length()));
        return;
    }

    std::vector<uint32_t> allWords;
    for (int i = 0; i < cleanInput.length(); i += 8) {
        QString hexWordStr = cleanInput.mid(i, 8);
        bool ok;
        uint32_t word = hexWordStr.toUInt(&ok, 16);
        if (ok) {
            allWords.push_back(word);
        } else {
            logMessage(QString("Erro Crítico: A string '%1' contém texto ou caracteres hexadecimais inválidos. Abortando.").arg(hexWordStr));
            return;
        }
    }

    int wordIndex = 0;
    while (wordIndex < allWords.size()) {
        uint32_t word0 = allWords[wordIndex];
        int mt = (word0 >> 28) & 0xF;
        int expectedWords = UmpParser::getWordCountForMessageType(mt);
        
        if (wordIndex + expectedWords > allWords.size()) {
            logMessage(QString("Erro de empacotamento UMP: O Message Type 0x%1 determina um pacote de %2 bits (%3 palavras), mas só %4 palavra(s) foram recebidas no buffer.")
                        .arg(mt, 1, 16).arg(expectedWords * 32).arg(expectedWords).arg(allWords.size() - wordIndex));
            break;
        }
        
        std::vector<uint32_t> msgWords;
        for (int i = 0; i < expectedWords; ++i) {
            msgWords.push_back(allWords[wordIndex + i]);
        }
        
        ParsedUmp parsed = UmpParser::parseMessage(msgWords);
        
        int rowCount = m_tableWidget->rowCount();
        m_tableWidget->insertRow(rowCount);
        
        // 0: Index
        m_tableWidget->setItem(rowCount, 0, new QTableWidgetItem(QString::number(rowCount + 1)));
        
        // 1: Words
        QStringList wordsStrList;
        for (uint32_t w : parsed.words) {
            wordsStrList << QString("%1").arg(w, 8, 16, QChar('0')).toUpper();
        }
        m_tableWidget->setItem(rowCount, 1, new QTableWidgetItem(wordsStrList.join(" ")));
        
        // 2: Size
        m_tableWidget->setItem(rowCount, 2, new QTableWidgetItem(QString("%1 bits").arg(parsed.sizeBits)));
        
        // 3: Type
        m_tableWidget->setItem(rowCount, 3, new QTableWidgetItem(UmpParser::getMessageTypeString(parsed.messageType)));
        
        // 4: Group
        QString groupStr = parsed.group >= 0 ? QString::number(parsed.group) : "-";
        m_tableWidget->setItem(rowCount, 4, new QTableWidgetItem(groupStr));
        
        // 5: Status
        QString statusStr = parsed.status >= 0 ? QString("0x%1").arg(parsed.status, 1, 16).toUpper() : "-";
        m_tableWidget->setItem(rowCount, 5, new QTableWidgetItem(statusStr));
        
        // 6: Channel
        QString channelStr = parsed.channel >= 0 ? QString::number(parsed.channel + 1) : "-"; 
        m_tableWidget->setItem(rowCount, 6, new QTableWidgetItem(channelStr));
        
        // 7: Description
        m_tableWidget->setItem(rowCount, 7, new QTableWidgetItem(parsed.description));
        
        logMessage(QString("Pacote UMP de %1 bits interpretado.").arg(parsed.sizeBits));
        
        wordIndex += expectedWords;
    }
    
    // m_inputField->clear(); // Removido para permitir que o usuário edite e re-rode se quiser
}

void MainWindow::openFileClicked() {
    QString fileName = QFileDialog::getOpenFileName(this, "Abrir Arquivo UMP", "", "Text Files (*.txt);;All Files (*)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString content = in.readAll().trimmed();
            m_inputField->setPlainText(content);
            logMessage(QString("Arquivo carregado: %1").arg(fileName));
        } else {
            logMessage("Erro ao abrir o arquivo selecionado.");
        }
    }
}

void MainWindow::saveLogClicked() {
    QString fileName = QFileDialog::getSaveFileName(this, "Salvar Log", "ump_log.txt", "Text Files (*.txt);;All Files (*)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "=== Log do Analisador UMP ===\n";
            out << "Data/Hora: " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "\n\n";
            out << "Entrada Original:\n" << m_inputField->toPlainText() << "\n\n";
            
            out << "=== Tabela de Resultados ===\n";
            out << "#\tWords\tSize\tType\tGroup\tStatus\tChannel\tDescription\n";
            for (int r = 0; r < m_tableWidget->rowCount(); ++r) {
                for (int c = 0; c < m_tableWidget->columnCount(); ++c) {
                    QTableWidgetItem* item = m_tableWidget->item(r, c);
                    out << (item ? item->text() : "") << "\t";
                }
                out << "\n";
            }
            
            out << "\n=== Log de Execucao ===\n" << m_logPanel->toPlainText() << "\n";
            
            logMessage(QString("Log salvo com sucesso em: %1").arg(fileName));
        } else {
            logMessage("Erro ao salvar o log.");
        }
    }
}

void MainWindow::clearClicked() {
    m_inputField->clear();
    m_tableWidget->setRowCount(0);
    m_logPanel->clear();
    logMessage("Interface limpa.");
}

void MainWindow::loadExamplesClicked() {
    m_inputField->setPlainText("20904000\n40904000 40000000\nB0000000 00000000 00000000\nF0000000 00000000 00000000 00000000");
    logMessage("Exemplos basicos carregados no campo de entrada.");
}
