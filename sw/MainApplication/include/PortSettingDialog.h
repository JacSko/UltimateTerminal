#ifndef PORT_SETTING_DIALOG_H_
#define PORT_SETTING_DIALOG_H_

#include <stdint.h>
#include <string>
#include <optional>
#include <arpa/inet.h>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLineEdit>

#include "Logger.h"
#include "ISerialDriver.h"

/**
 * @file PortSettingDialogTests.cpp
 *
 * @brief
 *    Dialog class that allows user to enter the port connection details.
 *
 * @details
 *    Dialog contains multiple field, are created dynamically basing on port type (SERIAL, ETHERNET).
 *    Current settings are presented to the user, their editability is configurable using allow_edit argument during showDialog() call.
 *    All settings are validated when user accepts dialog box. When user rejects, no settings change takes place.
 *
 * @author Jacek Skowronek
 * @date   26/10/2022
 *
 */



#define DEF_PORT_TYPES     \
   DEF_PORT_TYPE(SERIAL)   \
   DEF_PORT_TYPE(ETHERNET) \

class PortSettingDialog : public QObject
{
   Q_OBJECT

public:
   PortSettingDialog();
   ~PortSettingDialog();

#undef DEF_PORT_TYPE
#define DEF_PORT_TYPE(name) name,
enum class PortType
{
   DEF_PORT_TYPES
   PORT_TYPE_MAX
};
#undef DEF_PORT_TYPE

   class Settings
   {
   public:
      Settings():
      type(PortType::SERIAL),
      port_name(""),
      serialSettings({}),
      ip_address("127.0.0.1"),
      port(1234),
      trace_color(0xFFFFFF)
      {}

      Settings(PortType type,
               const std::string& name,
               const Drivers::Serial::Settings& serial,
               const std::string& ip_address,
               uint32_t port,
               uint32_t trace_color):
      type(type),
      port_name(name),
      serialSettings(serial),
      ip_address(ip_address),
      port(port),
      trace_color(trace_color)
      {}

      EnumValue<PortType> type;
      std::string port_name;
      Drivers::Serial::Settings serialSettings;
      std::string ip_address;
      uint32_t port;
      uint32_t trace_color;

      std::string shortSettingsString() const
      {
         std::string result = "";
         if (type == PortType::SERIAL)
         {
            result += port_name + "/";
            result += serialSettings.device + "/";
            result += std::string("SER") + "/";
            result += serialSettings.baudRate.toName();
         }
         else
         {
            result += port_name + "/";
            result += std::string("ETH") + "/";
            result += ip_address + ":";
            result += std::to_string(port);
         }
         return result;
      }
      bool areValid()
      {
         bool result = true;
         m_error_strings.clear();
         if (type == PortType::ETHERNET)
         {
            result &= validateIpAddress(ip_address);
            result &= validatePort(port);
         }
         return result;
      }
      std::vector<std::string> getErrorStrings()
      {
         return m_error_strings;
      }
   private:
      bool validateIpAddress(const std::string& ip_address)
      {
         struct sockaddr_in sa;
         int result = inet_pton(AF_INET, ip_address.c_str(), &(sa.sin_addr));
         if (result == 0)
         {
            std::string error = "Invalid IP address: ";
            error += ip_address;
            m_error_strings.push_back(error);
         }
         return result != 0;
      }
      bool validatePort(uint32_t port)
      {
         if (port > MAX_IPPORT_VALUE)
         {
            std::string error = "Invalid IP port: ";
            error += std::to_string(port);
            m_error_strings.push_back(error);
         }
         return (port >= MIN_IPPORT_VALUE) && (port <= MAX_IPPORT_VALUE);
      }

      static const uint32_t MIN_IPPORT_VALUE = 1;
      static const uint32_t MAX_IPPORT_VALUE = 99999;
      std::vector<std::string> m_error_strings;
   };

   std::optional<bool> showDialog(QWidget* parent, const Settings& current_settings, Settings& out_settings, bool allow_edit);
private:

   void addPortTypeComboBox(const EnumValue<PortType>& current_selection);
   void addDialogButtons();
   void renderSerialView(QDialog* dialog, QFormLayout* form, const Settings& settings = {});
   void renderEthernetView(QDialog* dialog, QFormLayout* form, const Settings& settings = {});
   void clearDialog();
   bool convertGuiValues(Settings& out_settings);

   QDialog* m_dialog;
   QFormLayout* m_form;
   QComboBox* m_portTypeBox;
   QDialogButtonBox* m_buttonBox;

   Settings m_current_settings;

   QLineEdit* m_portNameEdit;
   QLineEdit* m_deviceNameEdit;
   QComboBox* m_baudRateBox;
   QComboBox* m_dataBitsBox;
   QComboBox* m_parityBitsBox;
   QComboBox* m_stopBitsBox;
   QLineEdit* m_ipAddressEdit;
   QLineEdit* m_ipPortEdit;
   QPushButton* m_colorSelectionButton;

   std::vector<QWidget*> m_current_widgets;
   bool m_editable;
public slots:
   void onPortTypeChanged(const QString& port_name);
   void onColorButtonClicked();

};


#endif
