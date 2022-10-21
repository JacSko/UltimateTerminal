#pragma once

#include <vector>
#include <mutex>
#include <functional>

template <typename LISTENER_TYPE>
class GenericListener
{
public:
   void addListener(LISTENER_TYPE& listener)
   {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_listeners.push_back(&listener);
   }

   void removeListener(LISTENER_TYPE& listener)
   {
      std::lock_guard<std::mutex> lock(m_mutex);
      auto it = std::find(m_listeners.begin(), m_listeners.end(), &listener);
      if (it != m_listeners.end())
      {
         m_listeners.erase(it);
      }
   }

protected:

   void notifyChange(std::function<void(LISTENER_TYPE*)> function)
   {
      std::lock_guard<std::mutex> lock(m_mutex);
      for (auto& listener : m_listeners)
      {
         if (listener) function(listener);
      }
   }

private:
   typedef std::vector<LISTENER_TYPE*> listenersContainter;

   listenersContainter m_listeners;
   typename listenersContainter::iterator m_iterator;
   std::mutex m_mutex;
};
