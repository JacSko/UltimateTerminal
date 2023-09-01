#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QtWidgets/QShortcut>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QScrollBar>

#include "Settings.h"
#include "Logger.h"

QT_BEGIN_NAMESPACE

#define APPLICATION_THEMES       \
      APPLICATION_THEME(DEFAULT) \
      APPLICATION_THEME(DARK)    \

#undef APPLICATION_THEME
#define APPLICATION_THEME(theme_name) theme_name,
enum class Theme
{
   APPLICATION_THEMES
   APPLICATION_THEMES_MAX
};
#undef APPLICATION_THEME

constexpr uint32_t NUMBER_OF_PORTS = 5;

class Ui_MainWindow
{
public:

   struct TraceFilterItem
   {
      QLineEdit* line_edit;
      QPushButton* button;
   };
   struct PortItem
   {
      QLabel* throughputLabel;
      QLabel* label;
      QPushButton* button;
   };

   /* main window top layout */
   QWidget* centralWidget;
   QVBoxLayout* mainLayout;
   QSplitter* mainWindowSplitter;

   /* control buttons section */
   QWidget* controlButtonsWidget;
   QGridLayout* controlButtonsLayout;
   QPushButton* markerButton;
   QPushButton* loggingButton;
   QPushButton* settingsButton;
   QLabel* infoLabel;

   /* user buttons section */
   QTabWidget* buttonsTabWidget;

   /* terminal section */
   QWidget* terminalWidget;
   QGridLayout* terminalLayout;
   QTextEdit* terminalView;
   QComboBox* portComboBox;
   QPushButton* sendButton;
   QPushButton* terminalBottomButton;
   QComboBox* lineEndingComboBox;
   QComboBox* textEdit;
   QPushButton* clearButton;

   /* trace section */
   QWidget* traceWidget;
   QVBoxLayout* traceWidgetLayout;
   QSplitter* traceViewAndFiltersSplitter;

   /* trace view panel (left side) */
   QWidget* traceViewWidget;
   QGridLayout* traceViewLayout;
   QPushButton* traceClearButton;
   QPushButton* traceBottomButton;
   QListWidget* traceView;

   /* trace filter panel (right side) */
   QWidget* traceFiltersWidget;
   QGridLayout* traceFiltersLayout;
   QLineEdit* traceFilter_0;
   QLineEdit* traceFilter_1;
   QLineEdit* traceFilter_2;
   QLineEdit* traceFilter_3;
   QLineEdit* traceFilter_4;
   QLineEdit* traceFilter_5;
   QLineEdit* traceFilter_6;
   QPushButton* traceFilterButton_0;
   QPushButton* traceFilterButton_1;
   QPushButton* traceFilterButton_2;
   QPushButton* traceFilterButton_3;
   QPushButton* traceFilterButton_4;
   QPushButton* traceFilterButton_5;
   QPushButton* traceFilterButton_6;

   /* port section */
   QWidget* portsWidget;
   QGridLayout* portsWidgetLayout;
   QPushButton* portButton_0;
   QPushButton* portButton_1;
   QPushButton* portButton_2;
   QPushButton* portButton_3;
   QPushButton* portButton_4;
   QLabel* portLabel_0;
   QLabel* portLabel_1;
   QLabel* portLabel_2;
   QLabel* portLabel_3;
   QLabel* portLabel_4;
   QLabel* portThroughput_0;
   QLabel* portThroughput_1;
   QLabel* portThroughput_2;
   QLabel* portThroughput_3;
   QLabel* portThroughput_4;

   /* bars section */
   QStatusBar* statusBar;
   QMainWindow* mainWindow;

   /* key shortcuts */
   QShortcut* loggingButtonShortcut;
   QShortcut* markerButtonShortcut;
   QShortcut* port0ButtonShortcut;
   QShortcut* port1ButtonShortcut;
   QShortcut* port2ButtonShortcut;
   QShortcut* port3ButtonShortcut;
   QShortcut* port4ButtonShortcut;
   QShortcut* clearButtonShortcut;
   QShortcut* traceClearButtonShortcut;
   QShortcut* switchSendPortShortcut;

   Theme theme;

   void setupMainApplicationLayout()
   {
      centralWidget = new QWidget(mainWindow);
      centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
      QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
      sizePolicy.setHorizontalStretch(0);
      sizePolicy.setVerticalStretch(0);
      sizePolicy.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
      centralWidget->setSizePolicy(sizePolicy);
      mainLayout = new QVBoxLayout(centralWidget);
      mainWindowSplitter = new QSplitter(centralWidget);
      controlButtonsWidget = new QWidget(centralWidget);
      terminalWidget = new QWidget(mainWindowSplitter);
      traceWidget = new QWidget(mainWindowSplitter);
      portsWidget = new QWidget(mainWindowSplitter);
      buttonsTabWidget = new QTabWidget(mainWindowSplitter);
      centralWidget->setLayout(mainLayout);

      mainWindowSplitter->setOrientation(Qt::Vertical);
      mainWindowSplitter->addWidget(buttonsTabWidget);
      mainWindowSplitter->addWidget(terminalWidget);
      mainWindowSplitter->addWidget(traceWidget);
      mainWindowSplitter->addWidget(portsWidget);
      mainLayout->addWidget(controlButtonsWidget);
      mainLayout->addWidget(mainWindowSplitter);
      mainWindow->setCentralWidget(centralWidget);
   }

   void setupControlButtonsWidget()
   {
      controlButtonsLayout = new QGridLayout();
      controlButtonsLayout->setContentsMargins(0,0,0,0);
      controlButtonsLayout->setObjectName(QString::fromUtf8("controlButtonsLayout"));
      markerButton = new QPushButton(centralWidget);
      markerButton->setObjectName(QString::fromUtf8("markerButton"));
      controlButtonsLayout->addWidget(markerButton, 0, 0, 1, 1);
      loggingButton = new QPushButton(centralWidget);
      loggingButton->setObjectName(QString::fromUtf8("loggingButton"));
      controlButtonsLayout->addWidget(loggingButton, 0, 1, 1, 1);
      settingsButton = new QPushButton(centralWidget);
      settingsButton->setObjectName(QString::fromUtf8("settingsButton"));
      controlButtonsLayout->addWidget(settingsButton, 0, 2, 1, 1);
      infoLabel = new QLabel(centralWidget);
      infoLabel->setObjectName(QString::fromUtf8("infoLabel"));
      infoLabel->setAlignment(Qt::AlignCenter);
      controlButtonsLayout->addWidget(infoLabel, 0, 4, 1, 1);
      controlButtonsWidget->setLayout(controlButtonsLayout);

   }

   void setupUserButtonsWidget()
   {
      QSizePolicy policy = buttonsTabWidget->sizePolicy();
      policy.setVerticalPolicy(QSizePolicy::Minimum);
      buttonsTabWidget->setSizePolicy(policy);
      uint8_t tabs_count = SETTING_GET_U32(GUI_UserButtons_Tabs);
      for (uint8_t i = 0; i < tabs_count; i++)
      {
         UT_Log(GUI_CONTROLLER, LOW, "Creating user button tab %u", i);
         createUserButtonTab(i);
      }
   }

   void setupTerminalWidget()
   {
      terminalLayout = new QGridLayout(terminalWidget);
      terminalLayout->setContentsMargins(0,0,0,0);
      terminalLayout->setObjectName(QString::fromUtf8("terminalLayout"));
      clearButton = new QPushButton(terminalWidget);
      clearButton->setObjectName(QString::fromUtf8("clearButton"));
      terminalBottomButton = new QPushButton(terminalWidget);
      terminalBottomButton->setObjectName(QString::fromUtf8("bottomButton"));
      portComboBox = new QComboBox(terminalWidget);
      portComboBox->setObjectName(QString::fromUtf8("portComboBox"));
      QSizePolicy sizePolicy1(QSizePolicy::Maximum, QSizePolicy::Fixed);
      sizePolicy1.setHorizontalStretch(0);
      sizePolicy1.setVerticalStretch(0);
      sizePolicy1.setHeightForWidth(portComboBox->sizePolicy().hasHeightForWidth());
      portComboBox->setSizePolicy(sizePolicy1);
      textEdit = new QComboBox(terminalWidget);
      textEdit->setObjectName(QString::fromUtf8("textEdit"));
      QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Fixed);
      sizePolicy3.setHorizontalStretch(0);
      sizePolicy3.setVerticalStretch(0);
      sizePolicy3.setHeightForWidth(textEdit->sizePolicy().hasHeightForWidth());
      textEdit->setSizePolicy(sizePolicy3);
      textEdit->setEditable(true);
      textEdit->setInsertPolicy(QComboBox::NoInsert);
      lineEndingComboBox = new QComboBox(terminalWidget);
      lineEndingComboBox->setObjectName(QString::fromUtf8("lineEndingComboBox"));
      sizePolicy1.setHeightForWidth(lineEndingComboBox->sizePolicy().hasHeightForWidth());
      lineEndingComboBox->setSizePolicy(sizePolicy1);
      sendButton = new QPushButton(terminalWidget);
      sendButton->setObjectName(QString::fromUtf8("sendButton"));
      QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
      sizePolicy2.setHorizontalStretch(0);
      sizePolicy2.setVerticalStretch(0);
      sizePolicy2.setHeightForWidth(sendButton->sizePolicy().hasHeightForWidth());
      sendButton->setSizePolicy(sizePolicy2);
      terminalView = new QTextEdit(terminalWidget);
      terminalView->setObjectName(QString::fromUtf8("terminalView"));
      terminalView->setReadOnly(true);
      terminalView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      terminalView->setWordWrapMode(QTextOption::WrapAnywhere);

      terminalLayout->addWidget(clearButton, 0, 0, 1, 3);
      terminalLayout->addWidget(terminalBottomButton, 0, 3, 1, 2);
      terminalLayout->addWidget(portComboBox, 2, 0, 1, 1);
      terminalLayout->addWidget(textEdit, 2, 1, 1, 2);
      terminalLayout->addWidget(lineEndingComboBox, 2, 3, 1, 1);
      terminalLayout->addWidget(sendButton, 2, 4, 1, 1);
      terminalLayout->addWidget(terminalView, 1, 0, 1, 5);
      terminalWidget->setLayout(terminalLayout);
   }

   void setupTraceWidget()
   {
      traceWidgetLayout = new QVBoxLayout(traceWidget);
      traceWidgetLayout->setContentsMargins(0, 0, 0, 0);
      traceWidget->setLayout(traceWidgetLayout);
      traceViewAndFiltersSplitter = new QSplitter(traceWidget);
      traceViewWidget = new QWidget(traceViewAndFiltersSplitter);
      traceFiltersWidget = new QWidget(traceViewAndFiltersSplitter);
      traceViewAndFiltersSplitter->addWidget(traceViewWidget);
      traceViewAndFiltersSplitter->addWidget(traceFiltersWidget);
      traceViewLayout = new QGridLayout(traceViewWidget);
      traceViewLayout->setContentsMargins(0,0,0,0);
      traceClearButton = new QPushButton(traceViewWidget);
      traceClearButton->setObjectName(QString::fromUtf8("traceClearButton"));
      traceViewLayout->addWidget(traceClearButton, 0, 0, 1, 1);
      traceBottomButton = new QPushButton(traceViewWidget);
      traceBottomButton->setObjectName(QString::fromUtf8("traceBottomButton"));
      traceViewLayout->addWidget(traceBottomButton, 0, 1, 1, 1);
      traceView = new QListWidget(traceViewWidget);
      traceView->setObjectName(QString::fromUtf8("traceView"));
      traceView->setUniformItemSizes(true);
      traceView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      traceView->setWordWrap(true);
      QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Expanding);
      sizePolicy4.setHorizontalStretch(0);
      sizePolicy4.setVerticalStretch(0);
      sizePolicy4.setHeightForWidth(traceView->sizePolicy().hasHeightForWidth());
      traceView->setSizePolicy(sizePolicy4);
      traceViewLayout->addWidget(traceView, 1, 0, 1, 2);
      traceViewWidget->setLayout(traceViewLayout);
      traceFiltersLayout = new QGridLayout(traceFiltersWidget);
      traceFiltersLayout->setContentsMargins(0,0,0,0);
      traceFilter_0 = new QLineEdit(traceFiltersWidget);
      traceFilter_0->setObjectName(QString::fromUtf8("traceFilter_0"));
      traceFiltersLayout->addWidget(traceFilter_0, 0, 0, 1, 1);
      traceFilter_1 = new QLineEdit(traceFiltersWidget);
      traceFilter_1->setObjectName(QString::fromUtf8("traceFilter_1"));
      traceFiltersLayout->addWidget(traceFilter_1, 1, 0, 1, 1);
      traceFilter_2 = new QLineEdit(traceFiltersWidget);
      traceFilter_2->setObjectName(QString::fromUtf8("traceFilter_2"));
      traceFiltersLayout->addWidget(traceFilter_2, 2, 0, 1, 1);
      traceFilter_3 = new QLineEdit(traceFiltersWidget);
      traceFilter_3->setObjectName(QString::fromUtf8("traceFilter_3"));
      traceFiltersLayout->addWidget(traceFilter_3, 3, 0, 1, 1);
      traceFilter_4 = new QLineEdit(traceFiltersWidget);
      traceFilter_4->setObjectName(QString::fromUtf8("traceFilter_4"));
      traceFiltersLayout->addWidget(traceFilter_4, 4, 0, 1, 1);
      traceFilter_5 = new QLineEdit(traceFiltersWidget);
      traceFilter_5->setObjectName(QString::fromUtf8("traceFilter_5"));
      traceFiltersLayout->addWidget(traceFilter_5, 5, 0, 1, 1);
      traceFilter_6 = new QLineEdit(traceFiltersWidget);
      traceFilter_6->setObjectName(QString::fromUtf8("traceFilter_6"));
      traceFiltersLayout->addWidget(traceFilter_6, 6, 0, 1, 1);
      traceFilterButton_0 = new QPushButton(traceFiltersWidget);
      traceFilterButton_0->setObjectName(QString::fromUtf8("traceFilterButton_0"));
      traceFiltersLayout->addWidget(traceFilterButton_0, 0, 1, 1, 1);
      traceFilterButton_1 = new QPushButton(traceFiltersWidget);
      traceFilterButton_1->setObjectName(QString::fromUtf8("traceFilterButton_1"));
      traceFiltersLayout->addWidget(traceFilterButton_1, 1, 1, 1, 1);
      traceFilterButton_2 = new QPushButton(traceFiltersWidget);
      traceFilterButton_2->setObjectName(QString::fromUtf8("traceFilterButton_2"));
      traceFiltersLayout->addWidget(traceFilterButton_2, 2, 1, 1, 1);
      traceFilterButton_3 = new QPushButton(traceFiltersWidget);
      traceFilterButton_3->setObjectName(QString::fromUtf8("traceFilterButton_3"));
      traceFiltersLayout->addWidget(traceFilterButton_3, 3, 1, 1, 1);
      traceFilterButton_4 = new QPushButton(traceFiltersWidget);
      traceFilterButton_4->setObjectName(QString::fromUtf8("traceFilterButton_4"));
      traceFiltersLayout->addWidget(traceFilterButton_4, 4, 1, 1, 1);
      traceFilterButton_5 = new QPushButton(traceFiltersWidget);
      traceFilterButton_5->setObjectName(QString::fromUtf8("traceFilterButton_5"));
      traceFiltersLayout->addWidget(traceFilterButton_5, 5, 1, 1, 1);
      traceFilterButton_6 = new QPushButton(traceFiltersWidget);
      traceFilterButton_6->setObjectName(QString::fromUtf8("traceFilterButton_6"));
      traceFiltersLayout->addWidget(traceFilterButton_6, 6, 1, 1, 1);
      traceFiltersWidget->setLayout(traceFiltersLayout);
      traceWidgetLayout->addWidget(traceViewAndFiltersSplitter);
   }

   void setupPortsWidget()
   {
      portsWidgetLayout = new QGridLayout(portsWidget);
      portsWidgetLayout->setContentsMargins(0,0,0,0);
      portButton_0 = new QPushButton(portsWidget);
      portButton_0->setObjectName(QString::fromUtf8("portButton_0"));
      portsWidgetLayout->addWidget(portButton_0, 0, 0, 1, 1);
      portButton_1 = new QPushButton(portsWidget);
      portButton_1->setObjectName(QString::fromUtf8("portButton_1"));
      portsWidgetLayout->addWidget(portButton_1, 0, 1, 1, 1);
      portButton_2 = new QPushButton(portsWidget);
      portButton_2->setObjectName(QString::fromUtf8("portButton_2"));
      portsWidgetLayout->addWidget(portButton_2, 0, 2, 1, 1);
      portButton_3 = new QPushButton(portsWidget);
      portButton_3->setObjectName(QString::fromUtf8("portButton_3"));
      portsWidgetLayout->addWidget(portButton_3, 0, 3, 1, 1);
      portButton_4 = new QPushButton(portsWidget);
      portButton_4->setObjectName(QString::fromUtf8("portButton_4"));
      portsWidgetLayout->addWidget(portButton_4, 0, 4, 1, 1);
      portLabel_0 = new QLabel(portsWidget);
      portLabel_0->setObjectName(QString::fromUtf8("portLabel_0"));
      QSizePolicy sizePolicy5(QSizePolicy::Preferred, QSizePolicy::Maximum);
      sizePolicy5.setHorizontalStretch(0);
      sizePolicy5.setVerticalStretch(0);
      sizePolicy5.setHeightForWidth(portLabel_0->sizePolicy().hasHeightForWidth());
      portLabel_0->setSizePolicy(sizePolicy5);
      portsWidgetLayout->addWidget(portLabel_0, 1, 0, 1, 1);
      portLabel_1 = new QLabel(portsWidget);
      portLabel_1->setObjectName(QString::fromUtf8("portLabel_1"));
      sizePolicy5.setHeightForWidth(portLabel_1->sizePolicy().hasHeightForWidth());
      portLabel_1->setSizePolicy(sizePolicy5);
      portsWidgetLayout->addWidget(portLabel_1, 1, 1, 1, 1);
      portLabel_2 = new QLabel(portsWidget);
      portLabel_2->setObjectName(QString::fromUtf8("portLabel_2"));
      sizePolicy5.setHeightForWidth(portLabel_2->sizePolicy().hasHeightForWidth());
      portLabel_2->setSizePolicy(sizePolicy5);
      portsWidgetLayout->addWidget(portLabel_2, 1, 2, 1, 1);
      portLabel_3 = new QLabel(portsWidget);
      portLabel_3->setObjectName(QString::fromUtf8("portLabel_3"));
      sizePolicy5.setHeightForWidth(portLabel_3->sizePolicy().hasHeightForWidth());
      portLabel_3->setSizePolicy(sizePolicy5);
      portsWidgetLayout->addWidget(portLabel_3, 1, 3, 1, 1);
      portLabel_4 = new QLabel(portsWidget);
      portLabel_4->setObjectName(QString::fromUtf8("portLabel_4"));
      sizePolicy5.setHeightForWidth(portLabel_4->sizePolicy().hasHeightForWidth());
      portLabel_4->setSizePolicy(sizePolicy5);
      portsWidgetLayout->addWidget(portLabel_4, 1, 4, 1, 1);
      portThroughput_0 = new QLabel(portsWidget);
      portThroughput_0->setObjectName(QString::fromUtf8("portThroughput_0"));
      sizePolicy5.setHeightForWidth(portThroughput_0->sizePolicy().hasHeightForWidth());
      portThroughput_0->setSizePolicy(sizePolicy5);
      portsWidgetLayout->addWidget(portThroughput_0, 2, 0, 1, 1);
      portThroughput_1 = new QLabel(portsWidget);
      portThroughput_1->setObjectName(QString::fromUtf8("portThroughput_1"));
      sizePolicy5.setHeightForWidth(portThroughput_1->sizePolicy().hasHeightForWidth());
      portThroughput_1->setSizePolicy(sizePolicy5);
      portsWidgetLayout->addWidget(portThroughput_1, 2, 1, 1, 1);
      portThroughput_2 = new QLabel(portsWidget);
      portThroughput_2->setObjectName(QString::fromUtf8("portThroughput_2"));
      sizePolicy5.setHeightForWidth(portThroughput_2->sizePolicy().hasHeightForWidth());
      portThroughput_2->setSizePolicy(sizePolicy5);
      portsWidgetLayout->addWidget(portThroughput_2, 2, 2, 1, 1);
      portThroughput_3 = new QLabel(portsWidget);
      portThroughput_3->setObjectName(QString::fromUtf8("portThroughput_3"));
      sizePolicy5.setHeightForWidth(portThroughput_3->sizePolicy().hasHeightForWidth());
      portThroughput_3->setSizePolicy(sizePolicy5);
      portsWidgetLayout->addWidget(portThroughput_3, 2, 3, 1, 1);
      portThroughput_4 = new QLabel(portsWidget);
      portThroughput_4->setObjectName(QString::fromUtf8("portThroughput_4"));
      sizePolicy5.setHeightForWidth(portThroughput_4->sizePolicy().hasHeightForWidth());
      portThroughput_4->setSizePolicy(sizePolicy5);
      portsWidgetLayout->addWidget(portThroughput_4, 2, 4, 1, 1);
      portsWidget->setLayout(portsWidgetLayout);
   }

   void setupKeyboardShortcuts()
   {
      loggingButtonShortcut = new QShortcut(Qt::Key_F2, mainWindow);
      markerButtonShortcut = new QShortcut(Qt::CTRL | Qt::Key_M, mainWindow);
      port0ButtonShortcut = new QShortcut(Qt::CTRL | Qt::Key_1, mainWindow);
      port1ButtonShortcut = new QShortcut(Qt::CTRL | Qt::Key_2, mainWindow);
      port2ButtonShortcut = new QShortcut(Qt::CTRL | Qt::Key_3, mainWindow);
      port3ButtonShortcut = new QShortcut(Qt::CTRL | Qt::Key_4, mainWindow);
      port4ButtonShortcut = new QShortcut(Qt::CTRL | Qt::Key_5, mainWindow);
      clearButtonShortcut = new QShortcut(Qt::Key_F3, mainWindow);
      traceClearButtonShortcut = new QShortcut(Qt::Key_F4, mainWindow);
      switchSendPortShortcut = new QShortcut(Qt::Key_F1, mainWindow);
   }

   void setupUi(QMainWindow *MainWindow)
   {
      if (MainWindow->objectName().isEmpty())
         MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
      MainWindow->resize(872, 690);
      mainWindow = MainWindow;

      setupMainApplicationLayout();
      setupControlButtonsWidget();
      setupUserButtonsWidget();
      setupTerminalWidget();
      setupTraceWidget();
      setupPortsWidget();
      statusBar = new QStatusBar(MainWindow);
      statusBar->setObjectName(QString::fromUtf8("statusbar"));
      MainWindow->setStatusBar(statusBar);

      retranslateUi(MainWindow);
      QMetaObject::connectSlotsByName(MainWindow);

      setupKeyboardShortcuts();

      buttons.push_back(markerButton);
      buttons.push_back(loggingButton);
      buttons.push_back(settingsButton);
      buttons.push_back(sendButton);
      buttons.push_back(terminalBottomButton);
      buttons.push_back(clearButton);
      buttons.push_back(traceClearButton);
      buttons.push_back(traceBottomButton);
      buttons.push_back(traceFilterButton_0);
      buttons.push_back(traceFilterButton_1);
      buttons.push_back(traceFilterButton_2);
      buttons.push_back(traceFilterButton_3);
      buttons.push_back(traceFilterButton_4);
      buttons.push_back(traceFilterButton_5);
      buttons.push_back(traceFilterButton_6);
      buttons.push_back(portButton_0);
      buttons.push_back(portButton_1);
      buttons.push_back(portButton_2);
      buttons.push_back(portButton_3);
      buttons.push_back(portButton_4);

      trace_filters.push_back({traceFilter_0, traceFilterButton_0});
      trace_filters.push_back({traceFilter_1, traceFilterButton_1});
      trace_filters.push_back({traceFilter_2, traceFilterButton_2});
      trace_filters.push_back({traceFilter_3, traceFilterButton_3});
      trace_filters.push_back({traceFilter_4, traceFilterButton_4});
      trace_filters.push_back({traceFilter_5, traceFilterButton_5});
      trace_filters.push_back({traceFilter_6, traceFilterButton_6});

      ports.push_back({portThroughput_0, portLabel_0, portButton_0});
      ports.push_back({portThroughput_1, portLabel_1, portButton_1});
      ports.push_back({portThroughput_2, portLabel_2, portButton_2});
      ports.push_back({portThroughput_3, portLabel_3, portButton_3});
      ports.push_back({portThroughput_4, portLabel_4, portButton_4});

   }

   void retranslateUi(QMainWindow *MainWindow)
   {
      MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
      infoLabel->setText(QString());
      markerButton->setText(QCoreApplication::translate("MainWindow", "MARKER", nullptr));
      loggingButton->setText(QCoreApplication::translate("MainWindow", "LOG", nullptr));
      settingsButton->setText(QCoreApplication::translate("MainWindow", "SETTINGS", nullptr));
      sendButton->setText(QCoreApplication::translate("MainWindow", "SEND", nullptr));
      terminalBottomButton->setText(QCoreApplication::translate("MainWindow", "BOTTOM", nullptr));
      clearButton->setText(QCoreApplication::translate("MainWindow", "CLEAR", nullptr));
      traceClearButton->setText(QCoreApplication::translate("MainWindow", "CLEAR", nullptr));
      traceBottomButton->setText(QCoreApplication::translate("MainWindow", "BOTTOM", nullptr));
      traceFilterButton_0->setText(QCoreApplication::translate("MainWindow", "ACTIVE", nullptr));
      traceFilterButton_1->setText(QCoreApplication::translate("MainWindow", "ACTIVE", nullptr));
      traceFilterButton_2->setText(QCoreApplication::translate("MainWindow", "ACTIVE", nullptr));
      traceFilterButton_3->setText(QCoreApplication::translate("MainWindow", "ACTIVE", nullptr));
      traceFilterButton_4->setText(QCoreApplication::translate("MainWindow", "ACTIVE", nullptr));
      traceFilterButton_5->setText(QCoreApplication::translate("MainWindow", "ACTIVE", nullptr));
      traceFilterButton_6->setText(QCoreApplication::translate("MainWindow", "ACTIVE", nullptr));
      portButton_0->setText(QCoreApplication::translate("MainWindow", "PORT0", nullptr));
      portButton_1->setText(QCoreApplication::translate("MainWindow", "PORT1", nullptr));
      portButton_2->setText(QCoreApplication::translate("MainWindow", "PORT2", nullptr));
      portButton_3->setText(QCoreApplication::translate("MainWindow", "PORT3", nullptr));
      portButton_4->setText(QCoreApplication::translate("MainWindow", "PORT4", nullptr));
      portLabel_0->setText(QString());
      portLabel_1->setText(QString());
      portLabel_2->setText(QString());
      portLabel_3->setText(QString());
      portLabel_4->setText(QString());
      portThroughput_0->setText(QString());
      portThroughput_1->setText(QString());
      portThroughput_2->setText(QString());
      portThroughput_3->setText(QString());
      portThroughput_4->setText(QString());

   }

   void loadTheme(Theme new_theme)
   {
      theme = new_theme;
      if(new_theme == Theme::DEFAULT)
      {
         renderDefaultTheme();
      }
      else if(new_theme == Theme::DARK)
      {
         renderDarkTheme();
      }
   }
   const std::vector<QPushButton*>& getButtons()
   {
      return buttons;
   }
   const std::vector<TraceFilterItem>& getTraceFilters()
   {
      return trace_filters;
   }
   const std::vector<PortItem>& getPorts()
   {
      return ports;
   }
   uint32_t countUserButtons()
   {
      return SETTING_GET_U32(GUI_UserButtons_Tabs) * SETTING_GET_U32(GUI_UserButtons_RowsPerTab) * SETTING_GET_U32(GUI_UserButtons_ButtonsPerRow);
   }
   uint32_t countPorts()
   {
      return NUMBER_OF_PORTS;
   }
   uint32_t countTraceFilters()
   {
      return trace_filters.size();
   }
   uint32_t countUserButtonsTabs()
   {
      return SETTING_GET_U32(GUI_UserButtons_Tabs);
   }
   uint32_t countUserButtonsPerTab()
   {
      return SETTING_GET_U32(GUI_UserButtons_RowsPerTab) * SETTING_GET_U32(GUI_UserButtons_ButtonsPerRow);
   }
private:

   void renderDarkTheme()
   {
      const QColor BACKGROUND_COLOR = SETTING_GET_U32(GUI_Dark_WindowBackground);
      const QColor TERMINAL_BACKGROUND_COLOR = SETTING_GET_U32(GUI_Dark_TerminalBackground);
      const QColor TEXT_COLOR = SETTING_GET_U32(GUI_Dark_WindowText);

      QPalette combobox_palette {};
      combobox_palette.setColor(QPalette::Window, BACKGROUND_COLOR);
      combobox_palette.setColor(QPalette::WindowText, TEXT_COLOR);
      combobox_palette.setColor(QPalette::Text, TEXT_COLOR);
      portComboBox->setPalette(combobox_palette);
      lineEndingComboBox->setPalette(combobox_palette);
      portComboBox->view()->setPalette(combobox_palette);
      lineEndingComboBox->view()->setPalette(combobox_palette);
      textEdit->setPalette(combobox_palette);
      textEdit->view()->setPalette(combobox_palette);

      QPalette terminal_palette = terminalView->palette();
      terminal_palette.setColor(QPalette::Base, TERMINAL_BACKGROUND_COLOR);
      terminal_palette.setColor(QPalette::Text, Qt::black);
      terminalView->setPalette(terminal_palette);
      traceView->setPalette(terminal_palette);

      QPalette main_palette = mainWindow->palette();
      main_palette.setColor(QPalette::Base, TERMINAL_BACKGROUND_COLOR);
      main_palette.setColor(QPalette::Window, BACKGROUND_COLOR);
      main_palette.setColor(QPalette::WindowText, TEXT_COLOR);
      main_palette.setColor(QPalette::Text, TEXT_COLOR);
      main_palette.setColor(QPalette::Button, BACKGROUND_COLOR);
      main_palette.setColor(QPalette::ButtonText, TEXT_COLOR);
      mainWindow->setPalette(main_palette);
      terminalView->setPalette(main_palette);
   }
   void renderDefaultTheme()
   {
      portComboBox->setPalette({});
      portComboBox->view()->setPalette({});
      lineEndingComboBox->setPalette({});
      lineEndingComboBox->view()->setPalette({});
      textEdit->setPalette({});
      textEdit->view()->setPalette({});
      terminalView->setPalette({});
      traceView->setPalette({});
      mainWindow->setPalette({});
      terminalView->setPalette({});
   }
   void createUserButtonTab(uint8_t index)
   {
      QWidget* tab_widget = new QWidget();
      QGridLayout* widget_layout = new QGridLayout();
      uint8_t buttons_rows = SETTING_GET_U32(GUI_UserButtons_RowsPerTab);
      uint8_t buttons_per_row = SETTING_GET_U32(GUI_UserButtons_ButtonsPerRow);
      UT_Log(GUI_CONTROLLER, LOW, "Creating tab with %u rows, each with %u buttons", buttons_rows, buttons_per_row);
      uint8_t current_row = 0;

      for (uint8_t i = 0; i < buttons_rows; i++)
      {
         for (uint8_t j = 0; j < buttons_per_row; j++)
         {
            QPushButton* button = new QPushButton(buttonsTabWidget);
            buttons.push_back(button);
            /* button name in format BUTTON<tab>:<row>:<idx>*/
            std::string button_name = "BUTTON" + std::to_string(((index * buttons_rows * buttons_per_row) + (i * buttons_per_row) + j));
            UT_Log(GUI_CONTROLLER, LOW, "button with name %s created", button_name.c_str());
            button->setObjectName(QString(button_name.c_str()));
            widget_layout->addWidget(button, current_row, j, 1, 1);
         }
         current_row ++;
      }
      tab_widget->setLayout(widget_layout);
      std::string tab_name = "BUTTONS" + std::to_string(index);
      buttonsTabWidget->addTab(tab_widget, QString(tab_name.c_str()));
      UT_Log(GUI_CONTROLLER, LOW, "tab %s created", tab_name.c_str());
   }

   std::vector<QPushButton*> buttons;
   std::vector<TraceFilterItem> trace_filters;
   std::vector<PortItem> ports;
};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
