#pragma once

#include "gmock/gmock.h"

class QString
{

};

class QWidget
{

};

class QPushButton
{
public:
   virtual ~QPushButton(){};
   virtual void setCheckable(bool) = 0;
   virtual void setChecked(bool) = 0;
   virtual void repaint() = 0;
   virtual void setText(const QString&) = 0;
};


class QPushButtonMock : public QPushButton
{
   MOCK_METHOD1(setCheckable, void(bool));
   MOCK_METHOD1(setChecked, void(bool));
   MOCK_METHOD0(repaint, void());
   MOCK_METHOD1(setText, void(const QString&));
};
