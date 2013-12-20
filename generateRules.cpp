/* This is a utility program I used to calculate the Boolean function that will
 * play Conway's Game of Life. It's basically included to show I didn't just
 * pull the number out of thin air by the power of my awesomeness.
 */

#include <iostream>
#include <sstream>
#include <string>
using namespace std;

#define DEBUG

int print (int t, bool lives)
{
   cout << "---" << endl;
   int i = 8;
   for (int c = 0; c < 3; ++c)
      cout << (bool)((0x1 << i--) & t);
   cout << '[' << lives << ']' << t << endl;

   for (int c = 0; c < 3; ++c)
      cout << (bool)((0x1 << i--) & t);
   cout << endl;

   for (int c = 0; c < 3; ++c)
      cout << (bool)((0x1 << i--) & t);
   cout << endl;
}

bool lives (int t)
{
   int count = 0;
   for (int i = 0; i < 9; ++i)
      count += (i == 4 ? 0 : (bool)((0x1 << i) & t));

   switch (count)
   {
      case 2:
         return (bool)((0x1 << 4) & t);

      case 3:
         return 1;

      default:
         return 0;
   }
}

int main()
{
   stringstream rev;
   for (int a = 0; a < 512; ++a)
   {
#ifdef DEBUG
      print (a, lives(a));
#else
      rev << lives(a);
#endif
   }

#ifndef DEBUG
   string temp;
   rev >> temp;
   cout << string(temp.rbegin(), temp.rend()) << endl;
#endif

   return 0;
}
