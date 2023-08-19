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
      Settings():
      port_id(0),
      type(PortType::SERIAL),
      port_name(""),
      serialSettings({}),
      ip_address("127.0.0.1"),
      port(1234),
      trace_color(0xFFFFFF),
      font_color(0x000000)
      {}

      Settings(uint8_t id,
               PortType type,
               const std::string& name,
               const Drivers::Serial::Settings& serial,
               const std::string& ip_address,
               uint32_t port,
               uint32_t trace_color,
               uint32_t font_color):
      port_id(id),
      type(type),
      port_name(name),
      serialSettings(serial),
      ip_address(ip_address),
      port(port),
      trace_color(trace_color),
      font_color(font_color)
      {}

      uint8_t port_id;                          /**< instance ID of the port        */
      EnumValue<PortType> type;                 /**< Type of the connection         */
      std::string port_name;                    /**< User name of the port          */
      Drivers::Serial::Settings serialSettings; /**< Settings for serial connection */
      std::string ip_address;                   /**< Server's IP address            */
      uint32_t port;                            /**< Server's port                  */
      uint32_t trace_color;                     /**< Color of traces that will be shown in terminal */
      uint32_t font_color;                      /**< Color of the font in terminal */

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
      bool operator== (const Settings& rhs)
      {
         return (type == rhs.type) &&
                (port_name == rhs.port_name) &&
                (serialSettings == rhs.serialSettings) &&
                (ip_address == rhs.ip_address) &&
                (port == rhs.port) &&
                (trace_color == rhs.trace_color) &&
                (font_color == rhs.font_color);
      }
      bool operator!= (const Settings& rhs)
      {
         return !(*this == rhs);
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

   void addPortTypeComboBox(const EnumValue<PortType>& current_selection);
   void addDialogButtons();
   void renderSerialView(QFormLayout* form, const Settings& settings = {});
   void renderEthernetView(QFormLayout* form, const Settings& settings = {});
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

#endif
