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
#include <QClipboard>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi();
}

void MainWindow::setupUi() {
    setWindowTitle("MIDI 2.0 UMP Analyzer (Offline v0.6.0)");
    resize(900, 600);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // Área de Ações
    QHBoxLayout* actionsLayout = new QHBoxLayout();
    m_openFileBtn = new QPushButton("Abrir arquivo", this);
    m_saveLogBtn = new QPushButton("Salvar log", this);
    m_copyTableBtn = new QPushButton("Copiar Tabela", this);
    m_clearBtn = new QPushButton("Limpar", this);
    m_loadExamplesBtn = new QPushButton("Carregar exemplos", this);
    
    actionsLayout->addWidget(m_openFileBtn);
    actionsLayout->addWidget(m_saveLogBtn);
    actionsLayout->addWidget(m_copyTableBtn);
    actionsLayout->addWidget(m_clearBtn);
    actionsLayout->addWidget(m_loadExamplesBtn);
    actionsLayout->addStretch();
    mainLayout->addLayout(actionsLayout);

    // Área de Input
    QHBoxLayout* inputLayout = new QHBoxLayout();
    QLabel* inputLabel = new QLabel("Hex UMP (ex: 20904000 4090400040000000):", this);
    m_inputField = new QPlainTextEdit(this);
    m_inputField->setPlaceholderText("Cole pacotes UMPs em Hexadecimal (ex: SysEx7, SysEx8, Flex, Voice)...");
    m_inputField->setFixedHeight(80);
    m_interpretBtn = new QPushButton("Interpretar", this);
    inputLayout->addWidget(inputLabel);
    inputLayout->addWidget(m_inputField);
    inputLayout->addWidget(m_interpretBtn);
    mainLayout->addLayout(inputLayout);

    // Área de Filtro e Estatísticas
    QHBoxLayout* filterLayout = new QHBoxLayout();
    QLabel* filterLabel = new QLabel("Filtrar por Type:", this);
    m_filterField = new QLineEdit(this);
    m_filterField->setPlaceholderText("Digite para filtrar... (ex: SysEx, Flex, Voice)");
    
    m_statsLabel = new QLabel("Estatísticas: 0 lidos | 0 válidos | 0 erros", this);
    
    filterLayout->addWidget(filterLabel);
    filterLayout->addWidget(m_filterField);
    filterLayout->addStretch();
    filterLayout->addWidget(m_statsLabel);
    mainLayout->addLayout(filterLayout);

    // Área da Tabela
    m_tableWidget = new QTableWidget(this);
    m_tableWidget->setColumnCount(8);
    m_tableWidget->setHorizontalHeaderLabels({"#", "Words (Hex)", "Size", "Type", "Group", "Status", "Channel", "Description"});
    
    // Configura redimensionamento manual pelo usuário e larguras iniciais
    m_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_tableWidget->horizontalHeader()->setStretchLastSection(true); // Faz a descrição ocupar o resto
    
    m_tableWidget->setColumnWidth(0, 40);  // Index
    m_tableWidget->setColumnWidth(1, 180); // Words
    m_tableWidget->setColumnWidth(2, 60);  // Size
    m_tableWidget->setColumnWidth(3, 200); // Type
    m_tableWidget->setColumnWidth(4, 50);  // Group
    m_tableWidget->setColumnWidth(5, 50);  // Status
    m_tableWidget->setColumnWidth(6, 60);  // Channel
    m_tableWidget->setColumnWidth(7, 300); // Description (irá esticar)
    
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
    connect(m_copyTableBtn, &QPushButton::clicked, this, &MainWindow::copyTableClicked);
    connect(m_clearBtn, &QPushButton::clicked, this, &MainWindow::clearClicked);
    connect(m_loadExamplesBtn, &QPushButton::clicked, this, &MainWindow::loadExamplesClicked);
    connect(m_filterField, &QLineEdit::textChanged, this, &MainWindow::filterTable);
    
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

    m_tableWidget->setRowCount(0); // Limpa a tabela antes de cada nova interpretação

    logMessage(QString("Processando %1 bytes de texto...").arg(input.length()));

    // Remove espaços, quebras de linha e tabs para formar um buffer contínuo de palavras hexadecimais
    QString cleanInput = input.remove(" ").remove("\n").remove("\r").remove("\t");
    
    int totalLidos = 0;
    int totalErros = 0;
    int totalValidos = 0;

    if (cleanInput.length() % 8 != 0) {
        logMessage(QString("Erro de integridade UMP: O tamanho da entrada (%1 caracteres) não é múltiplo de 8.").arg(cleanInput.length()));
        totalErros++;
        m_statsLabel->setText(QString("Estatísticas: %1 lidos | %2 válidos | %3 erros").arg(totalLidos).arg(totalValidos).arg(totalErros));
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
            logMessage(QString("Erro Crítico: A string '%1' contém texto inválido.").arg(hexWordStr));
            totalErros++;
            m_statsLabel->setText(QString("Estatísticas: %1 lidos | %2 válidos | %3 erros").arg(totalLidos).arg(totalValidos).arg(totalErros));
            return;
        }
    }

    int wordIndex = 0;
    while (wordIndex < allWords.size()) {
        totalLidos++;
        uint32_t word0 = allWords[wordIndex];
        int mt = (word0 >> 28) & 0xF;
        int expectedWords = UmpParser::getWordCountForMessageType(mt);
        
        if (wordIndex + expectedWords > allWords.size()) {
            logMessage(QString("Erro de empacotamento UMP: Pacote Incompleto. MT 0x%1 esperava %2 palavras.")
                        .arg(mt, 1, 16).arg(expectedWords));
            totalErros++;
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
        QTableWidgetItem* itemIndex = new QTableWidgetItem(QString::number(rowCount + 1));
        itemIndex->setToolTip(itemIndex->text());
        m_tableWidget->setItem(rowCount, 0, itemIndex);
        
        // 1: Words
        QStringList wordsStrList;
        for (uint32_t w : parsed.words) {
            wordsStrList << QString("%1").arg(w, 8, 16, QChar('0')).toUpper();
        }
        QTableWidgetItem* itemWords = new QTableWidgetItem(wordsStrList.join(" "));
        itemWords->setToolTip(itemWords->text());
        m_tableWidget->setItem(rowCount, 1, itemWords);
        
        // 2: Size
        QTableWidgetItem* itemSize = new QTableWidgetItem(QString("%1 bits").arg(parsed.sizeBits));
        itemSize->setToolTip(itemSize->text());
        m_tableWidget->setItem(rowCount, 2, itemSize);
        
        // 3: Type
        QTableWidgetItem* itemType = new QTableWidgetItem(UmpParser::getMessageTypeString(parsed.messageType));
        itemType->setToolTip(itemType->text());
        m_tableWidget->setItem(rowCount, 3, itemType);
        
        // 4: Group
        QString groupStr = parsed.group >= 0 ? QString::number(parsed.group) : "-";
        QTableWidgetItem* itemGroup = new QTableWidgetItem(groupStr);
        itemGroup->setToolTip(itemGroup->text());
        m_tableWidget->setItem(rowCount, 4, itemGroup);
        
        // 5: Status
        QString statusStr = parsed.status >= 0 ? QString("0x%1").arg(parsed.status, 1, 16).toUpper() : "-";
        QTableWidgetItem* itemStatus = new QTableWidgetItem(statusStr);
        itemStatus->setToolTip(itemStatus->text());
        m_tableWidget->setItem(rowCount, 5, itemStatus);
        
        // 6: Channel
        QString channelStr = parsed.channel >= 0 ? QString::number(parsed.channel + 1) : "-"; 
        QTableWidgetItem* itemChannel = new QTableWidgetItem(channelStr);
        itemChannel->setToolTip(itemChannel->text());
        m_tableWidget->setItem(rowCount, 6, itemChannel);
        
        // 7: Description
        QTableWidgetItem* itemDesc = new QTableWidgetItem(parsed.description);
        itemDesc->setToolTip(itemDesc->text());
        m_tableWidget->setItem(rowCount, 7, itemDesc);
        
        logMessage(QString("Pacote UMP de %1 bits interpretado.").arg(parsed.sizeBits));
        totalValidos++;
        wordIndex += expectedWords;
    }
    
    m_statsLabel->setText(QString("Estatísticas: %1 lidos | %2 válidos | %3 erros").arg(totalLidos).arg(totalValidos).arg(totalErros));
    
    // Aplicar filtro ativo se houver
    filterTable(m_filterField->text());
    
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
    m_filterField->clear();
    m_statsLabel->setText("Estatísticas: 0 lidos | 0 válidos | 0 erros");
    logMessage("Interface limpa.");
}

void MainWindow::loadExamplesClicked() {
    m_inputField->setPlainText("20904000\n40904000 40000000\n30040000 00000000\n501D0000 00000000 00000000 00000000\nD0000101 41000000 00000000 00000000\nF0030000 41424300 00000000 00000000");
    logMessage("Exemplos completos carregados (Voice, SysEx7, SysEx8, Flex, Stream).");
}

void MainWindow::copyTableClicked() {
    QString clipboardText = "";
    for (int r = 0; r < m_tableWidget->rowCount(); ++r) {
        if (m_tableWidget->isRowHidden(r)) continue;
        for (int c = 0; c < m_tableWidget->columnCount(); ++c) {
            QTableWidgetItem* item = m_tableWidget->item(r, c);
            clipboardText += (item ? item->text() : "") + "\t";
        }
        clipboardText += "\n";
    }
    QApplication::clipboard()->setText(clipboardText);
    logMessage("Tabela visível copiada para a Área de Transferência.");
}

void MainWindow::filterTable(const QString& text) {
    for (int r = 0; r < m_tableWidget->rowCount(); ++r) {
        QTableWidgetItem* item = m_tableWidget->item(r, 3); // Coluna "Type"
        if (item) {
            bool match = item->text().contains(text, Qt::CaseInsensitive);
            m_tableWidget->setRowHidden(r, !match);
        }
    }
}
