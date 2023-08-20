#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
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
      QLabel* label;
      QPushButton* button;
   };

   QWidget *centralwidget;

   /* control buttons at the top of application */
   QGridLayout *controlButtonsLayout;
   QPushButton *markerButton;
   QPushButton *loggingButton;
   QPushButton *settingsButton;
   QLabel *infoLabel;

   /* layout with user buttons */
   QTabWidget* buttonsTabWidget;

   /* terminal layout */
   QGridLayout *terminalLayout;
   QSplitter *splitter_2;
   QWidget *gridLayoutWidget_2;
   QTextEdit *terminalView;
   QComboBox *portComboBox;
   QPushButton *sendButton;
   QPushButton *terminalBottomButton;
   QComboBox *lineEndingComboBox;
   QComboBox *textEdit;
   QPushButton *clearButton;

   /* trace filter layout */
   QWidget *gridLayoutWidget_3;
   QGridLayout *traceFilterLayout;
   QSplitter *splitter;
   QWidget *layoutWidget;
   QVBoxLayout *verticalLayout_2;
   QWidget *gridLayoutWidget_5;
   QGridLayout *traceFilterSetting;
   QPushButton *traceClearButton;
   QPushButton *traceBottomButton;
   QListWidget *traceView;
   QLineEdit *traceFilter_0;
   QLineEdit *traceFilter_1;
   QLineEdit *traceFilter_2;
   QLineEdit *traceFilter_3;
   QLineEdit *traceFilter_4;
   QLineEdit *traceFilter_5;
   QLineEdit *traceFilter_6;
   QPushButton *traceFilterButton_0;
   QPushButton *traceFilterButton_1;
   QPushButton *traceFilterButton_2;
   QPushButton *traceFilterButton_3;
   QPushButton *traceFilterButton_4;
   QPushButton *traceFilterButton_5;
   QPushButton *traceFilterButton_6;

   /* port buttons layout */
   QGridLayout *gridLayout;
   QVBoxLayout *verticalLayout_3;
   QGridLayout *portButtonsLayout;
   QPushButton *portButton_0;
   QPushButton *portButton_1;
   QPushButton *portButton_2;
   QPushButton *portButton_3;
   QPushButton *portButton_4;
   QLabel *portLabel_0;
   QLabel *portLabel_1;
   QLabel *portLabel_2;
   QLabel *portLabel_3;
   QLabel *portLabel_4;

   /* upper and lower bars */
   QMenuBar *menubar;
   QStatusBar *statusbar;
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

   void setupUi(QMainWindow *MainWindow)
   {
      if (MainWindow->objectName().isEmpty())
         MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
      MainWindow->resize(872, 690);
      mainWindow = MainWindow;
      centralwidget = new QWidget(MainWindow);
      centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
      QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
      sizePolicy.setHorizontalStretch(0);
      sizePolicy.setVerticalStretch(0);
      sizePolicy.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
      centralwidget->setSizePolicy(sizePolicy);
      gridLayout = new QGridLayout(centralwidget);
      gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
      verticalLayout_3 = new QVBoxLayout();
      verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
      verticalLayout_3->setSizeConstraint(QLayout::SetMinimumSize);

      controlButtonsLayout = new QGridLayout();
      controlButtonsLayout->setObjectName(QString::fromUtf8("controlButtonsLayout"));

      markerButton = new QPushButton(centralwidget);
      markerButton->setObjectName(QString::fromUtf8("markerButton"));
      controlButtonsLayout->addWidget(markerButton, 0, 0, 1, 1);
      loggingButton = new QPushButton(centralwidget);
      loggingButton->setObjectName(QString::fromUtf8("loggingButton"));
      controlButtonsLayout->addWidget(loggingButton, 0, 1, 1, 1);
      settingsButton = new QPushButton(centralwidget);
      settingsButton->setObjectName(QString::fromUtf8("settingsButton"));
      controlButtonsLayout->addWidget(settingsButton, 0, 2, 1, 1);
      infoLabel = new QLabel(centralwidget);
      infoLabel->setObjectName(QString::fromUtf8("infoLabel"));
      controlButtonsLayout->addWidget(infoLabel, 0, 4, 1, 1);
      verticalLayout_3->addLayout(controlButtonsLayout);

      buttonsTabWidget = new QTabWidget();
      QSizePolicy policy = buttonsTabWidget->sizePolicy();
      policy.setVerticalPolicy(QSizePolicy::Minimum);
      buttonsTabWidget->setSizePolicy(policy);
      uint8_t tabs_count = SETTING_GET_U32(GUI_UserButtons_Tabs);
      for (uint8_t i = 0; i < tabs_count; i++)
      {
         UT_Log(GUI_CONTROLLER, LOW, "Creating user button tab %u", i);
         createUserButtonTab(i);
      }
      verticalLayout_3->addWidget(buttonsTabWidget);

      /*fill terminal layout */
      splitter_2 = new QSplitter(centralwidget);
      splitter_2->setObjectName(QString::fromUtf8("splitter_2"));
      splitter_2->setOrientation(Qt::Vertical);
      gridLayoutWidget_2 = new QWidget(splitter_2);
      gridLayoutWidget_2->setObjectName(QString::fromUtf8("gridLayoutWidget_2"));
      terminalLayout = new QGridLayout(gridLayoutWidget_2);
      terminalLayout->setObjectName(QString::fromUtf8("terminalLayout"));
      terminalLayout->setContentsMargins(0, 0, 0, 0);
      terminalView = new QTextEdit(gridLayoutWidget_2);
      terminalView->setObjectName(QString::fromUtf8("terminalView"));
      terminalView->setReadOnly(true);
      terminalView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      terminalView->setWordWrapMode(QTextOption::WrapAnywhere);
      terminalLayout->addWidget(terminalView, 1, 0, 1, 5);

      portComboBox = new QComboBox(gridLayoutWidget_2);
      portComboBox->setObjectName(QString::fromUtf8("portComboBox"));
      QSizePolicy sizePolicy1(QSizePolicy::Maximum, QSizePolicy::Fixed);
      sizePolicy1.setHorizontalStretch(0);
      sizePolicy1.setVerticalStretch(0);
      sizePolicy1.setHeightForWidth(portComboBox->sizePolicy().hasHeightForWidth());
      portComboBox->setSizePolicy(sizePolicy1);
      terminalLayout->addWidget(portComboBox, 2, 0, 1, 1);

      sendButton = new QPushButton(gridLayoutWidget_2);
      sendButton->setObjectName(QString::fromUtf8("sendButton"));
      QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
      sizePolicy2.setHorizontalStretch(0);
      sizePolicy2.setVerticalStretch(0);
      sizePolicy2.setHeightForWidth(sendButton->sizePolicy().hasHeightForWidth());
      sendButton->setSizePolicy(sizePolicy2);
      terminalLayout->addWidget(sendButton, 2, 4, 1, 1);

      terminalBottomButton = new QPushButton(gridLayoutWidget_2);
      terminalBottomButton->setObjectName(QString::fromUtf8("bottomButton"));
      terminalLayout->addWidget(terminalBottomButton, 0, 3, 1, 2);

      lineEndingComboBox = new QComboBox(gridLayoutWidget_2);
      lineEndingComboBox->setObjectName(QString::fromUtf8("lineEndingComboBox"));
      sizePolicy1.setHeightForWidth(lineEndingComboBox->sizePolicy().hasHeightForWidth());
      lineEndingComboBox->setSizePolicy(sizePolicy1);
      terminalLayout->addWidget(lineEndingComboBox, 2, 3, 1, 1);

      textEdit = new QComboBox(gridLayoutWidget_2);
      textEdit->setObjectName(QString::fromUtf8("textEdit"));
      QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Fixed);
      sizePolicy3.setHorizontalStretch(0);
      sizePolicy3.setVerticalStretch(0);
      sizePolicy3.setHeightForWidth(textEdit->sizePolicy().hasHeightForWidth());
      textEdit->setSizePolicy(sizePolicy3);
      textEdit->setEditable(true);
      textEdit->setInsertPolicy(QComboBox::NoInsert);
      terminalLayout->addWidget(textEdit, 2, 1, 1, 2);

      clearButton = new QPushButton(gridLayoutWidget_2);
      clearButton->setObjectName(QString::fromUtf8("clearButton"));
      terminalLayout->addWidget(clearButton, 0, 0, 1, 3);

      /* fill trace filter layout */
      splitter_2->addWidget(gridLayoutWidget_2);
      layoutWidget = new QWidget(splitter_2);
      layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
      verticalLayout_2 = new QVBoxLayout(layoutWidget);
      verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
      verticalLayout_2->setContentsMargins(0, 0, 0, 0);
      splitter = new QSplitter(layoutWidget);
      splitter->setObjectName(QString::fromUtf8("splitter"));
      splitter->setOrientation(Qt::Horizontal);
      gridLayoutWidget_3 = new QWidget(splitter);
      gridLayoutWidget_3->setObjectName(QString::fromUtf8("gridLayoutWidget_3"));
      traceFilterLayout = new QGridLayout(gridLayoutWidget_3);
      traceFilterLayout->setObjectName(QString::fromUtf8("traceFilterLayout"));
      traceFilterLayout->setContentsMargins(0, 0, 0, 0);
      traceClearButton = new QPushButton(gridLayoutWidget_3);
      traceClearButton->setObjectName(QString::fromUtf8("traceClearButton"));
      traceFilterLayout->addWidget(traceClearButton, 0, 0, 1, 1);

      traceBottomButton = new QPushButton(gridLayoutWidget_3);
      traceBottomButton->setObjectName(QString::fromUtf8("traceBottomButton"));
      traceFilterLayout->addWidget(traceBottomButton, 0, 1, 1, 1);

      traceView = new QListWidget(gridLayoutWidget_3);
      traceView->setObjectName(QString::fromUtf8("traceView"));
      traceView->setUniformItemSizes(true);
      traceView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      traceView->setWordWrap(true);
      QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Expanding);
      sizePolicy4.setHorizontalStretch(0);
      sizePolicy4.setVerticalStretch(0);
      sizePolicy4.setHeightForWidth(traceView->sizePolicy().hasHeightForWidth());
      traceView->setSizePolicy(sizePolicy4);
      traceFilterLayout->addWidget(traceView, 1, 0, 1, 2);

      splitter->addWidget(gridLayoutWidget_3);
      gridLayoutWidget_5 = new QWidget(splitter);
      gridLayoutWidget_5->setObjectName(QString::fromUtf8("gridLayoutWidget_5"));
      traceFilterSetting = new QGridLayout(gridLayoutWidget_5);
      traceFilterSetting->setObjectName(QString::fromUtf8("traceFilterSetting"));
      traceFilterSetting->setContentsMargins(0, 0, 0, 0);

      traceFilter_0 = new QLineEdit(gridLayoutWidget_5);
      traceFilter_0->setObjectName(QString::fromUtf8("traceFilter_0"));
      traceFilterSetting->addWidget(traceFilter_0, 0, 0, 1, 1);

      traceFilter_1 = new QLineEdit(gridLayoutWidget_5);
      traceFilter_1->setObjectName(QString::fromUtf8("traceFilter_1"));
      traceFilterSetting->addWidget(traceFilter_1, 1, 0, 1, 1);

      traceFilter_2 = new QLineEdit(gridLayoutWidget_5);
      traceFilter_2->setObjectName(QString::fromUtf8("traceFilter_2"));
      traceFilterSetting->addWidget(traceFilter_2, 2, 0, 1, 1);

      traceFilter_3 = new QLineEdit(gridLayoutWidget_5);
      traceFilter_3->setObjectName(QString::fromUtf8("traceFilter_3"));
      traceFilterSetting->addWidget(traceFilter_3, 3, 0, 1, 1);

      traceFilter_4 = new QLineEdit(gridLayoutWidget_5);
      traceFilter_4->setObjectName(QString::fromUtf8("traceFilter_4"));
      traceFilterSetting->addWidget(traceFilter_4, 4, 0, 1, 1);

      traceFilter_5 = new QLineEdit(gridLayoutWidget_5);
      traceFilter_5->setObjectName(QString::fromUtf8("traceFilter_5"));
      traceFilterSetting->addWidget(traceFilter_5, 5, 0, 1, 1);

      traceFilter_6 = new QLineEdit(gridLayoutWidget_5);
      traceFilter_6->setObjectName(QString::fromUtf8("traceFilter_6"));
      traceFilterSetting->addWidget(traceFilter_6, 6, 0, 1, 1);

      traceFilterButton_0 = new QPushButton(gridLayoutWidget_5);
      traceFilterButton_0->setObjectName(QString::fromUtf8("traceFilterButton_0"));
      traceFilterSetting->addWidget(traceFilterButton_0, 0, 1, 1, 1);

      traceFilterButton_1 = new QPushButton(gridLayoutWidget_5);
      traceFilterButton_1->setObjectName(QString::fromUtf8("traceFilterButton_1"));
      traceFilterSetting->addWidget(traceFilterButton_1, 1, 1, 1, 1);

      traceFilterButton_2 = new QPushButton(gridLayoutWidget_5);
      traceFilterButton_2->setObjectName(QString::fromUtf8("traceFilterButton_2"));
      traceFilterSetting->addWidget(traceFilterButton_2, 2, 1, 1, 1);

      traceFilterButton_3 = new QPushButton(gridLayoutWidget_5);
      traceFilterButton_3->setObjectName(QString::fromUtf8("traceFilterButton_3"));
      traceFilterSetting->addWidget(traceFilterButton_3, 3, 1, 1, 1);

      traceFilterButton_4 = new QPushButton(gridLayoutWidget_5);
      traceFilterButton_4->setObjectName(QString::fromUtf8("traceFilterButton_4"));
      traceFilterSetting->addWidget(traceFilterButton_4, 4, 1, 1, 1);

      traceFilterButton_5 = new QPushButton(gridLayoutWidget_5);
      traceFilterButton_5->setObjectName(QString::fromUtf8("traceFilterButton_5"));
      traceFilterSetting->addWidget(traceFilterButton_5, 5, 1, 1, 1);

      traceFilterButton_6 = new QPushButton(gridLayoutWidget_5);
      traceFilterButton_6->setObjectName(QString::fromUtf8("traceFilterButton_6"));
      traceFilterSetting->addWidget(traceFilterButton_6, 6, 1, 1, 1);

      splitter->addWidget(gridLayoutWidget_5);

      verticalLayout_2->addWidget(splitter);

      portButtonsLayout = new QGridLayout();
      portButtonsLayout->setObjectName(QString::fromUtf8("portButtonsLayout"));
      portButton_0 = new QPushButton(layoutWidget);
      portButton_0->setObjectName(QString::fromUtf8("portButton_0"));
      portButtonsLayout->addWidget(portButton_0, 0, 0, 1, 1);

      portButton_1 = new QPushButton(layoutWidget);
      portButton_1->setObjectName(QString::fromUtf8("portButton_1"));
      portButtonsLayout->addWidget(portButton_1, 0, 1, 1, 1);

      portButton_2 = new QPushButton(layoutWidget);
      portButton_2->setObjectName(QString::fromUtf8("portButton_2"));
      portButtonsLayout->addWidget(portButton_2, 0, 2, 1, 1);

      portButton_3 = new QPushButton(layoutWidget);
      portButton_3->setObjectName(QString::fromUtf8("portButton_3"));
      portButtonsLayout->addWidget(portButton_3, 0, 3, 1, 1);

      portButton_4 = new QPushButton(layoutWidget);
      portButton_4->setObjectName(QString::fromUtf8("portButton_4"));
      portButtonsLayout->addWidget(portButton_4, 0, 4, 1, 1);

      portLabel_0 = new QLabel(layoutWidget);
      portLabel_0->setObjectName(QString::fromUtf8("portLabel_0"));
      QSizePolicy sizePolicy5(QSizePolicy::Preferred, QSizePolicy::Maximum);
      sizePolicy5.setHorizontalStretch(0);
      sizePolicy5.setVerticalStretch(0);
      sizePolicy5.setHeightForWidth(portLabel_0->sizePolicy().hasHeightForWidth());
      portLabel_0->setSizePolicy(sizePolicy5);
      portButtonsLayout->addWidget(portLabel_0, 1, 0, 1, 1);

      portLabel_1 = new QLabel(layoutWidget);
      portLabel_1->setObjectName(QString::fromUtf8("portLabel_1"));
      sizePolicy5.setHeightForWidth(portLabel_1->sizePolicy().hasHeightForWidth());
      portLabel_1->setSizePolicy(sizePolicy5);
      portButtonsLayout->addWidget(portLabel_1, 1, 1, 1, 1);

      portLabel_2 = new QLabel(layoutWidget);
      portLabel_2->setObjectName(QString::fromUtf8("portLabel_2"));
      sizePolicy5.setHeightForWidth(portLabel_2->sizePolicy().hasHeightForWidth());
      portLabel_2->setSizePolicy(sizePolicy5);
      portButtonsLayout->addWidget(portLabel_2, 1, 2, 1, 1);

      portLabel_3 = new QLabel(layoutWidget);
      portLabel_3->setObjectName(QString::fromUtf8("portLabel_3"));
      sizePolicy5.setHeightForWidth(portLabel_3->sizePolicy().hasHeightForWidth());
      portLabel_3->setSizePolicy(sizePolicy5);
      portButtonsLayout->addWidget(portLabel_3, 1, 3, 1, 1);

      portLabel_4 = new QLabel(layoutWidget);
      portLabel_4->setObjectName(QString::fromUtf8("portLabel_4"));
      sizePolicy5.setHeightForWidth(portLabel_4->sizePolicy().hasHeightForWidth());
      portLabel_4->setSizePolicy(sizePolicy5);
      portButtonsLayout->addWidget(portLabel_4, 1, 4, 1, 1);

      verticalLayout_2->addLayout(portButtonsLayout);
      splitter_2->addWidget(layoutWidget);
      verticalLayout_3->addWidget(splitter_2);
      gridLayout->addLayout(verticalLayout_3, 0, 0, 1, 1);

      MainWindow->setCentralWidget(centralwidget);
      menubar = new QMenuBar(MainWindow);
      menubar->setObjectName(QString::fromUtf8("menubar"));
      menubar->setGeometry(QRect(0, 0, 872, 22));
      MainWindow->setMenuBar(menubar);
      statusbar = new QStatusBar(MainWindow);
      statusbar->setObjectName(QString::fromUtf8("statusbar"));
      MainWindow->setStatusBar(statusbar);

      retranslateUi(MainWindow);

      QMetaObject::connectSlotsByName(MainWindow);

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

      m_buttons.push_back(markerButton);
      m_buttons.push_back(loggingButton);
      m_buttons.push_back(settingsButton);
      m_buttons.push_back(sendButton);
      m_buttons.push_back(terminalBottomButton);
      m_buttons.push_back(clearButton);
      m_buttons.push_back(traceClearButton);
      m_buttons.push_back(traceBottomButton);
      m_buttons.push_back(traceFilterButton_0);
      m_buttons.push_back(traceFilterButton_1);
      m_buttons.push_back(traceFilterButton_2);
      m_buttons.push_back(traceFilterButton_3);
      m_buttons.push_back(traceFilterButton_4);
      m_buttons.push_back(traceFilterButton_5);
      m_buttons.push_back(traceFilterButton_6);
      m_buttons.push_back(portButton_0);
      m_buttons.push_back(portButton_1);
      m_buttons.push_back(portButton_2);
      m_buttons.push_back(portButton_3);
      m_buttons.push_back(portButton_4);

      m_trace_filters.push_back({traceFilter_0, traceFilterButton_0});
      m_trace_filters.push_back({traceFilter_1, traceFilterButton_1});
      m_trace_filters.push_back({traceFilter_2, traceFilterButton_2});
      m_trace_filters.push_back({traceFilter_3, traceFilterButton_3});
      m_trace_filters.push_back({traceFilter_4, traceFilterButton_4});
      m_trace_filters.push_back({traceFilter_5, traceFilterButton_5});
      m_trace_filters.push_back({traceFilter_6, traceFilterButton_6});



      m_ports.push_back({portLabel_0, portButton_0});
      m_ports.push_back({portLabel_1, portButton_1});
      m_ports.push_back({portLabel_2, portButton_2});
      m_ports.push_back({portLabel_3, portButton_3});
      m_ports.push_back({portLabel_4, portButton_4});
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
      return m_buttons;
   }
   const std::vector<TraceFilterItem>& getTraceFilters()
   {
      return m_trace_filters;
   }
   const std::vector<PortItem>& getPorts()
   {
      return m_ports;
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
      return m_trace_filters.size();
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
            QPushButton* button = new QPushButton(centralwidget);
            m_buttons.push_back(button);
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

   std::vector<QPushButton*> m_buttons;
   std::vector<TraceFilterItem> m_trace_filters;
   std::vector<PortItem> m_ports;
};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
