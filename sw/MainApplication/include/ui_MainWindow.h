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

#include "Settings.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
   QWidget *centralwidget;

   /* layout with user buttons at the top of application */
   QPushButton *markerButton;
   QPushButton *loggingButton;
   QPushButton *settingsButton;
   QComboBox *buttonVariant;
   QLabel *infoLabel;
   QGridLayout *userButtonsLayout;
   QGridLayout *controlButtonsLayout;
   QPushButton *userButton_1;
   QPushButton *userButton_2;
   QPushButton *userButton_5;
   QPushButton *userButton_3;
   QPushButton *userButton_4;
   QPushButton *userButton_6;
   QPushButton *userButton_7;
   QPushButton *userButton_8;
   QPushButton *userButton_9;
   QPushButton *userButton_10;

   /* terminal layout */
   QGridLayout *terminalLayout;
   QSplitter *splitter_2;
   QWidget *gridLayoutWidget_2;
   QListWidget *terminalView;
   QComboBox *portComboBox;
   QPushButton *sendButton;
   QPushButton *scrollButton;
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
   QPushButton *traceScrollButton;
   QListWidget *traceView;
   QLineEdit *traceFilter_1;
   QLineEdit *traceFilter_2;
   QLineEdit *traceFilter_3;
   QLineEdit *traceFilter_4;
   QLineEdit *traceFilter_5;
   QLineEdit *traceFilter_6;
   QLineEdit *traceFilter_7;
   QPushButton *traceFilterButton_1;
   QPushButton *traceFilterButton_2;
   QPushButton *traceFilterButton_3;
   QPushButton *traceFilterButton_4;
   QPushButton *traceFilterButton_5;
   QPushButton *traceFilterButton_6;
   QPushButton *traceFilterButton_7;

   /* port buttons layout */
   QGridLayout *gridLayout;
   QVBoxLayout *verticalLayout_3;
   QGridLayout *portButtonsLayout;
   QPushButton *portButton_1;
   QPushButton *portButton_2;
   QPushButton *portButton_3;
   QPushButton *portButton_4;
   QPushButton *portButton_5;
   QLabel *portLabel_1;
   QLabel *portLabel_2;
   QLabel *portLabel_3;
   QLabel *portLabel_4;
   QLabel *portLabel_5;

   /* upper and lower bars */
   QMenuBar *menubar;
   QStatusBar *statusbar;
   QMainWindow* mainWindow;

   /* key shortcuts */
   QShortcut* loggingButtonShortcut;
   QShortcut* markerButtonShortcut;
   QShortcut* port1ButtonShortcut;
   QShortcut* port2ButtonShortcut;
   QShortcut* port3ButtonShortcut;
   QShortcut* port4ButtonShortcut;
   QShortcut* port5ButtonShortcut;
   QShortcut* clearButtonShortcut;
   QShortcut* traceClearButtonShortcut;
   QShortcut* switchSendPortShortcut;

   enum class Theme
   {
      DEFAULT,
      DARK,
   };

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
      verticalLayout_3->setSizeConstraint(QLayout::SetDefaultConstraint);

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
      buttonVariant = new QComboBox(centralwidget);
      buttonVariant->setObjectName(QString::fromUtf8("buttonVariant"));
      controlButtonsLayout->addWidget(buttonVariant, 0, 3, 1, 1);
      infoLabel = new QLabel(centralwidget);
      infoLabel->setObjectName(QString::fromUtf8("infoLabel"));
      controlButtonsLayout->addWidget(infoLabel, 0, 4, 1, 1);

      verticalLayout_3->addLayout(controlButtonsLayout);

      userButtonsLayout = new QGridLayout();
      userButtonsLayout->setObjectName(QString::fromUtf8("userButtonsLayout"));

      /* create user buttons */
      userButton_1 = new QPushButton(centralwidget);
      userButton_1->setObjectName(QString::fromUtf8("userButton_1"));
      userButtonsLayout->addWidget(userButton_1, 0, 0, 1, 1);

      userButton_2 = new QPushButton(centralwidget);
      userButton_2->setObjectName(QString::fromUtf8("userButton_2"));
      userButtonsLayout->addWidget(userButton_2, 0, 1, 1, 1);

      userButton_3 = new QPushButton(centralwidget);
      userButton_3->setObjectName(QString::fromUtf8("userButton_3"));
      userButtonsLayout->addWidget(userButton_3, 0, 2, 1, 1);

      userButton_4 = new QPushButton(centralwidget);
      userButton_4->setObjectName(QString::fromUtf8("userButton_4"));
      userButtonsLayout->addWidget(userButton_4, 0, 3, 1, 1);

      userButton_5 = new QPushButton(centralwidget);
      userButton_5->setObjectName(QString::fromUtf8("userButton_5"));
      userButtonsLayout->addWidget(userButton_5, 0, 4, 1, 1);

      userButton_6 = new QPushButton(centralwidget);
      userButton_6->setObjectName(QString::fromUtf8("userButton_6"));
      userButtonsLayout->addWidget(userButton_6, 1, 0, 1, 1);

      userButton_7 = new QPushButton(centralwidget);
      userButton_7->setObjectName(QString::fromUtf8("userButton_7"));
      userButtonsLayout->addWidget(userButton_7, 1, 1, 1, 1);

      userButton_8 = new QPushButton(centralwidget);
      userButton_8->setObjectName(QString::fromUtf8("userButton_8"));
      userButtonsLayout->addWidget(userButton_8, 1, 2, 1, 1);

      userButton_9 = new QPushButton(centralwidget);
      userButton_9->setObjectName(QString::fromUtf8("userButton_9"));
      userButtonsLayout->addWidget(userButton_9, 1, 3, 1, 1);

      userButton_10 = new QPushButton(centralwidget);
      userButton_10->setObjectName(QString::fromUtf8("userButton_10"));
      userButtonsLayout->addWidget(userButton_10, 1, 4, 1, 1);

      verticalLayout_3->addLayout(userButtonsLayout);

      /*fill terminal layout */
      splitter_2 = new QSplitter(centralwidget);
      splitter_2->setObjectName(QString::fromUtf8("splitter_2"));
      splitter_2->setOrientation(Qt::Vertical);
      gridLayoutWidget_2 = new QWidget(splitter_2);
      gridLayoutWidget_2->setObjectName(QString::fromUtf8("gridLayoutWidget_2"));
      terminalLayout = new QGridLayout(gridLayoutWidget_2);
      terminalLayout->setObjectName(QString::fromUtf8("terminalLayout"));
      terminalLayout->setContentsMargins(0, 0, 0, 0);
      terminalView = new QListWidget(gridLayoutWidget_2);
      terminalView->setObjectName(QString::fromUtf8("terminalView"));
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

      scrollButton = new QPushButton(gridLayoutWidget_2);
      scrollButton->setObjectName(QString::fromUtf8("scrollButton"));
      terminalLayout->addWidget(scrollButton, 0, 3, 1, 2);

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

      traceScrollButton = new QPushButton(gridLayoutWidget_3);
      traceScrollButton->setObjectName(QString::fromUtf8("traceScrollButton"));
      traceFilterLayout->addWidget(traceScrollButton, 0, 1, 1, 1);

      traceView = new QListWidget(gridLayoutWidget_3);
      traceView->setObjectName(QString::fromUtf8("traceView"));
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

      traceFilter_1 = new QLineEdit(gridLayoutWidget_5);
      traceFilter_1->setObjectName(QString::fromUtf8("traceFilter_1"));
      traceFilterSetting->addWidget(traceFilter_1, 0, 0, 1, 1);

      traceFilter_2 = new QLineEdit(gridLayoutWidget_5);
      traceFilter_2->setObjectName(QString::fromUtf8("traceFilter_2"));
      traceFilterSetting->addWidget(traceFilter_2, 1, 0, 1, 1);

      traceFilter_3 = new QLineEdit(gridLayoutWidget_5);
      traceFilter_3->setObjectName(QString::fromUtf8("traceFilter_3"));
      traceFilterSetting->addWidget(traceFilter_3, 2, 0, 1, 1);

      traceFilter_4 = new QLineEdit(gridLayoutWidget_5);
      traceFilter_4->setObjectName(QString::fromUtf8("traceFilter_4"));
      traceFilterSetting->addWidget(traceFilter_4, 3, 0, 1, 1);

      traceFilter_5 = new QLineEdit(gridLayoutWidget_5);
      traceFilter_5->setObjectName(QString::fromUtf8("traceFilter_5"));
      traceFilterSetting->addWidget(traceFilter_5, 4, 0, 1, 1);

      traceFilter_6 = new QLineEdit(gridLayoutWidget_5);
      traceFilter_6->setObjectName(QString::fromUtf8("traceFilter_6"));
      traceFilterSetting->addWidget(traceFilter_6, 5, 0, 1, 1);

      traceFilter_7 = new QLineEdit(gridLayoutWidget_5);
      traceFilter_7->setObjectName(QString::fromUtf8("traceFilter_7"));
      traceFilterSetting->addWidget(traceFilter_7, 6, 0, 1, 1);

      traceFilterButton_1 = new QPushButton(gridLayoutWidget_5);
      traceFilterButton_1->setObjectName(QString::fromUtf8("traceFilterButton_1"));
      traceFilterSetting->addWidget(traceFilterButton_1, 0, 1, 1, 1);

      traceFilterButton_2 = new QPushButton(gridLayoutWidget_5);
      traceFilterButton_2->setObjectName(QString::fromUtf8("traceFilterButton_2"));
      traceFilterSetting->addWidget(traceFilterButton_2, 1, 1, 1, 1);

      traceFilterButton_3 = new QPushButton(gridLayoutWidget_5);
      traceFilterButton_3->setObjectName(QString::fromUtf8("traceFilterButton_3"));
      traceFilterSetting->addWidget(traceFilterButton_3, 2, 1, 1, 1);

      traceFilterButton_4 = new QPushButton(gridLayoutWidget_5);
      traceFilterButton_4->setObjectName(QString::fromUtf8("traceFilterButton_4"));
      traceFilterSetting->addWidget(traceFilterButton_4, 3, 1, 1, 1);

      traceFilterButton_5 = new QPushButton(gridLayoutWidget_5);
      traceFilterButton_5->setObjectName(QString::fromUtf8("traceFilterButton_5"));
      traceFilterSetting->addWidget(traceFilterButton_5, 4, 1, 1, 1);

      traceFilterButton_6 = new QPushButton(gridLayoutWidget_5);
      traceFilterButton_6->setObjectName(QString::fromUtf8("traceFilterButton_6"));
      traceFilterSetting->addWidget(traceFilterButton_6, 5, 1, 1, 1);

      traceFilterButton_7 = new QPushButton(gridLayoutWidget_5);
      traceFilterButton_7->setObjectName(QString::fromUtf8("traceFilterButton_7"));
      traceFilterSetting->addWidget(traceFilterButton_7, 6, 1, 1, 1);

      splitter->addWidget(gridLayoutWidget_5);

      verticalLayout_2->addWidget(splitter);

      portButtonsLayout = new QGridLayout();
      portButtonsLayout->setObjectName(QString::fromUtf8("portButtonsLayout"));
      portButton_1 = new QPushButton(layoutWidget);
      portButton_1->setObjectName(QString::fromUtf8("portButton_1"));
      portButtonsLayout->addWidget(portButton_1, 0, 0, 1, 1);

      portButton_2 = new QPushButton(layoutWidget);
      portButton_2->setObjectName(QString::fromUtf8("portButton_2"));
      portButtonsLayout->addWidget(portButton_2, 0, 1, 1, 1);

      portButton_3 = new QPushButton(layoutWidget);
      portButton_3->setObjectName(QString::fromUtf8("portButton_3"));
      portButtonsLayout->addWidget(portButton_3, 0, 2, 1, 1);

      portButton_4 = new QPushButton(layoutWidget);
      portButton_4->setObjectName(QString::fromUtf8("portButton_4"));
      portButtonsLayout->addWidget(portButton_4, 0, 3, 1, 1);

      portButton_5 = new QPushButton(layoutWidget);
      portButton_5->setObjectName(QString::fromUtf8("portButton_5"));
      portButtonsLayout->addWidget(portButton_5, 0, 4, 1, 1);

      portLabel_1 = new QLabel(layoutWidget);
      portLabel_1->setObjectName(QString::fromUtf8("portLabel_1"));
      QSizePolicy sizePolicy5(QSizePolicy::Preferred, QSizePolicy::Maximum);
      sizePolicy5.setHorizontalStretch(0);
      sizePolicy5.setVerticalStretch(0);
      sizePolicy5.setHeightForWidth(portLabel_1->sizePolicy().hasHeightForWidth());
      portLabel_1->setSizePolicy(sizePolicy5);
      portButtonsLayout->addWidget(portLabel_1, 1, 0, 1, 1);

      portLabel_2 = new QLabel(layoutWidget);
      portLabel_2->setObjectName(QString::fromUtf8("portLabel_2"));
      sizePolicy5.setHeightForWidth(portLabel_2->sizePolicy().hasHeightForWidth());
      portLabel_2->setSizePolicy(sizePolicy5);
      portButtonsLayout->addWidget(portLabel_2, 1, 1, 1, 1);

      portLabel_3 = new QLabel(layoutWidget);
      portLabel_3->setObjectName(QString::fromUtf8("portLabel_3"));
      sizePolicy5.setHeightForWidth(portLabel_3->sizePolicy().hasHeightForWidth());
      portLabel_3->setSizePolicy(sizePolicy5);
      portButtonsLayout->addWidget(portLabel_3, 1, 2, 1, 1);

      portLabel_4 = new QLabel(layoutWidget);
      portLabel_4->setObjectName(QString::fromUtf8("portLabel_4"));
      sizePolicy5.setHeightForWidth(portLabel_4->sizePolicy().hasHeightForWidth());
      portLabel_4->setSizePolicy(sizePolicy5);
      portButtonsLayout->addWidget(portLabel_4, 1, 3, 1, 1);

      portLabel_5 = new QLabel(layoutWidget);
      portLabel_5->setObjectName(QString::fromUtf8("portLabel_5"));
      sizePolicy5.setHeightForWidth(portLabel_5->sizePolicy().hasHeightForWidth());
      portLabel_5->setSizePolicy(sizePolicy5);
      portButtonsLayout->addWidget(portLabel_5, 1, 4, 1, 1);


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
      markerButtonShortcut = new QShortcut(Qt::CTRL + Qt::Key_M, mainWindow);
      port1ButtonShortcut = new QShortcut(Qt::CTRL + Qt::Key_1, mainWindow);
      port2ButtonShortcut = new QShortcut(Qt::CTRL + Qt::Key_2, mainWindow);
      port3ButtonShortcut = new QShortcut(Qt::CTRL + Qt::Key_3, mainWindow);
      port4ButtonShortcut = new QShortcut(Qt::CTRL + Qt::Key_4, mainWindow);
      port5ButtonShortcut = new QShortcut(Qt::CTRL + Qt::Key_5, mainWindow);
      clearButtonShortcut = new QShortcut(Qt::Key_F3, mainWindow);
      traceClearButtonShortcut = new QShortcut(Qt::Key_F4, mainWindow);
      switchSendPortShortcut = new QShortcut(Qt::Key_F1, mainWindow);
   }

   void retranslateUi(QMainWindow *MainWindow)
   {
      MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
      infoLabel->setText(QString());
      userButton_1->setText(QString());
      userButton_2->setText(QString());
      userButton_3->setText(QString());
      userButton_4->setText(QString());
      userButton_5->setText(QString());
      userButton_6->setText(QString());
      userButton_7->setText(QString());
      userButton_8->setText(QString());
      userButton_9->setText(QString());
      userButton_10->setText(QString());
      markerButton->setText(QCoreApplication::translate("MainWindow", "MARKER", nullptr));
      loggingButton->setText(QCoreApplication::translate("MainWindow", "LOG", nullptr));
      settingsButton->setText(QCoreApplication::translate("MainWindow", "SETTINGS", nullptr));
      sendButton->setText(QCoreApplication::translate("MainWindow", "SEND", nullptr));
      scrollButton->setText(QCoreApplication::translate("MainWindow", "SCROLL", nullptr));
      clearButton->setText(QCoreApplication::translate("MainWindow", "CLEAR", nullptr));
      traceClearButton->setText(QCoreApplication::translate("MainWindow", "CLEAR", nullptr));
      traceScrollButton->setText(QCoreApplication::translate("MainWindow", "SCROLL", nullptr));
      traceFilterButton_1->setText(QCoreApplication::translate("MainWindow", "ACTIVE", nullptr));
      traceFilterButton_2->setText(QCoreApplication::translate("MainWindow", "ACTIVE", nullptr));
      traceFilterButton_3->setText(QCoreApplication::translate("MainWindow", "ACTIVE", nullptr));
      traceFilterButton_4->setText(QCoreApplication::translate("MainWindow", "ACTIVE", nullptr));
      traceFilterButton_5->setText(QCoreApplication::translate("MainWindow", "ACTIVE", nullptr));
      traceFilterButton_6->setText(QCoreApplication::translate("MainWindow", "ACTIVE", nullptr));
      traceFilterButton_7->setText(QCoreApplication::translate("MainWindow", "ACTIVE", nullptr));
      portButton_1->setText(QCoreApplication::translate("MainWindow", "PORT1", nullptr));
      portButton_2->setText(QCoreApplication::translate("MainWindow", "PORT2", nullptr));
      portButton_3->setText(QCoreApplication::translate("MainWindow", "PORT3", nullptr));
      portButton_4->setText(QCoreApplication::translate("MainWindow", "PORT4", nullptr));
      portButton_5->setText(QCoreApplication::translate("MainWindow", "PORT5", nullptr));
      portLabel_1->setText(QString());
      portLabel_2->setText(QString());
      portLabel_3->setText(QString());
      portLabel_4->setText(QString());
      portLabel_5->setText(QString());
   }

   void loadTheme(Theme theme)
   {
      if(theme == Theme::DEFAULT)
      {
       renderDefaultTheme();
      }
      else if(theme == Theme::DARK)
      {
       renderDarkTheme();
      }
   }
private:

   void renderDarkTheme()
   {
      const QColor BACKGROUND_COLOR = SETTING_GET_U32(GUI_Dark_WindowBackground);
      const QColor TERMINAL_BACKGROUND_COLOR = SETTING_GET_U32(GUI_Dark_TerminalBackground);
      const QColor TEXT_COLOR = SETTING_GET_U32(GUI_Dark_WindowText);

      QPalette combobox_palette = userButton_1->palette();
      combobox_palette.setColor(QPalette::Window, BACKGROUND_COLOR);
      combobox_palette.setColor(QPalette::WindowText, TEXT_COLOR);
      combobox_palette.setColor(QPalette::Text, TEXT_COLOR);
      portComboBox->setPalette(combobox_palette);
      lineEndingComboBox->setPalette(combobox_palette);
      portComboBox->view()->setPalette(combobox_palette);
      lineEndingComboBox->view()->setPalette(combobox_palette);
      textEdit->setPalette(combobox_palette);
      textEdit->view()->setPalette(combobox_palette);
      buttonVariant->setPalette(combobox_palette);
      buttonVariant->view()->setPalette(combobox_palette);

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
   }
   void renderDefaultTheme()
   {
      portComboBox->setPalette({});
      portComboBox->view()->setPalette({});
      lineEndingComboBox->setPalette({});
      lineEndingComboBox->view()->setPalette({});
      textEdit->setPalette({});
      textEdit->view()->setPalette({});
      buttonVariant->setPalette({});
      buttonVariant->view()->setPalette({});
      terminalView->setPalette({});
      traceView->setPalette({});
      mainWindow->setPalette({});
   }
};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H