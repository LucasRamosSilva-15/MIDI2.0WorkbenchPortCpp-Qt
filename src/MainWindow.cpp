#include "MainWindow.h"
#include "UmpParser.h"
#include "midi/Midi1LiveDecoder.h"
#include "midi/Midi1ToUmpPreviewConverter.h"
#include "midi/RtMidiInputBackend.h"
#include "midi/FakeUmpInputBackend.h"
#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QComboBox>
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QSplitter>
#include <QStringList>
#include <QTabWidget>
#include <QTextEdit>
#include <QTextStream>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_samplesPath("") {
  m_currentFile = "Nenhum";
  m_lastOperation = "Aguardando entrada...";
  m_midiBackend = std::make_unique<RtMidiInputBackend>();

  m_liveMidiTimer = new QTimer(this);
  connect(m_liveMidiTimer, &QTimer::timeout, this, &MainWindow::pollLiveMidi);

  setupUi();

  // Localizar pasta samples
  QString appDir = QCoreApplication::applicationDirPath();
  QStringList searchPaths = {appDir + "/samples", appDir + "/../samples",
                             appDir + "/../../samples"};

  for (const QString &path : searchPaths) {
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
    for (const QFileInfo &file : files) {
      m_samplesCombo->addItem(file.fileName());
    }
  } else {
    m_samplesCombo->addItem("(samples não encontrados)");
    m_samplesCombo->setEnabled(false);
    m_loadExamplesBtn->setEnabled(false);
  }

  updateDiagnostics();
}

void MainWindow::setupUi() {
  setWindowTitle("MIDI 2.0 UMP Analyzer (v4.16.0)");
  setMinimumSize(1100, 700);
  resize(1600, 900);

  QWidget *centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);
  QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

  QTabWidget *tabWidget = new QTabWidget(this);
  mainLayout->addWidget(tabWidget);

  // --- Tab 1: Offline UMP Analyzer ---
  QWidget *tabOffline = new QWidget();
  QVBoxLayout *offlineLayout = new QVBoxLayout(tabOffline);

  // Área de Ações
  QHBoxLayout *actionsLayout = new QHBoxLayout();
  m_openFileBtn = new QPushButton("Abrir arquivo", this);
  m_saveLogBtn = new QPushButton("Exportar TXT", this);
  m_exportCsvBtn = new QPushButton("Exportar CSV", this);
  m_copyTableBtn = new QPushButton("Copiar Tabela", this);
  m_clearBtn = new QPushButton("Limpar", this);
  m_adjustColsBtn = new QPushButton("Ajustar Colunas", this);
  m_loadExamplesBtn = new QPushButton("Carregar exemplo", this);
  m_samplesCombo = new QComboBox(this);

  actionsLayout->addWidget(m_openFileBtn);
  actionsLayout->addWidget(m_saveLogBtn);
  actionsLayout->addWidget(m_exportCsvBtn);
  actionsLayout->addWidget(m_copyTableBtn);
  actionsLayout->addWidget(m_adjustColsBtn);
  actionsLayout->addWidget(m_clearBtn);
  actionsLayout->addWidget(m_samplesCombo);
  actionsLayout->addWidget(m_loadExamplesBtn);
  actionsLayout->addStretch();
  offlineLayout->addLayout(actionsLayout);

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
  offlineLayout->addLayout(inputLayout);

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
  offlineLayout->addLayout(filterLayout);

  // Área da Tabela
  m_tableWidget = new QTableWidget(this);
  m_tableWidget->setColumnCount(8);
  m_tableWidget->setHorizontalHeaderLabels({"#", "Words (Hex)", "Size", "Type",
                                            "Group", "Status", "Channel",
                                            "Description"});
  m_tableWidget->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Interactive);
  m_tableWidget->horizontalHeader()->setStretchLastSection(true);
  m_tableWidget->setColumnWidth(0, 40);
  m_tableWidget->setColumnWidth(1, 260);
  m_tableWidget->setColumnWidth(2, 60);
  m_tableWidget->setColumnWidth(3, 240);
  m_tableWidget->setColumnWidth(4, 50);
  m_tableWidget->setColumnWidth(5, 50);
  m_tableWidget->setColumnWidth(6, 60);
  m_tableWidget->setColumnWidth(7, 400);
  offlineLayout->addWidget(m_tableWidget, 1);

  tabWidget->addTab(tabOffline, "Offline UMP Analyzer");

  // --- Tab 2: Live MIDI Monitor ---
  QWidget *tabLive = new QWidget();
  QVBoxLayout *liveLayout = new QVBoxLayout(tabLive);

  QGroupBox *liveMidiGroup = new QGroupBox("Live MIDI (Experimental)", this);
  QVBoxLayout *liveMidiMainLayout = new QVBoxLayout(liveMidiGroup);
  QHBoxLayout *liveMidiBtnLayout = new QHBoxLayout();

  m_refreshMidiPortsBtn = new QPushButton("Atualizar portas", this);
  m_liveMidiPortsCombo = new QComboBox(this);
  m_liveMidiPortsCombo->setMinimumWidth(250);
  m_openMidiPortBtn = new QPushButton("Abrir porta", this);
  m_closeMidiPortBtn = new QPushButton("Fechar porta", this);
  m_closeMidiPortBtn->setEnabled(false);

  liveMidiBtnLayout->addWidget(m_refreshMidiPortsBtn);
  liveMidiBtnLayout->addWidget(m_liveMidiPortsCombo);
  liveMidiBtnLayout->addWidget(m_openMidiPortBtn);
  liveMidiBtnLayout->addWidget(m_closeMidiPortBtn);
  liveMidiBtnLayout->addStretch();

  QHBoxLayout *liveMidiControlsLayout = new QHBoxLayout();
  m_pauseLiveMidiBtn = new QPushButton("Pausar Monitor", this);
  m_pauseLiveMidiBtn->setEnabled(false);
  m_clearLiveMidiLogBtn = new QPushButton("Limpar Live Log", this);
  m_exportLiveTxtBtn = new QPushButton("Exportar Live TXT", this);
  m_exportLiveCsvBtn = new QPushButton("Exportar Live CSV", this);
  m_liveMidiStatusLabel = new QLabel("Status: Porta fechada", this);
  m_liveMidiCountersLabel = new QLabel("Recebidas: 0 | Exibidas: 0", this);

  liveMidiControlsLayout->addWidget(m_pauseLiveMidiBtn);
  liveMidiControlsLayout->addWidget(m_clearLiveMidiLogBtn);
  liveMidiControlsLayout->addWidget(m_exportLiveTxtBtn);
  liveMidiControlsLayout->addWidget(m_exportLiveCsvBtn);
  liveMidiControlsLayout->addStretch();
  liveMidiControlsLayout->addWidget(m_liveMidiStatusLabel);
  liveMidiControlsLayout->addWidget(m_liveMidiCountersLabel);

  QGroupBox *sessionGroup = new QGroupBox("Session Recording", this);
  QHBoxLayout *sessionLayout = new QHBoxLayout(sessionGroup);
  m_sessionStatusLabel = new QLabel("Status: Parada", this);
  m_sessionCountLabel = new QLabel("Eventos gravados: 0", this);
  m_startSessionBtn = new QPushButton("Iniciar Gravação", this);
  m_stopSessionBtn = new QPushButton("Parar Gravação", this);
  m_stopSessionBtn->setEnabled(false);
  m_clearSessionBtn = new QPushButton("Limpar Sessão", this);
  m_exportSessionTxtBtn = new QPushButton("Exportar Sessão TXT", this);
  m_exportSessionCsvBtn = new QPushButton("Exportar Sessão CSV", this);
  m_exportSessionSummaryBtn =
      new QPushButton("Exportar Resumo da Sessão", this);

  sessionLayout->addWidget(m_sessionStatusLabel);
  sessionLayout->addWidget(m_sessionCountLabel);
  sessionLayout->addSpacing(20);
  sessionLayout->addWidget(m_startSessionBtn);
  sessionLayout->addWidget(m_stopSessionBtn);
  sessionLayout->addWidget(m_clearSessionBtn);
  sessionLayout->addStretch();
  sessionLayout->addWidget(m_exportSessionTxtBtn);
  sessionLayout->addWidget(m_exportSessionCsvBtn);
  sessionLayout->addWidget(m_exportSessionSummaryBtn);

  m_liveMidiLog = new QTextEdit(this);
  m_liveMidiLog->setReadOnly(true);
  m_liveMidiLog->document()->setMaximumBlockCount(1000);
  m_liveMidiLog->setPlaceholderText(
      "Eventos brutos MIDI 1.0 (Hex) aparecerão aqui...");

  m_liveUmpPreviewTable = new QTableWidget(this);
  m_liveUmpPreviewTable->setColumnCount(8);
  m_liveUmpPreviewTable->setHorizontalHeaderLabels(
      {"Timestamp", "MIDI Bytes", "UMP Word", "MT", "Group", "Status",
       "Channel", "Description"});
  m_liveUmpPreviewTable->horizontalHeader()->setStretchLastSection(true);
  m_liveUmpPreviewTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_liveUmpPreviewTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_liveUmpPreviewTable->verticalHeader()->setVisible(false);

  QHBoxLayout *liveMidiFiltersLayout = new QHBoxLayout();
  QLabel *typeLabel = new QLabel("Tipo:", this);
  m_liveMidiTypeFilterCombo = new QComboBox(this);
  m_liveMidiTypeFilterCombo->addItems(
      {"Todos", "Note On", "Note Off", "Control Change", "Program Change",
       "Pitch Bend", "Poly Aftertouch", "Channel Aftertouch",
       "System/Common/Real-Time"});

  QLabel *channelLabel = new QLabel("Canal:", this);
  m_liveMidiChannelFilterCombo = new QComboBox(this);
  m_liveMidiChannelFilterCombo->addItem("Todos os canais");
  for (int i = 1; i <= 16; ++i) {
    m_liveMidiChannelFilterCombo->addItem(QString("Ch %1").arg(i));
  }

  m_treatNoteOnZeroAsOffCb =
      new QCheckBox("Mostrar Note On vel 0 como Note Off", this);
  m_treatNoteOnZeroAsOffCb->setChecked(false);

  liveMidiFiltersLayout->addWidget(typeLabel);
  liveMidiFiltersLayout->addWidget(m_liveMidiTypeFilterCombo);
  liveMidiFiltersLayout->addSpacing(15);
  liveMidiFiltersLayout->addWidget(channelLabel);
  liveMidiFiltersLayout->addWidget(m_liveMidiChannelFilterCombo);
  liveMidiFiltersLayout->addSpacing(15);
  liveMidiFiltersLayout->addWidget(m_treatNoteOnZeroAsOffCb);
  liveMidiFiltersLayout->addStretch();

  QLabel *filterHintLabel = new QLabel(
      "<i>(Os filtros se aplicam a novas mensagens recebidas)</i>", this);
  filterHintLabel->setStyleSheet("color: #666;");
  liveMidiFiltersLayout->addWidget(filterHintLabel);

  liveMidiMainLayout->addLayout(liveMidiBtnLayout);
  liveMidiMainLayout->addLayout(liveMidiFiltersLayout);
  liveMidiMainLayout->addLayout(liveMidiControlsLayout);
  liveMidiMainLayout->addWidget(sessionGroup);

  QGroupBox *statsGroup = new QGroupBox("Live MIDI Statistics", this);
  QVBoxLayout *statsLayout = new QVBoxLayout(statsGroup);
  m_liveMidiStatsLabel = new QLabel("Aguardando porta abrir...", this);
  m_liveMidiStatsLabel->setWordWrap(true);
  m_liveMidiStatsLabel->setStyleSheet(
      "font-family: monospace; font-size: 12px;");
  statsLayout->addWidget(m_liveMidiStatsLabel);
  liveMidiMainLayout->addWidget(statsGroup);

  QGroupBox *umpPreviewGroup = new QGroupBox("UMP Preview", this);
  QHBoxLayout *umpPreviewLayout = new QHBoxLayout(umpPreviewGroup);
  m_umpPreviewCb = new QCheckBox("Mostrar UMP Preview", this);
  m_umpPreviewCb->setChecked(true);
  m_umpPreviewLabel = new QLabel("Último UMP Gerado: Nenhum", this);
  m_umpPreviewLabel->setStyleSheet("font-family: monospace;");
  umpPreviewLayout->addWidget(m_umpPreviewCb);
  umpPreviewLayout->addSpacing(20);
  umpPreviewLayout->addWidget(m_umpPreviewLabel, 1);
  liveMidiMainLayout->addWidget(umpPreviewGroup);
  QSplitter *liveSplitter = new QSplitter(Qt::Vertical, this);
  liveSplitter->addWidget(m_liveUmpPreviewTable);
  liveSplitter->addWidget(m_liveMidiLog);
  liveSplitter->setSizes({600, 400});
  liveMidiMainLayout->addWidget(liveSplitter, 1);

#ifndef USE_RTMIDI
  QLabel *rtMidiWarning = new QLabel(
      "<b>Aviso:</b> Suporte ao RtMidi não compilado (ENABLE_RTMIDI=OFF). "
      "Compile com RtMidi=ON para usar o Live MIDI.",
      this);
  rtMidiWarning->setStyleSheet(
      "color: #d32f2f; font-weight: bold; padding: 5px;");
  liveMidiMainLayout->addWidget(rtMidiWarning);
#endif

  liveLayout->addWidget(liveMidiGroup, 1);
  tabWidget->addTab(tabLive, "Live MIDI Monitor");

  // --- Tab Experimental UMP Backend ---
  QWidget *tabFakeUmp = new QWidget();
  QVBoxLayout *fakeUmpLayout = new QVBoxLayout(tabFakeUmp);

  QGroupBox *fakeBackendGroup = new QGroupBox("Fake UMP Backend", this);
  QHBoxLayout *fakeBackendLayout = new QHBoxLayout(fakeBackendGroup);
  
  QPushButton *refreshFakePortsBtn = new QPushButton("Atualizar portas UMP fake", this);
  m_fakeUmpPortCombo = new QComboBox(this);
  QPushButton *openFakePortBtn = new QPushButton("Abrir porta fake", this);
  QPushButton *closeFakePortBtn = new QPushButton("Fechar porta fake", this);
  m_fakeUmpStatusLabel = new QLabel("Fechada", this);
  m_fakeUmpStatusLabel->setStyleSheet("color: gray; font-weight: bold;");

  fakeBackendLayout->addWidget(refreshFakePortsBtn);
  fakeBackendLayout->addWidget(m_fakeUmpPortCombo, 1);
  fakeBackendLayout->addWidget(openFakePortBtn);
  fakeBackendLayout->addWidget(closeFakePortBtn);
  fakeBackendLayout->addWidget(new QLabel("Status:", this));
  fakeBackendLayout->addWidget(m_fakeUmpStatusLabel);

  QGroupBox *fakeStreamGroup = new QGroupBox("Experimental UMP Stream", this);
  QVBoxLayout *fakeStreamLayout = new QVBoxLayout(fakeStreamGroup);
  
  QHBoxLayout *fakeStreamControls = new QHBoxLayout();
  QPushButton *startFakePollingBtn = new QPushButton("Iniciar polling", this);
  QPushButton *stopFakePollingBtn = new QPushButton("Parar polling", this);
  QPushButton *clearFakeUmpBtn = new QPushButton("Limpar UMP experimental", this);
  QPushButton *exportFakeUmpTxtBtn = new QPushButton("Exportar TXT", this);
  QPushButton *exportFakeUmpCsvBtn = new QPushButton("Exportar CSV", this);
  m_fakeUmpCounterLabel = new QLabel("Eventos: 0", this);

  fakeStreamControls->addWidget(startFakePollingBtn);
  fakeStreamControls->addWidget(stopFakePollingBtn);
  fakeStreamControls->addWidget(clearFakeUmpBtn);
  fakeStreamControls->addWidget(exportFakeUmpTxtBtn);
  fakeStreamControls->addWidget(exportFakeUmpCsvBtn);
  fakeStreamControls->addWidget(m_fakeUmpCounterLabel);
  fakeStreamControls->addStretch();

  QLabel *fakeWarningLabel = new QLabel("<b>This is a fake backend prototype. It does not capture real MIDI 2.0/UMP hardware.</b>", this);
  fakeWarningLabel->setStyleSheet("color: #d32f2f; background-color: #ffcccc; padding: 5px; border-radius: 4px;");

  m_fakeUmpTable = new QTableWidget(0, 11, this);
  m_fakeUmpTable->setHorizontalHeaderLabels({"Timestamp", "Backend", "Port", "UMP Words", "Bits", "MT", "Group", "Status", "Channel", "Size", "Description"});
  m_fakeUmpTable->horizontalHeader()->setStretchLastSection(true);
  
  m_fakeUmpLog = new QTextEdit(this);
  m_fakeUmpLog->setReadOnly(true);
  m_fakeUmpLog->setMaximumHeight(100);

  fakeStreamLayout->addWidget(fakeWarningLabel);
  fakeStreamLayout->addLayout(fakeStreamControls);
  fakeStreamLayout->addWidget(m_fakeUmpTable, 1);
  fakeStreamLayout->addWidget(new QLabel("Log experimental:", this));
  fakeStreamLayout->addWidget(m_fakeUmpLog);

  QGroupBox *fakeRecordingGroup = new QGroupBox("Fake UMP Session Recording", this);
  QVBoxLayout *fakeRecordingLayout = new QVBoxLayout(fakeRecordingGroup);
  
  QHBoxLayout *fakeRecordingStatusLayout = new QHBoxLayout();
  m_fakeUmpRecordingStatusLabel = new QLabel("Status: Parada", this);
  m_fakeUmpRecordingCounterLabel = new QLabel("Eventos gravados: 0", this);
  fakeRecordingStatusLayout->addWidget(m_fakeUmpRecordingStatusLabel);
  fakeRecordingStatusLayout->addWidget(m_fakeUmpRecordingCounterLabel);
  fakeRecordingStatusLayout->addStretch();
  
  QHBoxLayout *fakeRecordingControlsLayout = new QHBoxLayout();
  QPushButton *startFakeRecordingBtn = new QPushButton("Iniciar Gravação UMP", this);
  QPushButton *stopFakeRecordingBtn = new QPushButton("Parar Gravação UMP", this);
  QPushButton *clearFakeSessionBtn = new QPushButton("Limpar Sessão UMP", this);
  QPushButton *exportFakeSessionTxtBtn = new QPushButton("Exportar Sessão TXT", this);
  QPushButton *exportFakeSessionCsvBtn = new QPushButton("Exportar Sessão CSV", this);
  QPushButton *exportFakeSessionSummaryBtn = new QPushButton("Exportar Resumo da Sessão UMP", this);
  
  fakeRecordingControlsLayout->addWidget(startFakeRecordingBtn);
  fakeRecordingControlsLayout->addWidget(stopFakeRecordingBtn);
  fakeRecordingControlsLayout->addWidget(clearFakeSessionBtn);
  fakeRecordingControlsLayout->addWidget(exportFakeSessionTxtBtn);
  fakeRecordingControlsLayout->addWidget(exportFakeSessionCsvBtn);
  fakeRecordingControlsLayout->addWidget(exportFakeSessionSummaryBtn);
  fakeRecordingControlsLayout->addStretch();
  
  fakeRecordingLayout->addLayout(fakeRecordingStatusLayout);
  fakeRecordingLayout->addLayout(fakeRecordingControlsLayout);

  fakeUmpLayout->addWidget(fakeBackendGroup);
  fakeUmpLayout->addWidget(fakeStreamGroup, 1);
  fakeUmpLayout->addWidget(fakeRecordingGroup);

  tabWidget->addTab(tabFakeUmp, "Experimental UMP Backend");

  // --- Tab 3: Logs / Diagnostics ---
  QWidget *tabLogs = new QWidget();
  QVBoxLayout *logsLayout = new QVBoxLayout(tabLogs);

  m_diagnosticsLabel = new QLabel(this);
  m_diagnosticsLabel->setStyleSheet(
      "QLabel { background-color: #f0f4f8; border: 1px solid #d9e2ec; padding: "
      "4px; border-radius: 4px; color: #102a43; font-weight: bold; }");
  logsLayout->addWidget(m_diagnosticsLabel);

  QLabel *logLabel = new QLabel("Log Geral da Aplicação:", this);
  m_logPanel = new QTextEdit(this);
  m_logPanel->setReadOnly(true);
  logsLayout->addWidget(logLabel);
  logsLayout->addWidget(m_logPanel, 1);

  tabWidget->addTab(tabLogs, "Logs / Diagnostics");

  // --- Tab 4: About / Help ---
  QWidget *tabAbout = new QWidget();
  QVBoxLayout *aboutLayout = new QVBoxLayout(tabAbout);
  QTextEdit *aboutText = new QTextEdit(this);
  aboutText->setReadOnly(true);
  aboutText->setHtml(
      "<h2>MIDI 2.0 Workbench Port</h2>"
      "<p><b>Versão:</b> v4.16.0</p>"
      "<p><b>Resumo:</b> Analisador estático forense para Universal MIDI "
      "Packets (UMP) "
      "e monitor experimental de portas de hardware MIDI 1.0 legado.</p>"
      "<p><i>Nota Experimental: Experimental UMP Backend uses FakeUmpInputBackend for controlled testing. It does not capture real operating-system UMP streams yet.</i></p>"

      "<h3>Offline UMP Analyzer</h3>"
      "<p>Módulo analítico principal. Disseca hexadecimais puros representando "
      "pacotes UMP de 32 a 128 bits "
      "(MT 0x2, 0x4, SysEx, UMP Stream), validando blocos e campos nativos do "
      "MIDI 2.0 em relatórios isolados.</p>"

      "<h3>Live MIDI Monitor</h3>"
      "<p>Módulo prático e experimental alimentado pelo backend <b>RtMidi</b>. "
      "O driver recebe "
      "<i>Bytes Crus MIDI 1.0</i> (ex: 90 3C 7F) diretamente do hardware "
      "físico.</p>"
      "<ul>"
      "<li><b>UMP Preview:</b> Traduz mensagens MIDI 1.0 Channel Voice para "
      "uma word UMP de 32-bits (<b>Message Type 0x2</b>).</li>"
      "<li><b>O que NÃO faz:</b> Não promove conversão pseudo-MIDI 2.0 (MT "
      "0x4), não atua como MIDI-CI ou Property Exchange, nem intercepta driver "
      "UMP real do S.O.</li>"
      "</ul>"

      "<h3>Gravações e Relatórios</h3>"
      "<p>A aba Live permite acionar a <b>Session Recording</b>, um cofre na "
      "memória RAM invisível aos filtros "
      "e limites de interface (1000 linhas). Ao exportar o <b>Session Summary "
      "Report</b>, um sumário matemático denso é gerado, o que é fundamental "
      "para depuração forense ou documentação em TCC.</p>");

  aboutLayout->addWidget(aboutText);
  tabWidget->addTab(tabAbout, "About / Help");

  connect(m_interpretBtn, &QPushButton::clicked, this,
          &MainWindow::interpretClicked);

  connect(m_openFileBtn, &QPushButton::clicked, this,
          &MainWindow::openFileClicked);
  connect(m_saveLogBtn, &QPushButton::clicked, this,
          &MainWindow::saveLogClicked);
  connect(m_exportCsvBtn, &QPushButton::clicked, this,
          &MainWindow::exportCsvClicked);
  connect(m_copyTableBtn, &QPushButton::clicked, this,
          &MainWindow::copyTableClicked);
  connect(m_adjustColsBtn, &QPushButton::clicked, m_tableWidget,
          &QTableWidget::resizeColumnsToContents);
  connect(m_clearBtn, &QPushButton::clicked, this, &MainWindow::clearClicked);
  connect(m_loadExamplesBtn, &QPushButton::clicked, this,
          &MainWindow::loadExamplesClicked);
  connect(m_filterField, &QLineEdit::textChanged, this,
          &MainWindow::filterTable);
  connect(m_refreshMidiPortsBtn, &QPushButton::clicked, this,
          &MainWindow::refreshMidiPortsClicked);
  connect(m_openMidiPortBtn, &QPushButton::clicked, this,
          &MainWindow::openMidiPortClicked);
  connect(m_closeMidiPortBtn, &QPushButton::clicked, this,
          &MainWindow::closeMidiPortClicked);
  connect(m_pauseLiveMidiBtn, &QPushButton::clicked, this,
          &MainWindow::pauseLiveMidiClicked);
  connect(m_clearLiveMidiLogBtn, &QPushButton::clicked, this,
          &MainWindow::clearLiveMidiLogClicked);
  connect(m_exportLiveTxtBtn, &QPushButton::clicked, this,
          &MainWindow::exportLiveTxtClicked);
  connect(m_exportLiveCsvBtn, &QPushButton::clicked, this,
          &MainWindow::exportLiveCsvClicked);

  connect(m_startSessionBtn, &QPushButton::clicked, this,
          &MainWindow::startSessionRecording);
  connect(m_stopSessionBtn, &QPushButton::clicked, this,
          &MainWindow::stopSessionRecording);
  connect(m_clearSessionBtn, &QPushButton::clicked, this,
          &MainWindow::clearSessionRecording);
  connect(m_exportSessionTxtBtn, &QPushButton::clicked, this,
          &MainWindow::exportSessionTxt);
  connect(m_exportSessionCsvBtn, &QPushButton::clicked, this,
          &MainWindow::exportSessionCsv);
  connect(m_exportSessionSummaryBtn, &QPushButton::clicked, this,
          &MainWindow::exportSessionSummaryClicked);

  connect(refreshFakePortsBtn, &QPushButton::clicked, this, &MainWindow::refreshFakeUmpPortsClicked);
  connect(openFakePortBtn, &QPushButton::clicked, this, &MainWindow::openFakeUmpPortClicked);
  connect(closeFakePortBtn, &QPushButton::clicked, this, &MainWindow::closeFakeUmpPortClicked);
  connect(startFakePollingBtn, &QPushButton::clicked, this, &MainWindow::startFakeUmpPollingClicked);
  connect(stopFakePollingBtn, &QPushButton::clicked, this, &MainWindow::stopFakeUmpPollingClicked);
  connect(clearFakeUmpBtn, &QPushButton::clicked, this, &MainWindow::clearFakeUmpClicked);
  connect(exportFakeUmpTxtBtn, &QPushButton::clicked, this, &MainWindow::exportExperimentalUmpTxtClicked);
  connect(exportFakeUmpCsvBtn, &QPushButton::clicked, this, &MainWindow::exportExperimentalUmpCsvClicked);

  connect(startFakeRecordingBtn, &QPushButton::clicked, this, &MainWindow::startFakeUmpRecordingClicked);
  connect(stopFakeRecordingBtn, &QPushButton::clicked, this, &MainWindow::stopFakeUmpRecordingClicked);
  connect(clearFakeSessionBtn, &QPushButton::clicked, this, &MainWindow::clearFakeUmpSessionClicked);
  connect(exportFakeSessionTxtBtn, &QPushButton::clicked, this, &MainWindow::exportFakeUmpSessionTxtClicked);
  connect(exportFakeSessionCsvBtn, &QPushButton::clicked, this, &MainWindow::exportFakeUmpSessionCsvClicked);
  connect(exportFakeSessionSummaryBtn, &QPushButton::clicked, this, &MainWindow::exportFakeUmpSessionSummaryClicked);

  auto filterChangedLog = [this]() {
    logMessage("Filtros do Live MIDI atualizados.");
  };
  connect(m_liveMidiTypeFilterCombo, &QComboBox::currentIndexChanged, this,
          filterChangedLog);
  connect(m_liveMidiChannelFilterCombo, &QComboBox::currentIndexChanged, this,
          filterChangedLog);
  connect(m_treatNoteOnZeroAsOffCb, &QCheckBox::stateChanged, this,
          filterChangedLog);

  logMessage(
      "Sistema inicializado. Aguardando pacotes UMP em formato hexadecimal.");
  updateLiveMidiStatus();
}

void MainWindow::logMessage(const QString &msg) {
  QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
  m_logPanel->append(QString("[%1] %2").arg(timestamp).arg(msg));
}

void MainWindow::updateDiagnostics() {
  int visibleRows = 0;
  for (int r = 0; r < m_tableWidget->rowCount(); ++r) {
    if (!m_tableWidget->isRowHidden(r)) {
      visibleRows++;
    }
  }

  QString filterStatus =
      m_filterField->text().isEmpty() ? "Desativado" : "Ativo";
  QString samplesStatus =
      m_samplesPath.isEmpty() ? "Não Encontrada" : "Encontrada";

  QString rtMidiStatus = "Desativado";
#ifdef USE_RTMIDI
  if (m_midiBackend && m_midiBackend->isOpen()) {
    rtMidiStatus = "Aberta (" + m_liveMidiPortsCombo->currentText() + ")";
  } else {
    rtMidiStatus = "Pronto";
  }
#endif

  QString text = QString("Arquivo Atual: %1 | Samples: %2 | Filtro: %3 | "
                         "Linhas: %4/%5 | MIDI: %6 | Última Operação: %7")
                     .arg(m_currentFile)
                     .arg(samplesStatus)
                     .arg(filterStatus)
                     .arg(visibleRows)
                     .arg(m_tableWidget->rowCount())
                     .arg(rtMidiStatus)
                     .arg(m_lastOperation);

  m_diagnosticsLabel->setText(text);
}

void MainWindow::interpretClicked() {
  QString input = m_inputField->toPlainText().trimmed();
  if (input.isEmpty()) {
    logMessage("Erro: Entrada vazia.");
    return;
  }

  m_tableWidget->setRowCount(0);
  logMessage("=== Nova Interpretação ===");

  int totalLidos = 0;
  int totalErros = 0;
  int totalValidos = 0;

  QStringList blocks =
      input.split(QRegularExpression("(\\r?\\n){2,}"), Qt::SkipEmptyParts);

  for (const QString &block : blocks) {
    QString trimmedBlock = block.trimmed();
    if (trimmedBlock.isEmpty())
      continue;

    ValidationResult result = UmpParser::validateAndExtractWords(trimmedBlock);

    if (!result.success) {
      if (result.errorType == UmpValidationError::EmptyAfterFormatting) {
        continue; // Ignora silenciosamente blocos só com comentários
      }
      logMessage(result.errorMessage);
      totalErros++;
      continue;
    }

    for (const auto &msgWords : result.extractedMessages) {
      totalLidos++;
      ParsedUmp parsed = UmpParser::parseMessage(msgWords);

      int rowCount = m_tableWidget->rowCount();
      m_tableWidget->insertRow(rowCount);

      // 0: Index
      QTableWidgetItem *itemIndex =
          new QTableWidgetItem(QString::number(rowCount + 1));
      itemIndex->setToolTip(itemIndex->text());
      m_tableWidget->setItem(rowCount, 0, itemIndex);

      // 1: Words
      QStringList wordsStrList;
      for (uint32_t w : parsed.words) {
        wordsStrList << QString("%1").arg(w, 8, 16, QChar('0')).toUpper();
      }
      QTableWidgetItem *itemWords =
          new QTableWidgetItem(wordsStrList.join(" "));
      itemWords->setToolTip(itemWords->text());
      m_tableWidget->setItem(rowCount, 1, itemWords);

      // 2: Size
      QTableWidgetItem *itemSize =
          new QTableWidgetItem(QString("%1 bits").arg(parsed.sizeBits));
      itemSize->setToolTip(itemSize->text());
      m_tableWidget->setItem(rowCount, 2, itemSize);

      // 3: Type
      QTableWidgetItem *itemType = new QTableWidgetItem(
          UmpParser::getMessageTypeString(parsed.messageType));
      itemType->setToolTip(itemType->text());
      m_tableWidget->setItem(rowCount, 3, itemType);

      // 4: Group
      QString groupStr =
          parsed.group >= 0 ? QString::number(parsed.group) : "-";
      QTableWidgetItem *itemGroup = new QTableWidgetItem(groupStr);
      itemGroup->setToolTip(itemGroup->text());
      m_tableWidget->setItem(rowCount, 4, itemGroup);

      // 5: Status
      QString statusStr =
          parsed.status >= 0
              ? QString("0x%1").arg(parsed.status, 1, 16).toUpper()
              : "-";
      QTableWidgetItem *itemStatus = new QTableWidgetItem(statusStr);
      itemStatus->setToolTip(itemStatus->text());
      m_tableWidget->setItem(rowCount, 5, itemStatus);

      // 6: Channel
      QString channelStr =
          parsed.channel >= 0 ? QString::number(parsed.channel + 1) : "-";
      QTableWidgetItem *itemChannel = new QTableWidgetItem(channelStr);
      itemChannel->setToolTip(itemChannel->text());
      m_tableWidget->setItem(rowCount, 6, itemChannel);

      // 7: Description
      QTableWidgetItem *itemDesc = new QTableWidgetItem(parsed.description);
      itemDesc->setToolTip(itemDesc->text());
      m_tableWidget->setItem(rowCount, 7, itemDesc);

      logMessage(
          QString("Pacote UMP de %1 bits interpretado.").arg(parsed.sizeBits));
      totalValidos++;
    }
  }

  m_statsLabel->setText(
      QString("Estatísticas: %1 lidos | %2 válidos | %3 erros")
          .arg(totalLidos)
          .arg(totalValidos)
          .arg(totalErros));

  // Aplicar filtro ativo se houver
  filterTable(m_filterField->text());

  m_lastOperation =
      totalErros > 0 ? "Interpretado com erros" : "Interpretado com sucesso";
  updateDiagnostics();
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
        m_currentFile = QFileInfo(fileName).fileName();
        m_lastOperation = "Arquivo externo aberto";
        updateDiagnostics();
      }
    } else {
      logMessage("Erro fatal ao tentar ler o arquivo selecionado no disco.");
    }
  }
}

void MainWindow::saveLogClicked() {
  QString fileName =
      QFileDialog::getSaveFileName(this, "Exportar Relatório", "ump_report.txt",
                                   "Text Files (*.txt);;All Files (*)");
  if (!fileName.isEmpty()) {
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      QTextStream out(&file);
      out << "=== Relatório do Analisador UMP ===\n";
      out << "Data/Hora: "
          << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")
          << "\n";
      out << m_statsLabel->text() << "\n\n";

      out << "=== Tabela de Resultados ===\n";
      out << "#\tWords\tSize\tType\tGroup\tStatus\tChannel\tDescription\n";
      for (int r = 0; r < m_tableWidget->rowCount(); ++r) {
        if (m_tableWidget->isRowHidden(r))
          continue;
        for (int c = 0; c < m_tableWidget->columnCount(); ++c) {
          QTableWidgetItem *item = m_tableWidget->item(r, c);
          out << (item ? item->text() : "") << "\t";
        }
        out << "\n";
      }

      out << "\n=== Log de Execucao ===\n" << m_logPanel->toPlainText() << "\n";

      logMessage(QString("Sucesso: Relatório salvo em %1").arg(fileName));
      m_lastOperation = QString("Relatório TXT salvo em %1")
                            .arg(QFileInfo(fileName).fileName());
      updateDiagnostics();
    } else {
      logMessage("Erro: Falha ao tentar salvar o relatório.");
    }
  }
}

void MainWindow::exportCsvClicked() {
  QString fileName =
      QFileDialog::getSaveFileName(this, "Exportar CSV", "ump_export.csv",
                                   "CSV Files (*.csv);;All Files (*)");
  if (!fileName.isEmpty()) {
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      QTextStream out(&file);

      QStringList headers;
      for (int c = 0; c < m_tableWidget->columnCount(); ++c) {
        headers << QString("\"%1\"").arg(
            m_tableWidget->horizontalHeaderItem(c)->text());
      }
      out << headers.join(";") << "\n";

      for (int r = 0; r < m_tableWidget->rowCount(); ++r) {
        if (m_tableWidget->isRowHidden(r))
          continue;
        QStringList rowData;
        for (int c = 0; c < m_tableWidget->columnCount(); ++c) {
          QTableWidgetItem *item = m_tableWidget->item(r, c);
          QString text = item ? item->text() : "";
          text.replace("\"", "\"\"");
          rowData << QString("\"%1\"").arg(text);
        }
        out << rowData.join(";") << "\n";
      }
      logMessage(QString("Sucesso: CSV salvo em %1").arg(fileName));
      m_lastOperation =
          QString("CSV salvo em %1").arg(QFileInfo(fileName).fileName());
      updateDiagnostics();
    } else {
      logMessage("Erro: Falha ao tentar salvar o arquivo CSV.");
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
  m_currentFile = "Nenhum";
  m_lastOperation = "Interface limpa";
  updateDiagnostics();
}

void MainWindow::loadExamplesClicked() {
  if (m_samplesPath.isEmpty() || m_samplesCombo->currentText().isEmpty()) {
    logMessage(
        "Erro: Nenhum exemplo selecionado ou pasta samples não encontrada.");
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
      m_currentFile = "Sample: " + fileName;
      m_lastOperation = "Exemplo carregado";
      updateDiagnostics();
    }
  } else {
    logMessage(
        QString("Erro ao tentar ler o arquivo de exemplo: %1").arg(fileName));
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
  updateDiagnostics();
}

void MainWindow::refreshMidiPortsClicked() {
  m_liveMidiPortsCombo->clear();
#ifdef USE_RTMIDI
  QStringList ports = m_midiBackend->listInputPorts();
  if (ports.isEmpty()) {
    m_liveMidiPortsCombo->addItem("Nenhuma porta MIDI encontrada.");
  } else {
    m_liveMidiPortsCombo->addItems(ports);
  }
  logMessage("Portas de entrada MIDI atualizadas com sucesso.");
#else
  m_liveMidiPortsCombo->addItem("RtMidi desativado nesta build.");
  logMessage("Aviso: Tentativa de listar portas, mas RtMidi está desativado.");
#endif
}

void MainWindow::openMidiPortClicked() {
#ifdef USE_RTMIDI
  int index = m_liveMidiPortsCombo->currentIndex();
  if (index < 0)
    return;

  if (m_midiBackend->isOpen()) {
    m_midiBackend->closeInputPort();
  }

  if (m_midiBackend->openInputPort(index)) {
    if (m_liveMidiTimer)
      m_liveMidiTimer->start(50);
    resetLiveMidiStats();
    m_liveMidiSessionStartTimeMs = QDateTime::currentMSecsSinceEpoch();
    logMessage(QString("Porta MIDI aberta com sucesso: %1")
                   .arg(m_liveMidiPortsCombo->currentText()));
    m_openMidiPortBtn->setEnabled(false);
    m_closeMidiPortBtn->setEnabled(true);
    m_refreshMidiPortsBtn->setEnabled(false);
    m_liveMidiPortsCombo->setEnabled(false);
    m_lastOperation = "Porta MIDI Aberta";
    updateLiveMidiStatus();
  } else {
    logMessage("Erro ao tentar abrir porta MIDI.");
  }
  updateDiagnostics();
#else
  logMessage("Aviso: Tentativa de abrir porta, mas RtMidi está desativado.");
#endif
}

void MainWindow::closeMidiPortClicked() {
#ifdef USE_RTMIDI
  if (m_midiBackend->isOpen()) {
    if (m_liveMidiTimer)
      m_liveMidiTimer->stop();
    m_midiBackend->closeInputPort();
    logMessage("Porta MIDI fechada com sucesso.");
    if (m_isLiveMidiRecording) {
      stopSessionRecording();
      logMessage("Gravação de sessão interrompida automaticamente devido ao "
                 "fechamento da porta.");
    }
    m_openMidiPortBtn->setEnabled(true);
    m_closeMidiPortBtn->setEnabled(false);
    m_refreshMidiPortsBtn->setEnabled(true);
    m_liveMidiPortsCombo->setEnabled(true);
    m_lastOperation = "Porta MIDI Fechada";
    updateLiveMidiStatus();
    updateDiagnostics();
  }
#else
  logMessage("Aviso: Tentativa de fechar porta, mas RtMidi está desativado.");
#endif
}

void MainWindow::pollLiveMidi() {
#ifdef USE_RTMIDI
  if (!m_midiBackend || !m_midiBackend->isOpen())
    return;

  std::vector<MidiRawEvent> events = m_midiBackend->pollEvents();
  if (events.empty())
    return;

  for (const auto &ev : events) {
    if (ev.sourceType == InputSourceType::LiveMidi1Bytes) {
      m_liveMidiStats.received++;

      QString hexStr;
      for (uint8_t byte : ev.midi1Bytes) {
        hexStr += QString("%1 ").arg(byte, 2, 16, QChar('0')).toUpper();
      }

      bool treatZeroVel = m_treatNoteOnZeroAsOffCb->isChecked();
      Midi1DecodedMessage decodedMsg =
          Midi1LiveDecoder::decodeDetailed(ev.midi1Bytes, treatZeroVel);

      Midi1ToUmpPreviewResult previewResult =
          Midi1ToUmpPreviewConverter::convert(ev.midi1Bytes);

      if (m_isLiveMidiRecording) {
        LiveMidiRecordedEvent recEvent;
        recEvent.timestamp = QString("%1").arg(ev.timestamp, 0, 'f', 3);
        recEvent.bytesHex = hexStr.trimmed();
        recEvent.description = decodedMsg.description;
        recEvent.messageType = decodedMsg.messageType;
        recEvent.channel = decodedMsg.channel;
        if (previewResult.supported) {
          recEvent.umpPreview = previewResult.umpHex;
        } else {
          recEvent.umpPreview = "";
        }
        m_liveMidiRecording.append(recEvent);
        m_liveMidiRecordedCount++;
        if (m_liveMidiRecordedCount % 10 == 0 || m_liveMidiRecordedCount < 10) {
          m_sessionCountLabel->setText(
              QString("Eventos gravados: %1").arg(m_liveMidiRecordedCount));
        }
      }

      if (!m_isLiveMidiPaused) {

        QString selectedType = m_liveMidiTypeFilterCombo->currentText();
        bool typePasses =
            (selectedType == "Todos" || selectedType == decodedMsg.messageType);

        int selectedChannelIdx = m_liveMidiChannelFilterCombo->currentIndex();
        bool channelPasses = false;
        if (selectedChannelIdx == 0) {
          channelPasses = true; // "Todos os canais"
        } else {
          channelPasses = (decodedMsg.channel == selectedChannelIdx);
        }

        if (typePasses && channelPasses) {
          m_liveMidiStats.displayed++;

          QString msg = QString("[%1s] %2 | %3")
                            .arg(ev.timestamp, 0, 'f', 3)
                            .arg(hexStr.trimmed(), -8)
                            .arg(decodedMsg.description);

          LiveMidiLogEntry entry;
          entry.timestamp = QString("%1").arg(ev.timestamp, 0, 'f', 3);
          entry.bytesHex = hexStr.trimmed();
          entry.description = decodedMsg.description;

          if (previewResult.supported) {
            entry.umpPreview = previewResult.umpHex;
            m_umpPreviewLabel->setText(
                QString("Último UMP Gerado: %1").arg(previewResult.umpHex));
            if (m_umpPreviewCb->isChecked()) {
              msg += " | UMP: " + previewResult.umpHex;
            }
            addLiveUmpPreviewRow(entry.timestamp, entry.bytesHex, previewResult,
                                 decodedMsg);
          } else {
            entry.umpPreview = "";
            m_umpPreviewLabel->setText("Último UMP Gerado: " +
                                       previewResult.reason);
          }

          if (m_liveMidiLog) {
            m_liveMidiLog->append(msg);
          }

          m_liveMidiEvents.append(entry);

          if (m_liveMidiEvents.size() > 1000) {
            m_liveMidiEvents.removeFirst();
          }
        }

        updateLiveMidiStats(decodedMsg, (typePasses && channelPasses));
      } else {
        // Even paused, we count received types (but display won't update)
        QString hexStr;
        for (uint8_t byte : ev.midi1Bytes) {
          hexStr += QString("%1 ").arg(byte, 2, 16, QChar('0')).toUpper();
        }

        bool treatZeroVel = m_treatNoteOnZeroAsOffCb->isChecked();
        Midi1DecodedMessage decodedMsg =
            Midi1LiveDecoder::decodeDetailed(ev.midi1Bytes, treatZeroVel);

        if (m_isLiveMidiRecording) {
          Midi1ToUmpPreviewResult previewResult =
              Midi1ToUmpPreviewConverter::convert(ev.midi1Bytes);
          LiveMidiRecordedEvent recEvent;
          recEvent.timestamp = QString("%1").arg(ev.timestamp, 0, 'f', 3);
          recEvent.bytesHex = hexStr.trimmed();
          recEvent.description = decodedMsg.description;
          recEvent.messageType = decodedMsg.messageType;
          recEvent.channel = decodedMsg.channel;
          if (previewResult.supported) {
            recEvent.umpPreview = previewResult.umpHex;
          } else {
            recEvent.umpPreview = "";
          }
          m_liveMidiRecording.append(recEvent);
          m_liveMidiRecordedCount++;
          if (m_liveMidiRecordedCount % 10 == 0 ||
              m_liveMidiRecordedCount < 10) {
            m_sessionCountLabel->setText(
                QString("Eventos gravados: %1").arg(m_liveMidiRecordedCount));
          }
        }
        updateLiveMidiStats(decodedMsg, false);
      }
    }
  }
  refreshLiveMidiStatsUi();
  updateLiveMidiStatus();
#else
  // Stub seguro para quando RtMidi está desativado
  if (m_liveMidiTimer && m_liveMidiTimer->isActive()) {
    m_liveMidiTimer->stop();
  }
#endif
}

void MainWindow::addLiveUmpPreviewRow(const QString &timestamp,
                                      const QString &midiBytes,
                                      const Midi1ToUmpPreviewResult &result,
                                      const Midi1DecodedMessage &decoded) {
  if (!m_liveUmpPreviewTable)
    return;

  int row = m_liveUmpPreviewTable->rowCount();
  m_liveUmpPreviewTable->insertRow(row);

  QTableWidgetItem *itemTimestamp = new QTableWidgetItem(timestamp);
  QTableWidgetItem *itemBytes = new QTableWidgetItem(midiBytes);
  QTableWidgetItem *itemUmp = new QTableWidgetItem(result.umpHex);
  QTableWidgetItem *itemMt = new QTableWidgetItem(
      QString("0x%1").arg((result.umpWord >> 28) & 0xF, 1, 16).toUpper());
  QTableWidgetItem *itemGroup =
      new QTableWidgetItem(QString::number((result.umpWord >> 24) & 0xF));
  QTableWidgetItem *itemStatus = new QTableWidgetItem(
      QString("0x%1 %2")
          .arg((result.umpWord >> 16) & 0xF0, 2, 16, QChar('0'))
          .toUpper()
          .arg(decoded.messageType));
  QTableWidgetItem *itemChannel =
      new QTableWidgetItem(QString::number(decoded.channel));
  QTableWidgetItem *itemDesc = new QTableWidgetItem(decoded.description);

  m_liveUmpPreviewTable->setItem(row, 0, itemTimestamp);
  m_liveUmpPreviewTable->setItem(row, 1, itemBytes);
  m_liveUmpPreviewTable->setItem(row, 2, itemUmp);
  m_liveUmpPreviewTable->setItem(row, 3, itemMt);
  m_liveUmpPreviewTable->setItem(row, 4, itemGroup);
  m_liveUmpPreviewTable->setItem(row, 5, itemStatus);
  m_liveUmpPreviewTable->setItem(row, 6, itemChannel);
  m_liveUmpPreviewTable->setItem(row, 7, itemDesc);

  m_liveUmpPreviewTable->scrollToBottom();

  if (m_liveUmpPreviewTable->rowCount() > 1000) {
    m_liveUmpPreviewTable->removeRow(0);
  }
}

void MainWindow::clearLiveUmpPreviewTable() {
  if (m_liveUmpPreviewTable) {
    m_liveUmpPreviewTable->setRowCount(0);
    m_umpPreviewLabel->setText("Último UMP Gerado: Nenhum");
  }
}

void MainWindow::pauseLiveMidiClicked() {
  m_isLiveMidiPaused = !m_isLiveMidiPaused;
  if (m_isLiveMidiPaused) {
    m_pauseLiveMidiBtn->setText("Retomar Monitor");
  } else {
    m_pauseLiveMidiBtn->setText("Pausar Monitor");
  }
  updateLiveMidiStatus();
}

void MainWindow::clearLiveMidiLogClicked() {
  m_liveMidiLog->clear();
  clearLiveUmpPreviewTable();
  m_liveMidiEvents.clear();
  resetLiveMidiStats();
  m_liveMidiSessionStartTimeMs = QDateTime::currentMSecsSinceEpoch();
  refreshLiveMidiStatsUi();
  updateLiveMidiStatus();
}

void MainWindow::updateLiveMidiStatus() {
#ifdef USE_RTMIDI
  if (!m_midiBackend || !m_midiBackend->isOpen()) {
    m_liveMidiStatusLabel->setText("Status: Porta fechada");
    m_pauseLiveMidiBtn->setEnabled(false);
  } else {
    m_pauseLiveMidiBtn->setEnabled(true);
    if (m_isLiveMidiPaused) {
      m_liveMidiStatusLabel->setText("Status: Pausado");
    } else {
      m_liveMidiStatusLabel->setText("Status: Monitorando");
    }
  }
#else
  m_liveMidiStatusLabel->setText("Status: RtMidi desativado nesta build");
  m_pauseLiveMidiBtn->setEnabled(false);
#endif

  m_liveMidiCountersLabel->setText(QString("Recebidas: %1 | Exibidas: %2")
                                       .arg(m_liveMidiStats.received)
                                       .arg(m_liveMidiStats.displayed));
}

void MainWindow::exportLiveTxtClicked() {
  if (m_liveMidiEvents.isEmpty()) {
    QMessageBox::information(this, "Aviso",
                             "Não há eventos Live MIDI para exportar.");
    return;
  }

  QString defaultFileName =
      QString("LiveMidiExport_%1.txt")
          .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
  QString fileName = QFileDialog::getSaveFileName(
      this, "Salvar Exportação Live MIDI (TXT)", defaultFileName,
      "Arquivos de Texto (*.txt);;Todos os Arquivos (*)");

  if (fileName.isEmpty()) {
    logMessage("Exportação cancelada.");
    return;
  }

  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    logMessage("Erro ao salvar exportação: Não foi possível abrir o arquivo "
               "para escrita.");
    return;
  }

  QTextStream out(&file);
  out << "MidiUmpAnalyzer - Live MIDI Monitor Export\n";
  out << "Version: v2.11.0\n";
  out << "Exported at: "
      << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "\n";
  out << m_liveMidiStatusLabel->text() << "\n";
  out << "Received: " << m_liveMidiStats.received << "\n";
  out << "Displayed: " << m_liveMidiStats.displayed << "\n";
  out << "Max live log lines: 1000\n\n";
  out << "Live MIDI Log:\n";

  for (const auto &ev : m_liveMidiEvents) {
    QString line = QString("[%1s] %2 | %3")
                       .arg(ev.timestamp)
                       .arg(ev.bytesHex)
                       .arg(ev.description);
    if (m_umpPreviewCb->isChecked() && !ev.umpPreview.isEmpty()) {
      line += " | UMP: " + ev.umpPreview;
    }
    out << line << "\n";
  }

  file.close();
  logMessage("Live MIDI TXT exportado com sucesso para: " + fileName);
}

void MainWindow::exportLiveCsvClicked() {
  if (m_liveMidiEvents.isEmpty()) {
    QMessageBox::information(this, "Aviso",
                             "Não há eventos Live MIDI para exportar.");
    return;
  }

  QString defaultFileName =
      QString("LiveMidiExport_%1.csv")
          .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
  QString fileName = QFileDialog::getSaveFileName(
      this, "Salvar Exportação Live MIDI (CSV)", defaultFileName,
      "Arquivos CSV (*.csv);;Todos os Arquivos (*)");

  if (fileName.isEmpty()) {
    logMessage("Exportação cancelada.");
    return;
  }

  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    logMessage("Erro ao salvar exportação: Não foi possível abrir o arquivo "
               "para escrita.");
    return;
  }

  QTextStream out(&file);
  out.setEncoding(QStringConverter::Utf8);
  out << "timestamp;bytes_hex;description;ump_preview\n";

  for (const auto &ev : m_liveMidiEvents) {
    QString desc = ev.description;
    desc.replace("\"", "\"\"");
    out << ev.timestamp << ";" << ev.bytesHex << ";\"" << desc << "\";"
        << ev.umpPreview << "\n";
  }

  file.close();
  logMessage("Live MIDI CSV exportado com sucesso para: " + fileName);
}

void MainWindow::updateLiveMidiStats(const Midi1DecodedMessage &decoded,
                                     bool displayed) {
  if (decoded.messageType == "Note On")
    m_liveMidiStats.byTypeNoteOn++;
  else if (decoded.messageType == "Note Off")
    m_liveMidiStats.byTypeNoteOff++;
  else if (decoded.messageType == "Control Change")
    m_liveMidiStats.byTypeControlChange++;
  else if (decoded.messageType == "Program Change")
    m_liveMidiStats.byTypeProgramChange++;
  else if (decoded.messageType == "Pitch Bend")
    m_liveMidiStats.byTypePitchBend++;
  else if (decoded.messageType == "Poly Aftertouch")
    m_liveMidiStats.byTypePolyAftertouch++;
  else if (decoded.messageType == "Channel Aftertouch")
    m_liveMidiStats.byTypeChannelAftertouch++;
  else if (decoded.messageType == "System/Common/Real-Time")
    m_liveMidiStats.byTypeSystem++;
  else
    m_liveMidiStats.byTypeUnknown++;

  if (decoded.channel >= 1 && decoded.channel <= 16) {
    m_liveMidiStats.byChannel[decoded.channel - 1]++;
  } else {
    m_liveMidiStats.noChannel++;
  }

  m_liveMidiStats.lastMessageType = decoded.messageType;
  m_liveMidiStats.lastChannel = decoded.channel;

  if (decoded.note != -1)
    m_liveMidiStats.lastNote = decoded.note;
  if (decoded.velocity != -1)
    m_liveMidiStats.lastVelocity = decoded.velocity;
  if (decoded.controller != -1)
    m_liveMidiStats.lastCc = decoded.controller;
  if (decoded.value != -1)
    m_liveMidiStats.lastCcValue = decoded.value;
  if (decoded.program != -1)
    m_liveMidiStats.lastProgram = decoded.program;
  if (decoded.pitchBend != -1)
    m_liveMidiStats.lastPitchBend = decoded.pitchBend;
}

void MainWindow::refreshLiveMidiStatsUi() {
  if (!m_midiBackend ||
      (!m_midiBackend->isOpen() && m_liveMidiStats.received == 0)) {
    m_liveMidiStatsLabel->setText("Aguardando porta abrir...");
    return;
  }

  qint64 elapsedMs =
      QDateTime::currentMSecsSinceEpoch() - m_liveMidiSessionStartTimeMs;
  double seconds = elapsedMs > 0 ? elapsedMs / 1000.0 : 0.001;
  double rate = m_liveMidiStats.received / seconds;

  quint64 filtered = m_liveMidiStats.received > m_liveMidiStats.displayed
                         ? m_liveMidiStats.received - m_liveMidiStats.displayed
                         : 0;

  QString text =
      QString(
          "<b>Received:</b> %1 &nbsp;&nbsp; <b>Displayed:</b> %2 &nbsp;&nbsp; "
          "<b>Filtered:</b> %3 &nbsp;&nbsp; <b>Rate:</b> %4 msg/s<br>")
          .arg(m_liveMidiStats.received)
          .arg(m_liveMidiStats.displayed)
          .arg(filtered)
          .arg(rate, 0, 'f', 1);

  text +=
      QString("<b>Last message:</b> %1 ").arg(m_liveMidiStats.lastMessageType);
  if (m_liveMidiStats.lastChannel != -1)
    text += QString("[Ch %1] ").arg(m_liveMidiStats.lastChannel);

  if (m_liveMidiStats.lastNote != -1)
    text += QString("&nbsp;&nbsp; <b>Last Note:</b> %1 Vel: %2")
                .arg(m_liveMidiStats.lastNote)
                .arg(m_liveMidiStats.lastVelocity);
  if (m_liveMidiStats.lastCc != -1)
    text += QString("&nbsp;&nbsp; <b>Last CC:</b> %1 Val: %2")
                .arg(m_liveMidiStats.lastCc)
                .arg(m_liveMidiStats.lastCcValue);
  if (m_liveMidiStats.lastProgram != -1)
    text += QString("&nbsp;&nbsp; <b>Last Program:</b> %1")
                .arg(m_liveMidiStats.lastProgram);
  if (m_liveMidiStats.lastPitchBend != -1)
    text += QString("&nbsp;&nbsp; <b>Last Pitch Bend:</b> %1")
                .arg(m_liveMidiStats.lastPitchBend);

  text += "<br><b>By type:</b> Note On: " +
          QString::number(m_liveMidiStats.byTypeNoteOn) +
          " | Note Off: " + QString::number(m_liveMidiStats.byTypeNoteOff) +
          " | CC: " + QString::number(m_liveMidiStats.byTypeControlChange) +
          " | Pitch Bend: " + QString::number(m_liveMidiStats.byTypePitchBend) +
          " | Sys: " + QString::number(m_liveMidiStats.byTypeSystem);

  QString channelStr;
  for (int i = 0; i < 16; ++i) {
    if (m_liveMidiStats.byChannel[i] > 0) {
      channelStr +=
          QString("Ch %1: %2 | ").arg(i + 1).arg(m_liveMidiStats.byChannel[i]);
    }
  }
  if (m_liveMidiStats.noChannel > 0)
    channelStr += QString("Sys/None: %1").arg(m_liveMidiStats.noChannel);
  else if (!channelStr.isEmpty())
    channelStr.chop(3); // Remove last " | "

  if (!channelStr.isEmpty()) {
    text += "<br><b>By channel:</b> " + channelStr;
  }

  m_liveMidiStatsLabel->setText(text);
}

void MainWindow::resetLiveMidiStats() { m_liveMidiStats = LiveMidiStats(); }

void MainWindow::startSessionRecording() {
  if (m_liveMidiRecordedCount > 0) {
    clearSessionRecording();
  }
  m_isLiveMidiRecording = true;
  m_startSessionBtn->setEnabled(false);
  m_stopSessionBtn->setEnabled(true);
  m_sessionStatusLabel->setText("Status: Gravando");
  logMessage("Gravação de sessão MIDI iniciada.");
}

void MainWindow::stopSessionRecording() {
  m_isLiveMidiRecording = false;
  m_startSessionBtn->setEnabled(true);
  m_stopSessionBtn->setEnabled(false);
  m_sessionStatusLabel->setText("Status: Parada");
  logMessage(
      QString("Gravação de sessão MIDI parada. Total de eventos gravados: %1")
          .arg(m_liveMidiRecordedCount));
}

void MainWindow::clearSessionRecording() {
  m_liveMidiRecording.clear();
  m_liveMidiRecordedCount = 0;
  m_sessionCountLabel->setText("Eventos gravados: 0");
  logMessage("Sessão MIDI gravada foi limpa.");
}

void MainWindow::exportSessionTxt() {
  if (m_liveMidiRecording.isEmpty()) {
    logMessage("Erro: Não há eventos gravados na sessão para exportar.");
    return;
  }
  QString fileName = QFileDialog::getSaveFileName(
      this, "Exportar Sessão MIDI TXT",
      QDir::homePath() + "/LiveMidiSession.txt", "Text Files (*.txt)");
  if (fileName.isEmpty())
    return;
  QFile file(fileName);
  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&file);
    out << "MidiUmpAnalyzer - Live MIDI Session Recording\n";
    out << "Version: v2.16.0\n";
    out << "Exported at: "
        << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "\n";
    out << "Recorded events: " << m_liveMidiRecordedCount << "\n";
    out << "Note: Session recording captures received Live MIDI events while "
           "recording is active.\n\n";
    for (const auto &ev : m_liveMidiRecording) {
      out << "[" << ev.timestamp << "s] " << ev.bytesHex << " | "
          << ev.description << " | Type: " << ev.messageType;
      if (ev.channel != -1) {
        out << " | Channel: " << ev.channel;
      }
      if (!ev.umpPreview.isEmpty()) {
        out << " | UMP: " << ev.umpPreview;
      }
      out << "\n";
    }
    file.close();
    logMessage("Sessão MIDI gravada exportada com sucesso (TXT).");
  } else {
    logMessage("Erro ao exportar Sessão MIDI gravada (TXT).");
  }
}

void MainWindow::exportSessionCsv() {
  if (m_liveMidiRecording.isEmpty()) {
    logMessage("Erro: Não há eventos gravados na sessão para exportar.");
    return;
  }
  QString fileName = QFileDialog::getSaveFileName(
      this, "Exportar Sessão MIDI CSV",
      QDir::homePath() + "/LiveMidiSession.csv", "CSV Files (*.csv)");
  if (fileName.isEmpty())
    return;
  QFile file(fileName);
  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << "timestamp;bytes_hex;description;message_type;channel;ump_preview\n";
    for (const auto &ev : m_liveMidiRecording) {
      QString desc = ev.description;
      desc.replace("\"", "\"\"");
      out << ev.timestamp << ";" << ev.bytesHex << ";\"" << desc << "\";\""
          << ev.messageType << "\";";
      if (ev.channel != -1) {
        out << ev.channel;
      }
      out << ";\"" << ev.umpPreview << "\"\n";
    }
    file.close();
    logMessage("Sessão MIDI gravada exportada com sucesso (CSV).");
  } else {
    logMessage("Erro ao exportar Sessão MIDI gravada (CSV).");
  }
}

LiveMidiSessionSummary MainWindow::buildLiveMidiSessionSummary() const {
  LiveMidiSessionSummary summary;
  summary.totalEvents = m_liveMidiRecording.size();

  if (summary.totalEvents == 0)
    return summary;

  summary.firstTimestamp = m_liveMidiRecording.first().timestamp;
  summary.lastTimestamp = m_liveMidiRecording.last().timestamp;

  summary.approximateDurationSeconds =
      summary.lastTimestamp.toDouble() - summary.firstTimestamp.toDouble();

  for (const auto &ev : m_liveMidiRecording) {
    if (!ev.umpPreview.isEmpty()) {
      summary.umpSupported++;
    } else {
      summary.umpUnsupported++;
    }

    summary.byType[ev.messageType]++;

    if (ev.channel >= 1 && ev.channel <= 16) {
      summary.byChannel[ev.channel - 1]++;
    } else {
      summary.noChannel++;
    }

    if (!summary.hasFirstNoteOn && ev.messageType == "Note On") {
      summary.firstNoteOn = ev;
      summary.hasFirstNoteOn = true;
    }
    if (!summary.hasFirstNoteOff && ev.messageType == "Note Off") {
      summary.firstNoteOff = ev;
      summary.hasFirstNoteOff = true;
    }
    if (!summary.hasFirstControlChange && ev.messageType == "Control Change") {
      summary.firstControlChange = ev;
      summary.hasFirstControlChange = true;
    }
    if (!summary.hasFirstProgramChange && ev.messageType == "Program Change") {
      summary.firstProgramChange = ev;
      summary.hasFirstProgramChange = true;
    }
    if (!summary.hasFirstPitchBend && ev.messageType == "Pitch Bend") {
      summary.firstPitchBend = ev;
      summary.hasFirstPitchBend = true;
    }
  }

  return summary;
}

QString
MainWindow::formatRecordedExample(const QString &title, bool hasEvent,
                                  const LiveMidiRecordedEvent &event) const {
  QString out;
  QTextStream stream(&out);
  if (hasEvent) {
    stream << "First " << title << "\n";
    stream << "  Timestamp: " << event.timestamp << "s\n";
    stream << "  MIDI bytes: " << event.bytesHex << "\n";
    stream << "  Decoded: " << event.description << "\n";
    stream << "  UMP Preview: " << event.umpPreview;
  } else {
    stream << "First " << title << ": not present in this session.";
  }
  return out;
}

QString MainWindow::formatLiveMidiSessionSummaryReport(
    const LiveMidiSessionSummary &summary) const {
  QString out;
  QTextStream stream(&out);

  stream << "MidiUmpAnalyzer - Live MIDI Session Summary Report\n";
  stream << "Version: v4.16.0\n";
  stream << "Exported at: "
         << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")
         << "\n\n";

  stream << "2. Purpose\n";
  stream << "------------------------------------------------------------------"
            "--------------\n";
  stream << "This report summarizes a recorded Live MIDI session captured "
            "through the\n"
            "experimental RtMidi backend. It is intended to document received "
            "MIDI 1.0\n"
            "messages, their decoded types/channels, and their optional UMP "
            "MIDI 1.0\n"
            "Channel Voice preview.\n\n";

  stream << "3. Methodology\n";
  stream << "------------------------------------------------------------------"
            "--------------\n";
  stream << "- The session recording captures received Live MIDI events while "
            "recording is active.\n";
  stream << "- Recording is independent from visual filters.\n";
  stream << "- Recording can continue while the visual monitor is paused.\n";
  stream
      << "- Each received MIDI 1.0 event is decoded using Midi1LiveDecoder.\n";
  stream << "- Supported Channel Voice events may receive an UMP Preview using "
            "Midi1ToUmpPreviewConverter.\n\n";

  stream << "4. Session Overview\n";
  stream << "------------------------------------------------------------------"
            "--------------\n";
  stream << "- Recording status: "
         << (m_isLiveMidiRecording ? "Active" : "Stopped") << "\n";
  stream << "- Recorded events: " << summary.totalEvents << "\n";
  stream << "- Events with UMP Preview: " << summary.umpSupported << "\n";
  stream << "- Events without UMP Preview: " << summary.umpUnsupported << "\n";
  stream << "- First timestamp: " << summary.firstTimestamp << "s\n";
  stream << "- Last timestamp: " << summary.lastTimestamp << "s\n";
  stream << "- Approximate duration: "
         << QString::number(summary.approximateDurationSeconds, 'f', 3)
         << " seconds\n\n";

  stream << "5. Message Type Distribution\n";
  stream << "------------------------------------------------------------------"
            "--------------\n";
  stream << "- Note On: " << summary.byType.value("Note On", 0) << "\n";
  stream << "- Note Off: " << summary.byType.value("Note Off", 0) << "\n";
  stream << "- Control Change: " << summary.byType.value("Control Change", 0)
         << "\n";
  stream << "- Program Change: " << summary.byType.value("Program Change", 0)
         << "\n";
  stream << "- Pitch Bend: " << summary.byType.value("Pitch Bend", 0) << "\n";
  stream << "- Poly Aftertouch: " << summary.byType.value("Poly Aftertouch", 0)
         << "\n";
  stream << "- Channel Aftertouch: "
         << summary.byType.value("Channel Aftertouch", 0) << "\n";
  stream << "- System/Common/Real-Time: "
         << summary.byType.value("System/Common/Real-Time", 0) << "\n";

  QStringList standardTypes = {"Note On",
                               "Note Off",
                               "Control Change",
                               "Program Change",
                               "Pitch Bend",
                               "Poly Aftertouch",
                               "Channel Aftertouch",
                               "System/Common/Real-Time"};
  int unknownCount = 0;
  for (auto it = summary.byType.constBegin(); it != summary.byType.constEnd();
       ++it) {
    if (!standardTypes.contains(it.key())) {
      unknownCount += it.value();
    }
  }
  stream << "- Unknown/Unsupported: " << unknownCount << "\n\n";

  stream << "6. Channel Distribution\n";
  stream << "------------------------------------------------------------------"
            "--------------\n";
  for (int i = 0; i < 16; ++i) {
    stream << "- Ch " << (i + 1) << ": " << summary.byChannel[i] << "\n";
  }
  stream << "- No channel/System: " << summary.noChannel << "\n\n";

  stream << "7. UMP Preview Summary\n";
  stream << "------------------------------------------------------------------"
            "--------------\n";
  stream << "- Supported UMP Preview events: " << summary.umpSupported << "\n";
  stream << "- Unsupported for UMP Preview: " << summary.umpUnsupported << "\n";
  stream << "- UMP format used: MIDI 1.0 Channel Voice in UMP\n";
  stream << "- Message Type used: 0x2\n";
  stream << "- Default Group: 0\n";
  stream << "- Important: This report does not demonstrate MIDI 2.0 Channel "
            "Voice MT 0x4 conversion.\n\n";

  stream << "8. Representative Examples\n";
  stream << "------------------------------------------------------------------"
            "--------------\n";
  stream << formatRecordedExample("Note On", summary.hasFirstNoteOn,
                                  summary.firstNoteOn)
         << "\n\n";
  stream << formatRecordedExample("Note Off", summary.hasFirstNoteOff,
                                  summary.firstNoteOff)
         << "\n\n";
  stream << formatRecordedExample("Control Change",
                                  summary.hasFirstControlChange,
                                  summary.firstControlChange)
         << "\n\n";
  stream << formatRecordedExample("Program Change",
                                  summary.hasFirstProgramChange,
                                  summary.firstProgramChange)
         << "\n\n";
  stream << formatRecordedExample("Pitch Bend", summary.hasFirstPitchBend,
                                  summary.firstPitchBend)
         << "\n\n";

  stream << "9. Limitations\n";
  stream << "------------------------------------------------------------------"
            "--------------\n";
  stream
      << "- RtMidi provides MIDI 1.0 byte streams, not native UMP packets.\n";
  stream << "- UMP Preview currently supports only MIDI 1.0 Channel Voice "
            "messages.\n";
  stream << "- System Common, System Real-Time and SysEx messages are not "
            "converted to UMP Preview in this version.\n";
  stream << "- MIDI-CI, Property Exchange and Profiles are not implemented.\n";
  stream << "- MIDI 2.0 Channel Voice MT 0x4 is not implemented in this "
            "preview.\n\n";

  stream << "10. Interpretation Notes\n";
  stream << "------------------------------------------------------------------"
            "--------------\n";
  stream << "- UMP MT 0x2 is an encapsulation of MIDI 1.0 Channel Voice "
            "messages inside a 32-bit UMP word.\n";
  stream << "- The preview word is built as:\n";
  stream << "  0x20000000 | (group << 24) | (status << 16) | (data1 << 8) | "
            "data2\n";
  stream << "- For 2-byte MIDI messages such as Program Change and Channel "
            "Aftertouch, data2 is padded with 0.\n";

  return out;
}

#include <QMessageBox>
void MainWindow::exportSessionSummaryClicked() {
  if (m_liveMidiRecording.isEmpty()) {
    QMessageBox::information(
        this, "Aviso",
        "Não há eventos gravados na sessão para gerar um resumo.");
    return;
  }

  QString fileName = QFileDialog::getSaveFileName(
      this, "Exportar Resumo da Sessão",
      QDir::homePath() + "/LiveMidiSessionSummary.txt", "Text Files (*.txt)");
  if (fileName.isEmpty()) {
    logMessage("Exportação do resumo da sessão cancelada.");
    return;
  }

  QFile file(fileName);
  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    LiveMidiSessionSummary summary = buildLiveMidiSessionSummary();
    QString report = formatLiveMidiSessionSummaryReport(summary);

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << report;
    file.close();
    logMessage("Resumo da sessão Live MIDI exportado com sucesso.");
  } else {
    logMessage("Erro ao exportar Resumo da Sessão MIDI.");
  }
}

// =======================================================================
// EXPERIMENTAL UMP BACKEND METHODS
// =======================================================================

void MainWindow::refreshFakeUmpPortsClicked() {
  if (!m_fakeUmpBackend) {
      m_fakeUmpBackend = std::make_unique<FakeUmpInputBackend>();
  }
  m_fakeUmpPortCombo->clear();
  QStringList ports = m_fakeUmpBackend->listInputPorts();
  m_fakeUmpPortCombo->addItems(ports);
  logFakeUmpMessage("Portas atualizadas.");
}

void MainWindow::openFakeUmpPortClicked() {
  if (!m_fakeUmpBackend) return;
  int idx = m_fakeUmpPortCombo->currentIndex();
  if (idx < 0) return;
  if (m_fakeUmpBackend->openInputPort(idx)) {
      logFakeUmpMessage("Porta aberta: " + m_fakeUmpPortCombo->currentText());
  } else {
      logFakeUmpMessage("Falha ao abrir porta.");
  }
  updateFakeUmpStatus();
}

void MainWindow::closeFakeUmpPortClicked() {
  if (m_fakeUmpTimer) {
      m_fakeUmpTimer->stop();
  }
  if (m_fakeUmpBackend) {
      m_fakeUmpBackend->closeInputPort();
      logFakeUmpMessage("Porta fechada.");
  }
  updateFakeUmpStatus();
  if (m_isFakeUmpRecording) {
      stopFakeUmpRecordingClicked();
  }
}

void MainWindow::startFakeUmpPollingClicked() {
  if (!m_fakeUmpBackend || !m_fakeUmpBackend->isOpen()) {
      logFakeUmpMessage("Erro: Abra a porta antes de iniciar o polling.");
      return;
  }
  if (!m_fakeUmpTimer) {
      m_fakeUmpTimer = new QTimer(this);
      connect(m_fakeUmpTimer, &QTimer::timeout, this, &MainWindow::pollFakeUmpBackend);
  }
  m_fakeUmpTimer->start(1000);
  logFakeUmpMessage("Polling iniciado (1000ms).");
  updateFakeUmpStatus();
}

void MainWindow::stopFakeUmpPollingClicked() {
  if (m_fakeUmpTimer) {
      m_fakeUmpTimer->stop();
      logFakeUmpMessage("Polling parado.");
  }
  updateFakeUmpStatus();
}

void MainWindow::clearFakeUmpClicked() {
  m_fakeUmpTable->setRowCount(0);
  m_fakeUmpLog->clear();
  m_fakeUmpReceivedCount = 0;
  m_fakeUmpCounterLabel->setText("Eventos: 0");
  logFakeUmpMessage("Tabela e contadores limpos.");
}

void MainWindow::pollFakeUmpBackend() {
  if (!m_fakeUmpBackend || !m_fakeUmpBackend->isOpen()) return;
  auto events = m_fakeUmpBackend->pollUmpEvents();
  for (const auto& ev : events) {
      addFakeUmpEventRow(ev);
      m_fakeUmpReceivedCount++;

      if (m_isFakeUmpRecording) {
          FakeUmpRecordedEvent rev;
          rev.timestamp = QString::number(ev.timestampMs, 'f', 2) + " ms";
          rev.backend = ev.backendName;
          rev.port = ev.portName;

          QString wordsHex;
          for (uint32_t w : ev.umpWords) {
              if (!wordsHex.isEmpty()) wordsHex += " ";
              wordsHex += QString("%1").arg(w, 8, 16, QChar('0')).toUpper();
          }
          rev.umpWords = wordsHex;
          
          rev.bits = "?";
          rev.messageType = "Unknown";
          rev.group = "?";
          rev.status = "-";
          rev.channel = "-";
          rev.size = "?";

          if (!ev.umpWords.empty()) {
              uint32_t first = ev.umpWords[0];
              uint8_t mt = (first >> 28) & 0xF;
              uint8_t group = (first >> 24) & 0xF;
              
              rev.messageType = QString("0x%1").arg(mt, 1, 16, QChar('0')).toUpper();
              rev.group = QString::number(group + 1);
              
              rev.bits = QString::number(ev.umpWords.size() * 32);
              rev.size = getUmpSizeLabel(mt);

              if (mt == 0x2 || mt == 0x4) {
                  uint8_t statusByte = (first >> 16) & 0xFF;
                  uint8_t statusNibble = statusByte & 0xF0;
                  uint8_t channel = (statusByte & 0x0F) + 1;
                  
                  rev.status = QString("0x%1 %2").arg(statusNibble, 2, 16, QChar('0')).toUpper().arg(getUmpStatusLabel(statusNibble));
                  rev.channel = QString::number(channel);
              }
          }

          QString desc = "Fake UMP Event";
          auto result = UmpParser::validateAndExtractWords(wordsHex);
          if (result.success && !result.extractedMessages.empty()) {
              auto parsed = UmpParser::parseMessage(result.extractedMessages[0]);
              desc = parsed.description;
              desc.replace('\n', " | ");
          }
          rev.description = desc;

          m_fakeUmpRecording.append(rev);
          m_fakeUmpRecordedCount++;
      }
  }
  m_fakeUmpCounterLabel->setText(QString("Eventos: %1").arg(m_fakeUmpReceivedCount));
  
  if (m_isFakeUmpRecording) {
      m_fakeUmpRecordingCounterLabel->setText(QString("Eventos gravados: %1").arg(m_fakeUmpRecordedCount));
  }
}

void MainWindow::logFakeUmpMessage(const QString& message) {
  QString timeStr = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
  m_fakeUmpLog->append(QString("[%1] %2").arg(timeStr, message));
}

void MainWindow::updateFakeUmpStatus() {
  if (!m_fakeUmpBackend) return;
  if (!m_fakeUmpBackend->isOpen()) {
      m_fakeUmpStatusLabel->setText("Fechada");
      m_fakeUmpStatusLabel->setStyleSheet("color: gray; font-weight: bold;");
  } else {
      if (m_fakeUmpTimer && m_fakeUmpTimer->isActive()) {
          m_fakeUmpStatusLabel->setText("Polling ativo");
          m_fakeUmpStatusLabel->setStyleSheet("color: #00796b; font-weight: bold;");
      } else {
          m_fakeUmpStatusLabel->setText("Aberta");
          m_fakeUmpStatusLabel->setStyleSheet("color: #d84315; font-weight: bold;");
      }
  }
}

void MainWindow::addFakeUmpEventRow(const UmpRawEvent& event) {
  if (m_fakeUmpTable->rowCount() >= 1000) {
      m_fakeUmpTable->removeRow(0);
  }

  int row = m_fakeUmpTable->rowCount();
  m_fakeUmpTable->insertRow(row);

  QString timeStr = QString::number(event.timestampMs, 'f', 2) + " ms";
  
  QString wordsHex;
  for (uint32_t w : event.umpWords) {
      if (!wordsHex.isEmpty()) wordsHex += " ";
      wordsHex += QString("%1").arg(w, 8, 16, QChar('0')).toUpper();
  }

  QString bitsStr = "?";
  QString mtStr = "Unknown";
  QString groupStr = "?";
  QString statusStr = "-";
  QString channelStr = "-";
  QString sizeStr = "?";
  
  if (!event.umpWords.empty()) {
      uint32_t first = event.umpWords[0];
      uint8_t mt = (first >> 28) & 0xF;
      uint8_t group = (first >> 24) & 0xF;
      
      mtStr = QString("0x%1").arg(mt, 1, 16, QChar('0')).toUpper();
      groupStr = QString::number(group + 1);
      
      bitsStr = QString::number(event.umpWords.size() * 32);
      sizeStr = getUmpSizeLabel(mt);

      if (mt == 0x2 || mt == 0x4) {
          uint8_t statusByte = (first >> 16) & 0xFF;
          uint8_t statusNibble = statusByte & 0xF0;
          uint8_t channel = (statusByte & 0x0F) + 1;
          
          statusStr = QString("0x%1 %2").arg(statusNibble, 2, 16, QChar('0')).toUpper().arg(getUmpStatusLabel(statusNibble));
          channelStr = QString::number(channel);
      }
  }

  // Use UmpParser for description
  QString desc = "Fake UMP Event";
  auto result = UmpParser::validateAndExtractWords(wordsHex);
  if (result.success && !result.extractedMessages.empty()) {
      auto parsed = UmpParser::parseMessage(result.extractedMessages[0]);
      desc = parsed.description;
      // Truncate newlines to look clean
      desc.replace('\n', " | ");
  }

  m_fakeUmpTable->setItem(row, 0, new QTableWidgetItem(timeStr));
  m_fakeUmpTable->setItem(row, 1, new QTableWidgetItem(event.backendName));
  m_fakeUmpTable->setItem(row, 2, new QTableWidgetItem(event.portName));
  m_fakeUmpTable->setItem(row, 3, new QTableWidgetItem(wordsHex));
  m_fakeUmpTable->setItem(row, 4, new QTableWidgetItem(bitsStr));
  m_fakeUmpTable->setItem(row, 5, new QTableWidgetItem(mtStr));
  m_fakeUmpTable->setItem(row, 6, new QTableWidgetItem(groupStr));
  m_fakeUmpTable->setItem(row, 7, new QTableWidgetItem(statusStr));
  m_fakeUmpTable->setItem(row, 8, new QTableWidgetItem(channelStr));
  m_fakeUmpTable->setItem(row, 9, new QTableWidgetItem(sizeStr));
  m_fakeUmpTable->setItem(row, 10, new QTableWidgetItem(desc));
  
  m_fakeUmpTable->scrollToBottom();
}

QString MainWindow::getUmpStatusLabel(uint8_t status) const {
    switch (status) {
        case 0x80: return "Note Off";
        case 0x90: return "Note On";
        case 0xA0: return "Poly Aftertouch";
        case 0xB0: return "Control Change";
        case 0xC0: return "Program Change";
        case 0xD0: return "Channel Aftertouch";
        case 0xE0: return "Pitch Bend";
        default: return "Unknown";
    }
}

QString MainWindow::getUmpSizeLabel(uint8_t messageType) const {
    switch (messageType) {
        case 0x0:
        case 0x1:
        case 0x2: return "1 word (32-bit)";
        case 0x3:
        case 0x4: return "2 words (64-bit)";
        case 0x5:
        case 0xD:
        case 0xF: return "4 words (128-bit)";
        default: return "Unknown";
    }
}

QString MainWindow::csvEscape(const QString& value) const {
    QString escaped = value;
    if (escaped.contains(';') || escaped.contains('"') || escaped.contains('\n')) {
        escaped.replace("\"", "\"\"");
        escaped = "\"" + escaped + "\"";
    }
    return escaped;
}

QString MainWindow::csvTextCellForSpreadsheet(const QString& value) const {
    return "=\"" + value + "\"";
}

void MainWindow::exportExperimentalUmpTxtClicked() {
    if (!m_fakeUmpTable || m_fakeUmpTable->rowCount() == 0) {
        QMessageBox::information(this, "Aviso", "A tabela experimental está vazia. Não há dados para exportar.");
        return;
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString defaultName = QString("experimental_ump_export_%1.txt").arg(timestamp);
    QString defaultPath = QDir::homePath() + "/" + defaultName;

    QString fileName = QFileDialog::getSaveFileName(this, "Exportar UMP Experimental (TXT)", defaultPath, "Text Files (*.txt);;All Files (*)");
    if (fileName.isEmpty()) {
        logFakeUmpMessage("Exportação UMP experimental cancelada.");
        return;
    }

    if (!fileName.endsWith(".txt", Qt::CaseInsensitive)) {
        fileName += ".txt";
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        logFakeUmpMessage("Erro ao exportar TXT Experimental.");
        QMessageBox::warning(this, "Erro", "Falha ao criar o arquivo TXT.");
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    out << "MidiUmpAnalyzer - Experimental UMP Backend Export\n";
    out << "Version: v4.16.0\n";
    out << "Exported at: " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "\n";
    out << "Source: FakeUmpInputBackend\n";
    out << "Important note: This export uses a fake UMP backend prototype. It does not represent real MIDI 2.0 hardware capture.\n\n";
    
    out << "Total events: " << m_fakeUmpTable->rowCount() << "\n\n";
    out << "Experimental UMP Events:\n";

    for (int r = 0; r < m_fakeUmpTable->rowCount(); ++r) {
        out << "[" << m_fakeUmpTable->item(r, 0)->text() << "] "
            << "Backend: " << m_fakeUmpTable->item(r, 1)->text() << " | "
            << "Port: " << m_fakeUmpTable->item(r, 2)->text() << " | "
            << "UMP: " << m_fakeUmpTable->item(r, 3)->text() << " | "
            << "Bits: " << m_fakeUmpTable->item(r, 4)->text() << " | "
            << "MT: " << m_fakeUmpTable->item(r, 5)->text() << " | "
            << "Group: " << m_fakeUmpTable->item(r, 6)->text() << " | "
            << "Status: " << m_fakeUmpTable->item(r, 7)->text() << " | "
            << "Channel: " << m_fakeUmpTable->item(r, 8)->text() << " | "
            << "Size: " << m_fakeUmpTable->item(r, 9)->text() << " | "
            << "Description: " << m_fakeUmpTable->item(r, 10)->text() << "\n";
    }

    file.close();
    logFakeUmpMessage("Exportação UMP experimental TXT concluída.");
}

void MainWindow::exportExperimentalUmpCsvClicked() {
    if (!m_fakeUmpTable || m_fakeUmpTable->rowCount() == 0) {
        QMessageBox::information(this, "Aviso", "A tabela experimental está vazia. Não há dados para exportar.");
        return;
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString defaultName = QString("experimental_ump_export_%1.csv").arg(timestamp);
    QString defaultPath = QDir::homePath() + "/" + defaultName;

    QString fileName = QFileDialog::getSaveFileName(this, "Exportar UMP Experimental (CSV)", defaultPath, "CSV Files (*.csv);;All Files (*)");
    if (fileName.isEmpty()) {
        logFakeUmpMessage("Exportação UMP experimental cancelada.");
        return;
    }

    if (!fileName.endsWith(".csv", Qt::CaseInsensitive)) {
        fileName += ".csv";
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        logFakeUmpMessage("Erro ao exportar CSV Experimental.");
        QMessageBox::warning(this, "Erro", "Falha ao criar o arquivo CSV.");
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out.setGenerateByteOrderMark(true);

    out << "timestamp;backend;port;ump_words;bits;mt;group;status;channel;size;description\n";

    for (int r = 0; r < m_fakeUmpTable->rowCount(); ++r) {
        out << csvEscape(m_fakeUmpTable->item(r, 0)->text()) << ";"
            << csvEscape(m_fakeUmpTable->item(r, 1)->text()) << ";"
            << csvEscape(m_fakeUmpTable->item(r, 2)->text()) << ";"
            << csvTextCellForSpreadsheet(m_fakeUmpTable->item(r, 3)->text()) << ";"
            << csvEscape(m_fakeUmpTable->item(r, 4)->text()) << ";"
            << csvEscape(m_fakeUmpTable->item(r, 5)->text()) << ";"
            << csvEscape(m_fakeUmpTable->item(r, 6)->text()) << ";"
            << csvEscape(m_fakeUmpTable->item(r, 7)->text()) << ";"
            << csvEscape(m_fakeUmpTable->item(r, 8)->text()) << ";"
            << csvEscape(m_fakeUmpTable->item(r, 9)->text()) << ";"
            << csvEscape(m_fakeUmpTable->item(r, 10)->text()) << "\n";
    }

    file.close();
    logFakeUmpMessage("Exportação UMP experimental CSV concluída.");
}

void MainWindow::startFakeUmpRecordingClicked() {
    m_fakeUmpRecording.clear();
    m_fakeUmpRecordedCount = 0;
    m_isFakeUmpRecording = true;
    m_fakeUmpRecordingStatusLabel->setText("Status: Gravando");
    m_fakeUmpRecordingStatusLabel->setStyleSheet("color: red; font-weight: bold;");
    m_fakeUmpRecordingCounterLabel->setText("Eventos gravados: 0");
    logFakeUmpMessage("Gravação UMP experimental iniciada.");
}

void MainWindow::stopFakeUmpRecordingClicked() {
    m_isFakeUmpRecording = false;
    m_fakeUmpRecordingStatusLabel->setText("Status: Parada");
    m_fakeUmpRecordingStatusLabel->setStyleSheet("");
    logFakeUmpMessage("Gravação UMP experimental parada.");
}

void MainWindow::clearFakeUmpSessionClicked() {
    m_fakeUmpRecording.clear();
    m_fakeUmpRecordedCount = 0;
    m_fakeUmpRecordingStatusLabel->setText("Status: Parada");
    m_fakeUmpRecordingStatusLabel->setStyleSheet("");
    m_isFakeUmpRecording = false;
    m_fakeUmpRecordingCounterLabel->setText("Eventos gravados: 0");
    logFakeUmpMessage("Sessão UMP experimental limpa.");
}

void MainWindow::exportFakeUmpSessionTxtClicked() {
    if (m_fakeUmpRecording.isEmpty()) {
        QMessageBox::information(this, "Aviso", "Não há sessão UMP gravada para exportar.");
        return;
    }
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString defaultName = QString("fake_ump_session_%1.txt").arg(timestamp);
    QString defaultPath = QDir::homePath() + "/" + defaultName;

    QString fileName = QFileDialog::getSaveFileName(this, "Exportar Sessão UMP Fake (TXT)", defaultPath, "Text Files (*.txt);;All Files (*)");
    if (fileName.isEmpty()) {
        logFakeUmpMessage("Exportação da sessão UMP experimental cancelada.");
        return;
    }
    if (!fileName.endsWith(".txt", Qt::CaseInsensitive)) {
        fileName += ".txt";
    }
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        logFakeUmpMessage("Erro ao exportar TXT da sessão.");
        QMessageBox::warning(this, "Erro", "Falha ao criar o arquivo TXT.");
        return;
    }
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    out << "MidiUmpAnalyzer - Fake UMP Session Recording Export\n";
    out << "Version: v4.16.0\n";
    out << "Exported at: " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "\n";
    out << "Source: FakeUmpInputBackend\n";
    out << "Important note: This session uses a fake UMP backend prototype. It does not represent real MIDI 2.0 hardware capture.\n";
    out << "Recorded events: " << m_fakeUmpRecordedCount << "\n\n";

    for (const auto& ev : m_fakeUmpRecording) {
        out << "[" << ev.timestamp << "] "
            << "Backend: " << ev.backend << " | "
            << "Port: " << ev.port << " | "
            << "UMP: " << ev.umpWords << " | "
            << "Bits: " << ev.bits << " | "
            << "MT: " << ev.messageType << " | "
            << "Group: " << ev.group << " | "
            << "Status: " << ev.status << " | "
            << "Channel: " << ev.channel << " | "
            << "Size: " << ev.size << " | "
            << "Description: " << ev.description << "\n";
    }
    file.close();
    logFakeUmpMessage("Sessão UMP experimental TXT exportada com sucesso.");
}

void MainWindow::exportFakeUmpSessionCsvClicked() {
    if (m_fakeUmpRecording.isEmpty()) {
        QMessageBox::information(this, "Aviso", "Não há sessão UMP gravada para exportar.");
        return;
    }
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString defaultName = QString("fake_ump_session_%1.csv").arg(timestamp);
    QString defaultPath = QDir::homePath() + "/" + defaultName;

    QString fileName = QFileDialog::getSaveFileName(this, "Exportar Sessão UMP Fake (CSV)", defaultPath, "CSV Files (*.csv);;All Files (*)");
    if (fileName.isEmpty()) {
        logFakeUmpMessage("Exportação da sessão UMP experimental cancelada.");
        return;
    }
    if (!fileName.endsWith(".csv", Qt::CaseInsensitive)) {
        fileName += ".csv";
    }
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        logFakeUmpMessage("Erro ao exportar CSV da sessão.");
        QMessageBox::warning(this, "Erro", "Falha ao criar o arquivo CSV.");
        return;
    }
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out.setGenerateByteOrderMark(true);

    out << "timestamp;backend;port;ump_words;bits;mt;group;status;channel;size;description\n";

    for (const auto& ev : m_fakeUmpRecording) {
        out << csvEscape(ev.timestamp) << ";"
            << csvEscape(ev.backend) << ";"
            << csvEscape(ev.port) << ";"
            << csvTextCellForSpreadsheet(ev.umpWords) << ";"
            << csvEscape(ev.bits) << ";"
            << csvEscape(ev.messageType) << ";"
            << csvEscape(ev.group) << ";"
            << csvEscape(ev.status) << ";"
            << csvEscape(ev.channel) << ";"
            << csvEscape(ev.size) << ";"
            << csvEscape(ev.description) << "\n";
    }
    file.close();
    logFakeUmpMessage("Sessão UMP experimental CSV exportada com sucesso.");
}

FakeUmpSessionSummary MainWindow::buildFakeUmpSessionSummary() const {
    FakeUmpSessionSummary summary;
    summary.totalEvents = m_fakeUmpRecording.size();
    if (summary.totalEvents > 0) {
        summary.firstTimestamp = m_fakeUmpRecording.first().timestamp;
        summary.lastTimestamp = m_fakeUmpRecording.last().timestamp;
        summary.hasFirstEvent = true;
        summary.hasLastEvent = true;
        summary.firstEvent = m_fakeUmpRecording.first();
        summary.lastEvent = m_fakeUmpRecording.last();
        
        for (const auto& ev : m_fakeUmpRecording) {
            summary.byMessageType[ev.messageType]++;
            summary.byGroup[ev.group]++;
            summary.byStatus[ev.status]++;
            summary.byChannel[ev.channel]++;
            
            if (ev.channel != "-" && ev.channel != "?") {
                summary.channelMessages++;
            } else {
                summary.nonChannelMessages++;
            }
            
            if (!summary.firstExampleByMessageType.contains(ev.messageType)) {
                summary.firstExampleByMessageType[ev.messageType] = ev;
            }
        }
    }
    return summary;
}

QString MainWindow::formatFakeUmpSessionSummaryReport(const FakeUmpSessionSummary& summary) const {
    QString out;
    QTextStream stream(&out);

    stream << "MidiUmpAnalyzer - Fake UMP Session Summary Report\n";
    stream << "Version: v4.16.0\n";
    stream << "Exported at: " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "\n";
    stream << "Source: FakeUmpInputBackend\n";
    stream << "Important note: This report summarizes fake UMP backend events only.\n";
    stream << "It does not represent real MIDI 2.0 hardware capture.\n\n";

    stream << "=== Session Overview ===\n";
    stream << "Total recorded events: " << summary.totalEvents << "\n";
    stream << "First event timestamp: " << (summary.hasFirstEvent ? summary.firstTimestamp : "N/A") << "\n";
    stream << "Last event timestamp: " << (summary.hasLastEvent ? summary.lastTimestamp : "N/A") << "\n";
    stream << "Recording status: " << (m_isFakeUmpRecording ? "Active" : "Stopped") << "\n";
    stream << "Source backend: FakeUmpInputBackend\n\n";

    stream << "=== Message Type Summary ===\n";
    if (summary.byMessageType.isEmpty()) {
        stream << "No messages recorded.\n";
    } else {
        for (auto it = summary.byMessageType.constBegin(); it != summary.byMessageType.constEnd(); ++it) {
            stream << it.key() << ": " << it.value() << "\n";
        }
    }
    stream << "\n";

    stream << "=== Group Summary ===\n";
    if (summary.byGroup.isEmpty()) {
        stream << "No messages recorded.\n";
    } else {
        for (auto it = summary.byGroup.constBegin(); it != summary.byGroup.constEnd(); ++it) {
            stream << "Group " << it.key() << ": " << it.value() << "\n";
        }
    }
    stream << "\n";

    stream << "=== Status Summary ===\n";
    if (summary.byStatus.isEmpty()) {
        stream << "No messages recorded.\n";
    } else {
        for (auto it = summary.byStatus.constBegin(); it != summary.byStatus.constEnd(); ++it) {
            stream << it.key() << ": " << it.value() << "\n";
        }
    }
    stream << "\n";

    stream << "=== Channel Summary ===\n";
    if (summary.byChannel.isEmpty()) {
        stream << "No messages recorded.\n";
    } else {
        for (auto it = summary.byChannel.constBegin(); it != summary.byChannel.constEnd(); ++it) {
            if (it.key() != "-" && it.key() != "?") {
                stream << "Channel " << it.key() << ": " << it.value() << "\n";
            }
        }
        stream << "Non-channel / N/A: " << summary.nonChannelMessages << "\n";
    }
    stream << "\n";

    stream << "=== Representative Examples ===\n";
    if (summary.firstExampleByMessageType.isEmpty()) {
        stream << "No examples available.\n";
    } else {
        for (auto it = summary.firstExampleByMessageType.constBegin(); it != summary.firstExampleByMessageType.constEnd(); ++it) {
            stream << "First " << it.key() << " Event:\n";
            stream << "  Timestamp: " << it.value().timestamp << "\n";
            stream << "  Backend: " << it.value().backend << "\n";
            stream << "  Port: " << it.value().port << "\n";
            stream << "  UMP Words: " << it.value().umpWords << "\n";
            stream << "  MT: " << it.value().messageType << "\n";
            stream << "  Group: " << it.value().group << "\n";
            stream << "  Status: " << it.value().status << "\n";
            stream << "  Channel: " << it.value().channel << "\n";
            stream << "  Description: " << it.value().description << "\n\n";
        }
    }

    stream << "=== Technical Notes ===\n";
    stream << "- This report is generated from m_fakeUmpRecording.\n";
    stream << "- The Experimental UMP table may be cleared without clearing the recorded session.\n";
    stream << "- FakeUmpInputBackend generates artificial UMP words for testing the UI and parser pipeline.\n";
    stream << "- This report does not prove operating system UMP capture.\n";
    stream << "- Windows MIDI Services, ALSA UMP, libremidi and JUCE are not implemented in this version.\n";
    stream << "- MIDI-CI and Property Exchange are not implemented.\n";

    return out;
}

void MainWindow::exportFakeUmpSessionSummaryClicked() {
    if (m_fakeUmpRecording.isEmpty()) {
        QMessageBox::information(this, "Aviso", "Não há sessão UMP gravada para exportar.");
        logFakeUmpMessage("Resumo da sessão UMP experimental não exportado: sessão vazia.");
        return;
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString defaultName = QString("fake_ump_session_summary_%1.txt").arg(timestamp);
    QString defaultPath = QDir::homePath() + "/" + defaultName;

    QString fileName = QFileDialog::getSaveFileName(this, "Exportar Resumo da Sessão UMP (TXT)", defaultPath, "Text Files (*.txt);;All Files (*)");
    if (fileName.isEmpty()) {
        logFakeUmpMessage("Exportação do resumo da sessão UMP experimental cancelada.");
        return;
    }

    if (!fileName.endsWith(".txt", Qt::CaseInsensitive)) {
        fileName += ".txt";
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        logFakeUmpMessage("Erro ao exportar resumo da sessão UMP experimental.");
        QMessageBox::warning(this, "Erro", "Falha ao criar o arquivo de resumo TXT.");
        return;
    }

    FakeUmpSessionSummary summary = buildFakeUmpSessionSummary();
    QString reportContent = formatFakeUmpSessionSummaryReport(summary);

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << reportContent;
    file.close();

    logFakeUmpMessage("Resumo da sessão UMP experimental exportado com sucesso.");
}
