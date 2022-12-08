#pragma once
#include <functional>
#include <stdint.h>
#include <string>

enum class ButtonEvent
{
   CLICKED,
   CONTEXT_MENU_REQUESTED,
};

class IGUIController
{
public:
   virtual ~IGUIController(){};
   /**
    * @brief Returns the unique button ID basing on button name.
    * @param[in] name - name of the button
    * @return ID of the button if found.
    * @return If not button found, UINT32_MAX is returned
    */
   virtual uint32_t getButtonID(const std::string& name) = 0;
   /**
    * @brief Register callback to receive the buttons event.
    * @param[in] button_id - ID of the button
    * @param[in[ event - type of event
    * @param[in[ callback - function to be called
    * @return None
    */
   virtual void subscribeForButtonEvent(uint32_t button_id, ButtonEvent event, std::function<bool()> callback) = 0;
   /**
    * @brief Unregister callback.
    * @param[in] button_id - ID of the button
    * @param[in[ event - type of event
    * @param[in[ callback - function to be called
    * @return None
    */
   virtual void unsubscribeFromButtonEvent(uint32_t button_id, ButtonEvent event, std::function<bool()> callback) = 0;
   /**
    * @brief Sets button background color.
    * @param[in] button_id - ID of the button
    * @param[in[ color - color to be set
    * @return None
    */
   virtual void setButtonBackgroundColor(uint32_t button_id, uint32_t color) = 0;
   /**
    * @brief Sets button font color.
    * @param[in] button_id - ID of the button
    * @param[in[ color - color to be set
    * @return None
    */
   virtual void setButtonFontColor(uint32_t button_id, uint32_t color) = 0;
   /**
    * @brief Sets button checkable property. This allows to i.e. hold button pressed for longer tasks.
    * @param[in] button_id - ID of the button
    * @param[in[ bool - checkable value
    * @return None
    */
   virtual void setButtonCheckable(uint32_t button_id, bool checkable) = 0;
   /**
    * @brief Sets button check status. If checked, button is visible as pressed-down until checked==false is called.
    * @param[in] button_id - ID of the button
    * @param[in[ checked - check value
    * @return None
    */
   virtual void setButtonChecked(uint32_t button_id, bool checked) = 0;
   /**
    * @brief Controls the button state. If disable, no events will be delivered via respective callbacks.
    * @param[in] button_id - ID of the button
    * @param[in[ enable - enable value
    * @return None
    */
   virtual void setButtonEnabled(uint32_t button_id, bool enabled);
   /**
    * @brief Set the text visible on button.
    * @param[in] button_id - ID of the button
    * @param[in[ text - text to display
    * @return None
    */
   virtual void setButtonText(uint32_t button_id, const std::string& text);
   /**
    * @brief Removes all the items from terminal view.
    * @return None
    */
   virtual void clearTerminalView() = 0;
   /**
    * @brief Removes all the items from trace view.
    * @return None
    */
   virtual void clearTraceView() = 0;
   /**
    * @brief Adds text to terminal view.
    * @param[in] text - text to add
    * @param[in] background_color - text's background color
    * @param[in[ font_color - text's font color
    * @return None
    */
   virtual void addToTerminalView(const std::string& text, uint32_t background_color, uint32_t font_color) = 0;
   /**
    * @brief Adds text to trace view.
    * @param[in] text - text to add
    * @param[in] background_color - text's background color
    * @param[in[ font_color - text's font color
    * @return None
    */
   virtual void addToTraceView(const std::string& text, uint32_t background_color, uint32_t font_color) = 0;
   /**
    * @brief Returns number of already added items to terminal view.
    * @return Number of traces
    */
   virtual uint32_t countTerminalItems() = 0;
   /**
    * @brief Returns number of already added items to trace view.
    * @return Number of traces
    */
   virtual uint32_t countTraceItems() = 0;
   /**
    * @brief Controls terminal view scrolling.
    * @param[in] enabled - if true, the view is scrolled down after every item added
    * @return None
    */
   virtual void setTerminalScrollingEnabled(bool enabled);
   /**
    * @brief Controls trace view scrolling.
    * @param[in] enabled - if true, the view is scrolled down after every item added
    * @return None
    */
   virtual void setTraceScrollingEnabled(bool enabled);
   /**
    * @brief Registers callback to be called on active port change event.
    * @param[in] callback - function to be called.
    * @return None
    */
   virtual void subscribeForActivePortChangedEvent(std::function<bool(const std::string&)> callback) = 0;
   /**
    * @brief Registers new port opened.
    * @param[in] port_name - name of the port.
    * @return None
    */
   virtual void registerPortOpened(const std::string& port_name) = 0;
   /**
    * @brief Registers port close.
    * @param[in] port_name - name of the port.
    * @return None
    */
   virtual void registerPortClosed(const std::string& port_name) = 0;
   /**
    * @brief Sets the current command history. This can be called i.e. after active port change event.
    * @param[in] port_name - name of the port.
    * @return None
    */
   virtual void setCommandsHistory(const std::vector<std::string>& history) = 0;
   /**
    * @brief Returns current command entered by user.
    * @return Command as a text
    */
   virtual std::string getCurrentCommand() = 0;
   /**
    * @brief Returns current line ending choosen by user.
    * @return Command as a text
    */
   virtual std::string getCurrentLineEnding() = 0;
   /**
    * @brief Adds the new line ending to combobox.
    * @param[in] ending - new line ending to be added
    * @return None
    */
   virtual void addLineEnding(const std::string& ending) = 0;
   /**
    * @brief Returns the unique trace filter ID basing on line edit name.
    * @param[in] name - name of the filter
    * @return ID of the trace filter if found.
    * @return If no trace filter found, UINT32_MAX is returned
    */
   virtual uint32_t getTraceFilerID(const std::string& name) = 0;
   /**
    * @brief Sets the new filtering regular expression for given filter.
    * @param[in] id - ID of the filter
    * @return None
    */
   virtual void setTraceFilter(uint8_t id, const std::string& filter) = 0;
   /**
    * @brief Returns the filtering regular expression for given filter.
    * @param[in] id - ID of the filter
    * @return Regular expression as a string
    */
   virtual std::string getTraceFilter(uint8_t id);
   /**
    * @brief Controls editability of trace filters. If set to true, user is not able to edit the regular expression.
    * @param[in] id - ID of the filter
    * @param[in] enabled - filter status
    * @return None
    */
   virtual void setTraceFilterEnabled(uint8_t id, bool enabled) = 0;
   /**
    * @brief Sets the trace filter background color.
    * @param[in] id - ID of the filter
    * @param[in] color - background color
    * @return None
    */
   virtual void setTraceFilterBackgroundColor(uint32_t id, uint32_t color) = 0;
   /**
    * @brief Sets the trace filter font color.
    * @param[in] id - ID of the filter
    * @param[in] color - font color
    * @return None
    */
   virtual void setTraceFilterFontColor(uint32_t id, uint32_t color) = 0;
   /**
    * @brief Sets the text presented on label below the port.
    * @param[in] id - ID of the port
    * @param[in] description - text to be set
    * @return None
    */
   virtual void setPortLabelText(uint8_t id, const std::string& description) = 0;
   /**
    * @brief Sets the stylesheet of port label.
    * @param[in] id - ID of the port
    * @param[in] stylesheet - stylesheet as a text
    * @return None
    */
   virtual void setPortLabelStylesheet(uint8_t id, const std::string& stylesheet) = 0;

};
