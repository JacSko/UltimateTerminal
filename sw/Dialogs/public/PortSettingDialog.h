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
#include <QtWidgets/QLabel>

#include "ISerialDriver.h"

/**
 * @file PortSettingDialog.h
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
namespace Dialogs
{

#define DEF_PORT_TYPES     \
   DEF_PORT_TYPE(SERIAL)   \
   DEF_PORT_TYPE(ETHERNET) \
   DEF_PORT_TYPE(COMMAND) \

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

   /** Set of setting used to describe port connection */
   class Settings
   {
   public:

      #undef DEF_PORT_TYPE
      #define DEF_PORT_TYPE(a) #a,
      inline static const std::vector<std::string> port_names = { DEF_PORT_TYPES };
      #undef DEF_PORT_TYPE

      Settings():
      port_id(0),
      type(PortType::SERIAL),
      port_name(""),
      serialSettings({}),
      ip_address("127.0.0.1"),
      port(1234),
      command(""),
      trace_color(0xFFFFFF),
      font_color(0x000000)
      {}

      Settings(uint8_t id,
               PortType type,
               const std::string& name,
               const Drivers::Serial::Settings& serial,
               const std::string& ip_address,
               uint32_t port,
               const std::string& command,
               uint32_t trace_color,
               uint32_t font_color):
      port_id(id),
      type(type),
      port_name(name),
      serialSettings(serial),
      ip_address(ip_address),
      port(port),
      command(command),
      trace_color(trace_color),
      font_color(font_color)
      {}

      uint8_t port_id;                          /**< instance ID of the port        */
      Utilities::EnumValue<PortType> type;      /**< Type of the connection         */
      std::string port_name;                    /**< User name of the port          */
      Drivers::Serial::Settings serialSettings; /**< Settings for serial connection */
      std::string ip_address;                   /**< Server's IP address            */
      uint32_t port;                            /**< Server's port                  */
      std::string command;                      /**< Command to be executed         */
      uint32_t trace_color;                     /**< Color of traces that will be shown in terminal */
      uint32_t font_color;                      /**< Color of the font in terminal */

      std::string settingsString() const
      {
         std::string result = "";
         if (type == PortType::SERIAL)
         {
            result += port_name + "/";
            result += serialSettings.device + "/";
            result += std::string("SER") + "/";
            result += serialSettings.baudRate.toName();
         }
         else if (type == PortType::ETHERNET)
         {
            result += port_name + "/";
            result += std::string("ETH") + "/";
            result += ip_address + ":";
            result += std::to_string(port);
         }
         else
         {
            result += port_name + "/";
            result += command;
         }
         return result;
      }
      std::string summaryString() const
      {
         std::string result = "[" + std::to_string(port_id) + "]: ";
         result += preparePortSpecificLabel();
         return result;
      }
      bool operator== (const Settings& rhs)
      {
         return (type == rhs.type) &&
                (port_name == rhs.port_name) &&
                (serialSettings == rhs.serialSettings) &&
                (ip_address == rhs.ip_address) &&
                (port == rhs.port) &&
                (command== rhs.command) &&
                (trace_color == rhs.trace_color) &&
                (font_color == rhs.font_color);
      }
      bool operator!= (const Settings& rhs)
      {
         return !(*this == rhs);
      }
   private:
      std::string preparePortSpecificLabel() const
      {
         std::string result = "";
         using namespace Drivers::Serial;
         static const std::map<BaudRate, std::string> baudRatesMap = {{BaudRate::BR_1200, "1200"},
                                                                      {BaudRate::BR_2400, "2400"},
                                                                      {BaudRate::BR_4800, "4800"},
                                                                      {BaudRate::BR_9600, "9600"},
                                                                      {BaudRate::BR_19200, "19200"},
                                                                      {BaudRate::BR_38400, "38400"},
                                                                      {BaudRate::BR_57600, "57600"},
                                                                      {BaudRate::BR_115200, "115200"},
                                                                      {BaudRate::BAUDRATE_MAX, "INVALID"}};
         static const std::map<DataBitType, std::string> dataBitsMap = {{DataBitType::FIVE, "1200"},
                                                                        {DataBitType::SIX, "6"},
                                                                        {DataBitType::SEVEN, "7"},
                                                                        {DataBitType::EIGHT, "8"},
                                                                        {DataBitType::DATA_BIT_MAX, "INVALID"}};
         static const std::map<StopBitType, std::string> stopBitsMap = {{StopBitType::ONE, "1"},
                                                                        {StopBitType::TWO, "2"},
                                                                        {StopBitType::STOP_BIT_MAX, "INVALID"}};
         static const std::map<ParityType, std::string> parityBitsMap = {{ParityType::NONE, "n"},
                                                                         {ParityType::ODD, "o"},
                                                                         {ParityType::EVEN, "e"},
                                                                         {ParityType::PARITY_BIT_MAX, "INVALID"}};

         if (type == Dialogs::PortSettingDialog::PortType::SERIAL)
         {
            result += extractDeviceName(serialSettings.device);
            result += "/" + baudRatesMap.at(serialSettings.baudRate.value);
            result += "/" + dataBitsMap.at(serialSettings.dataBits.value);
            result += "/" + parityBitsMap.at(serialSettings.parityBits.value);
            result += "/" + stopBitsMap.at(serialSettings.stopBits.value);
         }
         else if (type == PortType::ETHERNET)
         {
            result += ip_address;
            result += ":";
            result += std::to_string(port);
         }
         else
         {
            result += command;
         }
         return result;
      }
      std::string extractDeviceName(const std::string& device) const
      {
         std::string result = "";
         auto it = device.find_last_of('/');
         if (it != std::string::npos)
         {
            result = device.substr(it+1);
         }
         return result;
      }
   };

   /**
    * @brief Opens a port connection settings dialog.
    * @param[in] parent - parent Widget
    * @param[in] current_settings - settings to be presented to user
    * @param[in] out_settings - new settings gathered from user
    * @param[in] allow_edit - set window editability
    *
    * @return Optional has value if user accepted the dialog, has no value if user rejected the dialog.
    * @return If value of optional is true, new settings are correct. Otherwise false.
    */
   std::optional<bool> showDialog(QWidget* parent, const Settings& current_settings, Settings& out_settings, bool allow_edit);
   /**
    * @brief Creates a layout related to dialog window that can be placed in another QWidget (i.e application settings).
    * @param[in] current_settings - settings to be presented to user
    * @param[in] allow_edit - set window editability
    *
    * @return Pointer to created Layout.
    */
   QLayout* createLayout(QWidget* parent, const Settings& current_settings, bool allow_edit);
   /**
    * @brief Destroys previously created layout.
    * @return None.
    */
   void destroyLayout();
   /**
    * @brief Converts the values from currently created layout. Method createLayout() has to be called prior to conversion.
    * @return True if gathered settings are correct, othwerwise false.
    * @return If settings are correct, out_settings are filled with the new data.
    */
   bool convertGuiValues(Settings& out_settings);

private:

   void addPortTypeComboBox(const Utilities::EnumValue<PortType>& current_selection);
   void addDialogButtons();
   void renderSerialView(QFormLayout* form, const Settings& settings = {});
   void renderEthernetView(QFormLayout* form, const Settings& settings = {});
   void renderCommandView(QFormLayout* form, const Settings& settings = {});
   void clearDialog();
   QWidget* m_parent;
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
   QLineEdit* m_commandEdit;
   QLineEdit* m_ipPortEdit;
   QPushButton* m_traceColorSelectionButton;
   QPushButton* m_fontColorSelectionButton;
   QLabel* m_info_label;

   std::vector<QWidget*> m_current_widgets;
   bool m_editable;
public slots:
   void onPortTypeChanged(const QString& port_name);
   void onBackgroundColorButtonClicked();
   void onFontColorButtonClicked();
};

}

template<>
inline std::string Utilities::EnumValue<Dialogs::PortSettingDialog::PortType>::toName() const
{
   if(value < Dialogs::PortSettingDialog::PortType::PORT_TYPE_MAX)
   {
      return Dialogs::PortSettingDialog::Settings::port_names[(size_t)value];
   }
   else
   {
      return "";
   }
}
template<>
inline Dialogs::PortSettingDialog::PortType Utilities::EnumValue<Dialogs::PortSettingDialog::PortType>::fromName(const std::string& name)
{
   value = Dialogs::PortSettingDialog::PortType::PORT_TYPE_MAX;
   auto it = std::find(Dialogs::PortSettingDialog::Settings::port_names.begin(), Dialogs::PortSettingDialog::Settings::port_names.end(), name);
   if (it != Dialogs::PortSettingDialog::Settings::port_names.end())
   {
      value = (Dialogs::PortSettingDialog::PortType)(std::distance(Dialogs::PortSettingDialog::Settings::port_names.begin(), it));
   }
   return value;
}


#endif
