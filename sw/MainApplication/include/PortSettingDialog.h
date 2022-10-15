#ifndef PORT_SETTING_DIALOG_H_
#define PORT_SETTING_DIALOG_H_

#include <stdint.h>
#include <string>
#include <optional>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLineEdit>

#include "Logger.h"

#define DEF_PORT_TYPES     \
   DEF_PORT_TYPE(SERIAL)   \
   DEF_PORT_TYPE(ETHERNET) \

#define DEF_DATA_BITS      \
   DEF_DATA_BIT(BITS5)     \
   DEF_DATA_BIT(BITS6)     \
   DEF_DATA_BIT(BITS7)     \
   DEF_DATA_BIT(BITS8)     \

#define DEF_PARITY_BITS    \
   DEF_PARITY_BIT(NONE)    \
   DEF_PARITY_BIT(EVEN)    \
   DEF_PARITY_BIT(ODD)     \
   DEF_PARITY_BIT(MARK)    \
   DEF_PARITY_BIT(SPACE)   \

#define DEF_STOP_BITS      \
   DEF_STOP_BIT(ONE)       \
   DEF_STOP_BIT(TWO)       \


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

#undef DEF_DATA_BIT
#define DEF_DATA_BIT(name) name,
enum class DataBits
{
   DEF_DATA_BITS
   DATA_BIT_MAX
};
#undef DEF_DATA_BIT

#undef DEF_PARITY_BIT
#define DEF_PARITY_BIT(name) name,
enum class ParityBits
{
   DEF_PARITY_BITS
   PARITY_BIT_MAX
};
#undef DEF_PARITY_BIT

#undef DEF_STOP_BIT
#define DEF_STOP_BIT(name) name,
enum class StopBits
{
   DEF_STOP_BITS
   STOP_BIT_MAX
};
#undef DEF_STOP_BIT

   struct Settings
   {
      Settings():
      type(PortType::SERIAL),
      port_name(""),
      device(""),
      baud_rate(0),
      data_bits(DataBits::BITS5),
      parity_bits(ParityBits::EVEN),
      stop_bits(StopBits::ONE),
      ip_address("127.0.0.1"),
      port(1234),
      trace_color(0xFFFFFF)
      {

      }

      PortType type;
      std::string port_name;
      std::string device;
      uint32_t baud_rate;
      DataBits data_bits;
      ParityBits parity_bits;
      StopBits stop_bits;
      std::string ip_address;
      uint32_t port;
      uint32_t trace_color;

      operator std::string () const
      {
         std::string result = "";
         result += "port name = " + port_name + "\n";
         result += "device = " + device + "\n";
         result += "type = " + toString(type) + "\n";
         result += "baud rate = " + std::to_string(baud_rate) + "\n";
         result += "data bits = " + toString(data_bits) + "\n";
         result += "parity bits = " + toString(parity_bits) + "\n";
         result += "stop bits = " + toString(stop_bits) + "\n";
         result += "ip address = " + ip_address + "\n";
         result += "ip port = " + std::to_string(port);
         return result;
      }

      std::string shortSettingsString()
      {
         std::string result = "";
         if (type == PortType::SERIAL)
         {
            result += port_name + "/";
            result += device + "/";
            result += std::string("SER") + "/";
            result += std::to_string(baud_rate);
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
   };

   std::optional<bool> showDialog(QWidget* parent, const Settings& current_settings, Settings& out_settings, bool allow_edit);
   static std::string toString(PortType);
   static std::string toString(DataBits);
   static std::string toString(ParityBits);
   static std::string toString(StopBits);
private:

   void addPortTypeComboBox(PortType current_selection);
   void addDialogButtons();
   void addItemsToComboBox(QComboBox* box, const std::vector<std::string>& values);
   void renderSerialView(QDialog* dialog, QFormLayout* form, const Settings& settings = {});
   void renderEthernetView(QDialog* dialog, QFormLayout* form, const Settings& settings = {});
   void clearDialog();
   bool convertGuiValues(Settings& out_settings);
   bool validateSettings(const Settings& settings);
   bool validateBaudRate(uint32_t baudrate);
   bool validateIpAddress(const std::string& ip_address);
   bool validatePort(uint32_t port);
   PortType stringToPortType(const QString& name);
   DataBits stringToDataBits(const QString& name);
   ParityBits stringToParityBits(const QString& name);
   StopBits stringToStopBits(const QString& name);

   QDialog* m_dialog;
   QFormLayout* m_form;
   QComboBox* m_portTypeBox;
   QDialogButtonBox* m_buttonBox;

   QLineEdit* m_portNameEdit;
   QLineEdit* m_deviceNameEdit;
   QLineEdit* m_baudRateEdit;
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
