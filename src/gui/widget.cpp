#include <gui/widget.h>

using namespace myos::common;
using namespace myos::gui;


Widget::Widget(Widget *pParent, int32_t x, int32_t y, int32_t w, int32_t h, uint8_t r, uint8_t g, uint8_t b)
: KeyboardEventHandler()
{
   this->pParent =  pParent;
   this->x = x;
   this->y = y;
   this->w = w;
   this->h = h;

   this->r = r;
   this->g = g;
   this->b = b;
}

Widget::~Widget()
{
}


void Widget::GetFocus(Widget *pWidget)
{
   if(pParent != 0)
      pParent->GetFocus(pWidget);      // doubt why ?
}

void Widget::ModelToScreen(int32_t &x, int32_t &y)
{
   if(pParent != 0)
      pParent->ModelToScreen(x, y);

   x += this->x;
   y += this->y;
}

void Widget::Draw(GraphicsContext *gc)
{
   int X = 0;
   int Y = 0;
   ModelToScreen(X, Y);
   gc->FillRectangle(X,Y,w,h, r,g,b);
}

void Widget::OnMouseDown(int32_t x, int32_t y, uint8_t button)
{
   if(isFocusable)
      GetFocus(this);
}


bool Widget::ContainsCoordinate(int32_t x, int32_t y)
{
   return this->x <= x && x < this->x + this->w
      &&  this->y <= y && y < this->y + this->h;
}


void Widget::OnMouseUp(int32_t x, int32_t y, uint8_t button)
{
}

void Widget::OnMouseMove(int32_t oldx, int32_t oldy, int32_t newx, int32_t newy)
{
}



















CompositeWidget::CompositeWidget(Widget *pParent, int32_t x, int32_t y, int32_t w, int32_t h, uint8_t r, uint8_t g, uint8_t b)
: Widget(pParent, x,y,w,h, r,g,b)
{
   pFocussedChild = 0;
   numChildren = 0;
}

CompositeWidget::~CompositeWidget()
{
}


void CompositeWidget::GetFocus(Widget *pWidget)
{
   this->pFocussedChild = pWidget;
   if(pParent != 0)
      pParent->GetFocus(this);
}

void CompositeWidget::Draw(GraphicsContext *gc)
{
   Widget::Draw(gc);
   for(int i = numChildren - 1; i >= 0; --i)
      pChildren[i]->Draw(gc);
}

bool CompositeWidget::AddChild(Widget *pChild)
{
   if(numChildren >= 100)
      return false;
   pChildren[numChildren++] = pChild;
   return true;
}

void CompositeWidget::OnMouseDown(int32_t x, int32_t y, uint8_t button)
{
   for(int i = 0; i < numChildren; ++i)
      if(pChildren[i]->ContainsCoordinate(x - this->x, y - this->y))
      {
         pChildren[i]->OnMouseDown(x - this->x, y - this->y, button);
      }
}

void CompositeWidget::OnMouseUp(int32_t x, int32_t y, uint8_t button)
{
   for(int i = 0; i < numChildren; ++i)
      if(pChildren[i]->ContainsCoordinate(x - this->x, y - this->y))
      {
         pChildren[i]->OnMouseUp(x - this->x, y - this->y, button);
      }
}

void CompositeWidget::OnMouseMove(int32_t oldx, int32_t oldy, int32_t newx, int32_t newy)
{
   int firstchild = -1;
   for(int i = 0; i < numChildren; ++i)
      if(pChildren[i]->ContainsCoordinate(oldx - this->x, oldy - this->y))
      {
         pChildren[i]->OnMouseMove(oldx - this->x, oldy - this->y, newx - this->x, newy - this->y);
         firstchild = i;
         break;
      }


   for(int i = 0; i < numChildren; ++i)
      if(pChildren[i]->ContainsCoordinate(newx - this->x, newy - this->y))
      {
         if(firstchild != i)
            pChildren[i]->OnMouseMove(oldx - this->x, oldy - this->y, newx - this->x, newy - this->y);
         break;
      }
}


void CompositeWidget::OnKeyDown(char ch)
{
   if(pFocussedChild != 0)
      pFocussedChild->OnKeyDown(ch);
}

void CompositeWidget::OnKeyUp(char ch)
{
   if(pFocussedChild != 0)
      pFocussedChild->OnKeyUp(ch);
}

