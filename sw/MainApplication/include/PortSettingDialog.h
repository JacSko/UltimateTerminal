#ifndef PORT_SETTING_DIALOG_H_
#define PORT_SETTING_DIALOG_H_

#include <stdint.h>
#include <string>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>

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
   PortSettingDialog(QWidget* parent);
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
      PortType type;
      std::string port_name;
      std::string device;
      uint32_t baud_rate;
      uint8_t data_bits;
      uint8_t parity_bits;
      uint8_t stop_bits;
      std::string ip_address;
      uint32_t port;
   };

   Settings showDialog(const Settings& current_settings);
private:

   void addPortTypeComboBox(PortType current_selection = PortType::SERIAL);
   void addDialogButtons();

   void renderSerialView(QDialog* dialog, QFormLayout* form, const Settings& settings = {});
   void renderEthernetView(QDialog* dialog, QFormLayout* form, const Settings& settings = {});

   void clearDialog();
   PortType stringToPortType(const QString& name);
   DataBits stringToDataBits(const QString& name);
   ParityBits stringToParityBits(const QString& name);
   StopBits stringToStopBits(const QString& name);


   QDialog m_dialog;
   QFormLayout m_form;
   std::vector<QWidget*> m_current_widgets;
public slots:
   void onPortTypeChanged(const QString& port_name);

};


#endif
