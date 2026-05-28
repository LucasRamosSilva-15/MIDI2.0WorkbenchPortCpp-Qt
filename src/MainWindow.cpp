#include "MainWindow.h"
#include "UmpParser.h"
#include <QApplication>
#include <QClipboard>
#include <QDateTime>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QStringList>
#include <QTextStream>
#include <QVBoxLayout>
#include <QWidget>
#include <QFileInfo>
#include <QCoreApplication>
#include <QDir>
#include <QComboBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) { setupUi(); }

void MainWindow::setupUi() {
  setWindowTitle("MIDI 2.0 UMP Analyzer (Offline v0.6.0)");
  resize(900, 600);

  QWidget *centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);

  QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

  // Área de Ações
  QHBoxLayout *actionsLayout = new QHBoxLayout();
  m_openFileBtn = new QPushButton("Abrir arquivo", this);
  m_saveLogBtn = new QPushButton("Salvar log", this);
  m_copyTableBtn = new QPushButton("Copiar Tabela", this);
  m_clearBtn = new QPushButton("Limpar", this);
  m_loadExamplesBtn = new QPushButton("Carregar exemplo", this);
  m_samplesCombo = new QComboBox(this);

  // Localizar pasta samples
  QString appDir = QCoreApplication::applicationDirPath();
  QStringList searchPaths = {
      appDir + "/samples",
      appDir + "/../samples",
      appDir + "/../../samples"
  };
  
  m_samplesPath = "";
  for (const QString& path : searchPaths) {
      if (QDir(path).exists()) {
          m_samplesPath = path;
          break;
      }
  }

  if (!m_samplesPath.isEmpty()) {
      QDir dir(m_samplesPath);
      QStringList filters;
      filters << "*.txt";
      QFileInfoList files = dir.entryInfoList(filters, QDir::Files);
      for (const QFileInfo& file : files) {
          m_samplesCombo->addItem(file.fileName());
      }
  } else {
      m_samplesCombo->addItem("(samples não encontrados)");
      m_samplesCombo->setEnabled(false);
      m_loadExamplesBtn->setEnabled(false);
  }

  actionsLayout->addWidget(m_openFileBtn);
  actionsLayout->addWidget(m_saveLogBtn);
  actionsLayout->addWidget(m_copyTableBtn);
  actionsLayout->addWidget(m_clearBtn);
  actionsLayout->addWidget(m_samplesCombo);
  actionsLayout->addWidget(m_loadExamplesBtn);
  actionsLayout->addStretch();
  mainLayout->addLayout(actionsLayout);

  // Área de Input
  QHBoxLayout *inputLayout = new QHBoxLayout();
  QLabel *inputLabel =
      new QLabel("Hex UMP (ex: 20904000 4090400040000000):", this);
  m_inputField = new QPlainTextEdit(this);
  m_inputField->setPlaceholderText(
      "Cole pacotes UMPs em Hexadecimal (ex: SysEx7, SysEx8, Flex, Voice)...");
  m_inputField->setFixedHeight(80);
  m_interpretBtn = new QPushButton("Interpretar", this);
  inputLayout->addWidget(inputLabel);
  inputLayout->addWidget(m_inputField);
  inputLayout->addWidget(m_interpretBtn);
  mainLayout->addLayout(inputLayout);

  // Área de Filtro e Estatísticas
  QHBoxLayout *filterLayout = new QHBoxLayout();
  QLabel *filterLabel = new QLabel("Filtrar por Type:", this);
  m_filterField = new QLineEdit(this);
  m_filterField->setPlaceholderText(
      "Digite para filtrar... (ex: SysEx, Flex, Voice)");

  m_statsLabel =
      new QLabel("Estatísticas: 0 lidos | 0 válidos | 0 erros", this);

  filterLayout->addWidget(filterLabel);
  filterLayout->addWidget(m_filterField);
  filterLayout->addStretch();
  filterLayout->addWidget(m_statsLabel);
  mainLayout->addLayout(filterLayout);

  // Área da Tabela
  m_tableWidget = new QTableWidget(this);
  m_tableWidget->setColumnCount(8);
  m_tableWidget->setHorizontalHeaderLabels({"#", "Words (Hex)", "Size", "Type",
                                            "Group", "Status", "Channel",
                                            "Description"});

  // Configura redimensionamento manual pelo usuário e larguras iniciais
  m_tableWidget->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Interactive);
  m_tableWidget->horizontalHeader()->setStretchLastSection(
      true); // Faz a descrição ocupar o resto

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
  QLabel *logLabel = new QLabel("Log:", this);
  m_logPanel = new QTextEdit(this);
  m_logPanel->setReadOnly(true);
  mainLayout->addWidget(logLabel);
  mainLayout->addWidget(m_logPanel, 1);

  connect(m_interpretBtn, &QPushButton::clicked, this,
          &MainWindow::interpretClicked);

  connect(m_openFileBtn, &QPushButton::clicked, this,
          &MainWindow::openFileClicked);
  connect(m_saveLogBtn, &QPushButton::clicked, this,
          &MainWindow::saveLogClicked);
  connect(m_copyTableBtn, &QPushButton::clicked, this,
          &MainWindow::copyTableClicked);
  connect(m_clearBtn, &QPushButton::clicked, this, &MainWindow::clearClicked);
  connect(m_loadExamplesBtn, &QPushButton::clicked, this,
          &MainWindow::loadExamplesClicked);
  connect(m_filterField, &QLineEdit::textChanged, this,
          &MainWindow::filterTable);

  logMessage(
      "Sistema inicializado. Aguardando pacotes UMP em formato hexadecimal.");
}

void MainWindow::logMessage(const QString &msg) { m_logPanel->append(msg); }

void MainWindow::interpretClicked() {
  QString input = m_inputField->toPlainText().trimmed();
  if (input.isEmpty()) {
    logMessage("Erro: Entrada vazia.");
    return;
  }

  m_tableWidget->setRowCount(0);

  int totalLidos = 0;
  int totalErros = 0;
  int totalValidos = 0;

  ValidationResult result = UmpParser::validateAndExtractWords(input);
    
  if (!result.success) {
      logMessage(result.errorMessage);
      totalErros++;
      m_statsLabel->setText(QString("Estatísticas: %1 lidos | %2 válidos | %3 erros").arg(totalLidos).arg(totalValidos).arg(totalErros));
      return;
  }

  for (const auto& msgWords : result.extractedMessages) {
      totalLidos++;
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
  }

  m_statsLabel->setText(
      QString("Estatísticas: %1 lidos | %2 válidos | %3 erros")
          .arg(totalLidos)
          .arg(totalValidos)
          .arg(totalErros));

  // Aplicar filtro ativo se houver
  filterTable(m_filterField->text());

  // m_inputField->clear(); // Removido para permitir que o usuário edite e
  // re-rode se quiser
}

void MainWindow::openFileClicked() {
  QString fileName = QFileDialog::getOpenFileName(
      this, "Abrir Arquivo UMP", "", "Text Files (*.txt);;All Files (*)");
  if (!fileName.isEmpty()) {
    QFile file(fileName);
    if (file.size() > 1024 * 1024) { // 1 MB limit
      logMessage(
          "Erro ao abrir arquivo: O arquivo excede o limite de tamanho (1 MB). "
          "Por segurança, arquivos massivos estão bloqueados no MVP.");
      return;
    }
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      QTextStream in(&file);
      QString content = in.readAll().trimmed();
      if (content.isEmpty()) {
        logMessage("Aviso: O arquivo selecionado está vazio.");
      } else {
        m_inputField->setPlainText(content);
        logMessage(QString("Arquivo carregado: %1 (%2 bytes)")
                       .arg(fileName)
                       .arg(content.length()));
      }
    } else {
      logMessage("Erro fatal ao tentar ler o arquivo selecionado no disco.");
    }
  }
}

void MainWindow::saveLogClicked() {
  QString fileName = QFileDialog::getSaveFileName(
      this, "Salvar Log", "ump_log.txt", "Text Files (*.txt);;All Files (*)");
  if (!fileName.isEmpty()) {
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      QTextStream out(&file);
      out << "=== Log do Analisador UMP ===\n";
      out << "Data/Hora: "
          << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")
          << "\n\n";
      out << "Entrada Original:\n" << m_inputField->toPlainText() << "\n\n";

      out << "=== Tabela de Resultados ===\n";
      out << "#\tWords\tSize\tType\tGroup\tStatus\tChannel\tDescription\n";
      for (int r = 0; r < m_tableWidget->rowCount(); ++r) {
        for (int c = 0; c < m_tableWidget->columnCount(); ++c) {
          QTableWidgetItem *item = m_tableWidget->item(r, c);
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
  if (m_samplesPath.isEmpty() || m_samplesCombo->currentText().isEmpty()) {
      logMessage("Erro: Nenhum exemplo selecionado ou pasta samples não encontrada.");
      return;
  }

  QString fileName = m_samplesCombo->currentText();
  QString fullPath = QDir(m_samplesPath).filePath(fileName);
  QFile file(fullPath);
  
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      QTextStream in(&file);
      QString content = in.readAll().trimmed();
      if (content.isEmpty()) {
          logMessage(QString("Aviso: O exemplo %1 está vazio.").arg(fileName));
      } else {
          m_inputField->setPlainText(content);
          logMessage(QString("Exemplo carregado: %1").arg(fileName));
      }
  } else {
      logMessage(QString("Erro ao tentar ler o arquivo de exemplo: %1").arg(fileName));
  }
}

void MainWindow::copyTableClicked() {
  QString clipboardText = "";
  for (int r = 0; r < m_tableWidget->rowCount(); ++r) {
    if (m_tableWidget->isRowHidden(r))
      continue;
    for (int c = 0; c < m_tableWidget->columnCount(); ++c) {
      QTableWidgetItem *item = m_tableWidget->item(r, c);
      clipboardText += (item ? item->text() : "") + "\t";
    }
    clipboardText += "\n";
  }
  QApplication::clipboard()->setText(clipboardText);
  logMessage("Tabela visível copiada para a Área de Transferência.");
}

void MainWindow::filterTable(const QString &text) {
  for (int r = 0; r < m_tableWidget->rowCount(); ++r) {
    QTableWidgetItem *item = m_tableWidget->item(r, 3); // Coluna "Type"
    if (item) {
      bool match = item->text().contains(text, Qt::CaseInsensitive);
      m_tableWidget->setRowHidden(r, !match);
    }
  }
}
