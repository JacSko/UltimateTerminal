#pragma once

#include <sstream>
#include <condition_variable>

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

namespace GUIController
{

class CommandExecutor
{
public:
   CommandExecutor()
   {
      ready = false;
   }
   virtual ~CommandExecutor(){};
   virtual void execute(Ui::MainWindow* ui) = 0;
   virtual bool waitReady(uint32_t timeout)
   {
      std::unique_lock<std::mutex> lock(mutex);
      bool result = cond_var.wait_for(lock, std::chrono::milliseconds(timeout), [&](){return ready.load();});
      ready = false;
      return result;
   }
protected:
   std::atomic<bool> ready;
   std::mutex mutex;
   std::condition_variable cond_var;
};

class GetCommandRequest : public CommandExecutor
{
public:
   std::string command;
private:
   void execute(Ui::MainWindow* ui)
   {
      std::unique_lock<std::mutex> lock(mutex);
      command = ui->textEdit->currentText().toStdString();
      ready = true;
      cond_var.notify_all();
   }
};

class GetLineEndingRequest : public CommandExecutor
{
public:
   std::string line_ending;
private:
   void execute(Ui::MainWindow* ui)
   {
      std::unique_lock<std::mutex> lock(mutex);
      line_ending = ui->lineEndingComboBox->currentText().toStdString();
      ready = true;
      cond_var.notify_all();
   }
};

class GetTraceFilterRequest : public CommandExecutor
{
public:
   GetTraceFilterRequest(uint8_t id):
   id(id)
   {}
   uint8_t id;
   std::string trace_filter;
private:
   void execute(Ui::MainWindow* ui)
   {
      std::unique_lock<std::mutex> lock(mutex);
      auto filters = ui->getTraceFilters();
      UT_Assert(filters.size() > id);
      trace_filter = filters[id].line_edit->text().toStdString();
      ready = true;
      cond_var.notify_all();
   }
};

class GetUserButtonTabNameRequest : public CommandExecutor
{
public:
   GetUserButtonTabNameRequest(int index):
   index(index)
   {}
   int index;
   std::string tab_name;
private:
   void execute(Ui::MainWindow* ui)
   {
      std::unique_lock<std::mutex> lock(mutex);
      tab_name = "";
      if (ui->buttonsTabWidget->count() > index)
      {
         tab_name = ui->buttonsTabWidget->tabText(index).toStdString();
      }
      ready = true;
      cond_var.notify_all();
   }
};

class Stylesheet
{
public:
    enum class Item
    {
        BACKGROUND_COLOR,
        COLOR
    };

    Stylesheet()
    {

    }
    Stylesheet(const std::string& stylesheet)
    {
       m_colors_map[Item::BACKGROUND_COLOR] = 0;
       m_colors_map[Item::COLOR] = 0;
       m_stylesheet = stylesheet;
       parseStylesheet();
    }

    uint32_t getColor(Item type)
    {
       return m_colors_map[type] & 0xFFFFFF;
    }

    void setColor(Item type, uint32_t color)
    {
       m_colors_map[type] = color & 0xFFFFFF;
       createStylesheet();
    }

    const std::string& stylesheet() {return m_stylesheet;}

private:
    void createStylesheet()
    {
       char stylesheet [512];
       std::snprintf(stylesheet, 512, "background-color: #%.6x;color: #%.6x;", m_colors_map[Item::BACKGROUND_COLOR], m_colors_map[Item::COLOR]);
       m_stylesheet = std::string(stylesheet);
    }
    void parseStylesheet()
    {
       std::stringstream ss(m_stylesheet);
       std::string tag;
       while(std::getline(ss, tag, ';'))
       {
           std::string property = tag.substr(0, tag.find(':'));
           std::string value = tag.substr(tag.find('#') + 1, tag.size());
           uint32_t color = strtol(value.c_str(), NULL, 16);
           Item type = Item::BACKGROUND_COLOR;
           if (property == "background-color")
           {
               type = Item::BACKGROUND_COLOR;
           }
           else if (property == "color")
           {
               type = Item::COLOR;
           }
           m_colors_map[type] = color;
       }
   }
   std::string m_stylesheet;
   std::map<Item, uint32_t> m_colors_map;
};

}
