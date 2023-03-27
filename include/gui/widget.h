#ifndef __MYOS__GUI__WIDGET_H
#define __MYOS__GUI__WIDGET_H

#include <common/types.h>
#include <common/graphicscontext.h>
#include <drivers/keyboard.h>

namespace myos
{
   namespace gui
   {
      class Widget: public myos::drivers::KeyboardEventHandler
      {
         /*
            * GetFocus() for keybd key press/release... As keybd don't have any x, y coordinate
            * ModelToScreen() to map Widget relative index with Screen absolute index
            * ContainsCoordinate() to find current x,y index press is within widget or not
            * Focusable bool var to signify focus widget for keyboard inputs
          */
         protected:
            Widget* pParent;
            common::int32_t x;
            common::int32_t y;
            common::int32_t w;
            common::int32_t h;

            common::uint8_t r;
            common::uint8_t g;
            common::uint8_t b;
            bool isFocusable;

         public:
            Widget(Widget *pParent, common::int32_t x, common::int32_t y, common::int32_t w, common::int32_t h, common::uint8_t r, common::uint8_t g, common::uint8_t b);
            ~Widget();

            virtual void GetFocus(Widget *pWidget);
            virtual void ModelToScreen(common::int32_t &x, common::int32_t &y);
            virtual bool ContainsCoordinate(common::int32_t x, common::int32_t y);

            virtual void Draw(common::GraphicsContext *gc);
            virtual void OnMouseDown(common::int32_t x, common::int32_t y, common::uint8_t button);
            virtual void OnMouseUp(common::int32_t x, common::int32_t y, common::uint8_t button);
            virtual void OnMouseMove(common::int32_t oldx, common::int32_t oldy, common::int32_t newx, common::int32_t newy);
      };


      class CompositeWidget: public Widget
      {
         private:
            Widget *pChildren[100];
            int numChildren;
            Widget *pFocussedChild;
         public:
            CompositeWidget(Widget *pParent, common::int32_t x, common::int32_t y, common::int32_t w, common::int32_t h, common::uint8_t r, common::uint8_t g, common::uint8_t b);
            ~CompositeWidget();

            virtual void GetFocus(Widget *pWidget);
            virtual bool AddChild(Widget *pChild);

            virtual void Draw(common::GraphicsContext *gc);
            virtual void OnMouseDown(common::int32_t x, common::int32_t y, common::uint8_t button);
            virtual void OnMouseUp(common::int32_t x, common::int32_t y, common::uint8_t button);
            virtual void OnMouseMove(common::int32_t oldx, common::int32_t oldy, common::int32_t newx, common::int32_t newy);

            virtual void OnKeyDown(char);
            virtual void OnKeyUp(char);
      };
   }
}

#endif
