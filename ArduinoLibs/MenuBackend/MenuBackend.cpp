/* $Id: MenuBackend.cpp 1198 2011-06-14 21:08:27Z bhagman $
||
|| @author         Alexander Brevig <abrevig@wiring.org.co>
|| @url            http://wiring.org.co/
|| @contribution   Adrian Brzezinski
|| @contribution   Bernhard Benum
|| @contribution   Brett Hagman <bhagman@wiring.org.co>
||
|| @description
|| | Provides an easy way of making menus.
|| |
|| | Wiring Cross-platform Library
|| #
||
|| @license Please see cores/Common/License.txt.
||
*/

#include "MenuBackend.h"

void MenuItem::setValue(int v)
{
  if (v != value)
  {
    v = value;
    menuBackend->fireItemChangedEvent(this);
  }
}

int MenuItem::increment(int v)
{
  if (v != 0 && menuBackend)
  {
    value += v;
    menuBackend->fireItemChangedEvent(this);
  }
  return value;
}

int MenuItem::decrement(int v)
{
  if (v != 0 && menuBackend)
  {
    value -= v;
    menuBackend->fireItemChangedEvent(this);
  }
  return value;
}

