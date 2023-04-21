
#include <gui/window.h>
using namespace::myos::common;
using namespace::myos::gui;

Window::Window(Widget *pParent, int32_t x, int32_t y, int32_t w, int32_t h, uint8_t r, uint8_t g, uint8_t b)
: CompositeWidget(pParent, x,y,w,h, r,g,b)
{
   isDragging = false;
}

Window::~Window()
{
}


void Window::OnMouseDown(int32_t x, int32_t y, uint8_t button)
{
   isDragging = (button == 1) ? 1 : 0;
   CompositeWidget::OnMouseDown(x, y, button);
}

void Window::OnMouseUp(int32_t x, int32_t y, uint8_t button)
{
   isDragging = false;
   CompositeWidget::OnMouseUp(x, y, button);
}

void Window::OnMouseMove(int32_t oldx, int32_t oldy, int32_t newx, int32_t newy)
{
   if(isDragging)
   {
      this->x += newx - oldx;
      this->y += newy - oldy;
   }
   CompositeWidget::OnMouseMove(oldx, oldy, newx, newy);
}

