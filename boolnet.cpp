/* Note: A large part of this is scaffolding provided as part of the assignment,
 * so much of this code can be attributed to Rick Neff, professor at BYU-Idaho.
 *
 * Departures from the starting code are individually commented.
 */

#include <cstdlib>
#include <iostream>
#include <cstdarg>
#include <sstream>
#include <cmath>
#include <ctime>
using namespace std;

#define DEFAULT_CELLS 100
#define DEFAULT_STEPS numCells
//#define LOOP_EDGES
//#define TERM
#define BARRIER_VALUE 0

const bool prettyPrint = true;
#ifdef TERM
const char * pretty1 = "\E[44m \E[0m";
#else
const char * pretty1 = "x";
#endif
const char * pretty0 = ".";


/*******************************************************************
 * A class that represents a Wire with a current and a new boolean
 * value.  The new value is set by the operation of a Cell, and it
 * will become the current value after being updated, which typically
 * happens right before showing (outputting) the value).
 *******************************************************************/
class Wire
{
private:
   bool mValue;
   bool mNewValue;

public:
   /****************************************************************
    * Constructs a new wire instance with a default value (false);
    ****************************************************************/
   Wire()
   {
      setValue(false);
      setNewValue(false);
   }

   /****************************************************************
    * Constructs a new wire instance with a given value.
    ****************************************************************/
   Wire(bool pValue)
   {
      setValue(pValue);
      setNewValue(false);
   }

   /******************************
    * Gets this wire's value.
    *******************************/
   bool getValue() const { return mValue; }

   /******************************
    * Gets this wire's new value.
    *******************************/
   bool getNewValue() const { return mNewValue; }

   /******************************
    * Sets this wire's value
    *******************************/
   void setValue(bool pValue) { mValue = pValue; }

   /******************************
    * Sets this wire's new value
    *******************************/
   void setNewValue(bool pNewValue) { mNewValue = pNewValue; }

   /******************************
    * Sets this wire's value to its new value.
    *******************************/
   void setValueToNew() { mValue = mNewValue; }

   /****************************************************
    * Gets the character representing this wire's value.
    ****************************************************/
   const char * getCharacterRepresentingValue() const
   {
      return
         ((prettyPrint) ?
          (getValue() ? pretty1 : pretty0) :
          (getValue() ? "1" : "0"));
   }
};

/****************************************************************
 * Outputs a Wire.
 ****************************************************************/
ostream& operator<<(ostream& os, const Wire& w)
{
   os << w.getCharacterRepresentingValue();
   return os;
}

/****************************************************************
 * A base class representing a Boolean function of arbitrary degree.
 ****************************************************************/
class BooleanFunction
{
protected:
   // If we are going to have a large number of these, storing the
   // representation as integers is more efficent. Also: less work in most
   // cases, especially as we are being passed the value as an integer anyway
   int mValues;
   int mValuesLen;
   int degree;

public:
   BooleanFunction(int deg, int values = 0) : mValues(values), mValuesLen(0), degree(deg)
   {
      mValuesLen = pow (2, degree);
      //test();
   }

   // Note: p must point to an array of length degree
   bool evaluate(bool * p)
   {
      // This builds our index, basically converting our inputs into an
      // integer as if they were the binary representation of the index of an
      // array.
      int index = 0;
      int mask = 0x1;

      for (int i = 0; i < degree; ++i)
      {
         if (p[i])
            index |= mask;
         mask <<= 1;
      }
      // Generates the mask and grabs the appropriate value from our bit
      // string, and converts this into a bool representing that bit.
      return (bool)(mValues & (0x1 << index));
   }

   // Much simpler
   void populate(int values)
   {
      mValues = values;
      //test();
   }

   void test()
   {
      int mask = 0x1 << (mValuesLen - 1);
      for (int i = 0; i < mValuesLen; ++i)
      {
         cout << (bool)(mValues & mask);
         mask >>= 1;
      }
      cout << endl;
   }

   int getDegree()
   {
      return degree;
   }
};

/****************************************************************
 * A Cell implementing a boolean function of arbitrary degree
 * (How many of them are there?)
 ****************************************************************/
class Cell
{
protected:
   BooleanFunction* mBooleanFunction;

public:
   /****************************************************************
    * Changes were made here to allow working with an arbitrary number of
    * wires. Primarily it was set up to facilitate simpler variable passing
    * between this and the BooleanFunction. It will also reduce errors caused
    * by mis-judging the output wire by maping that to an exclusive
    * input. Finally the wires are now passed as pointers, to comply with stdarg
    *
    * Constructs a new Cell instance with an arbitrary number of wires
    * and a pointer to a Boolean Function of arbitrary degree.
    *
    * This merely has the side effect of computing the boolean
    * function on the wire inputs, storing the result in the
    * first (output) wire
    ****************************************************************/
   Cell(BooleanFunction* pBooleanFunction, Wire & output, ...)
   {
      setBooleanFunction(pBooleanFunction);

      // These feed all of our input wire values into an array of bools for
      // the boolean function to use
      int numWires = pBooleanFunction->getDegree();
      bool * wireVals = new bool [numWires];

      va_list ap;
      va_start (ap, output);
      for (int i = 0; i < numWires; ++i)
      {
         Wire * argWire = va_arg (ap, Wire *);
         wireVals[i] = argWire->getValue();
      }

      output.setNewValue(getBooleanFunction()->evaluate(wireVals));
   }

   /****************************************************************
    * Gets the pointer to the Boolean Function of arbitrary degree.
    ****************************************************************/
   BooleanFunction* getBooleanFunction()
   {
      return mBooleanFunction;
   }

   /****************************************************************
    * Sets the pointer to the Boolean Function of arbitrary degree.
    ****************************************************************/
   void setBooleanFunction(BooleanFunction* pBooleanFunction)
   {
      mBooleanFunction = pBooleanFunction;
   }
};

void runSimulation (int boolFunction, int numCells, int numSteps, bool randomStart)
{
   cout << "F:" << boolFunction
        << " on " << numCells << " cells for " << numSteps
        << " generations with " << (randomStart ? "random" : "fixed") << " initial conditions\n";

   // TODO: create numCells Wires.
   Wire * wires = new Wire[numCells];

   // TODO: set to true the new value of the middle-most Wire
   //       (the middle of the whole array, *NOT* the middle wire of each cell).
   if (randomStart)
   {
      srand(time(NULL));
      int randSrc = rand();
      for (int i = 0; i < numCells; ++i)
      {
         wires[i].setValue ( (randSrc >>= 1) & 0x1);
         if (!randSrc)
            randSrc = rand();
      }
   }
   else
      wires[numCells/2].setValue(1);

   // TODO: create a BooleanFunction given the booleanFunctionNumber.
   BooleanFunction rule(3, boolFunction);

#ifndef LOOP_EDGES
      wires[0].setValue(BARRIER_VALUE);
      wires[numCells - 1].setValue(BARRIER_VALUE);
#endif

   // TODO: update values to new values and show (output) all
   // Wires. (reversing these so I can see the initial conditions)
   for (int i = 0; i < numCells; ++i)
      cout << wires[i];
   cout << endl;

   // TODO: iterate A and B for numSteps - 1 steps (initial condition is
   // considered a step).
   for (int step = 0; step < numSteps - 1; ++step)
   {
      // TODO: A) create the network of Cells implementing the BooleanFunction
      //            and connect them to the numCells Wires, as per the write-up.

#ifdef LOOP_EDGES
      for (int i = 0; i < numCells; ++i)
         Cell(&rule, wires[i],
              wires+((i+1)%numCells),
              wires+i,
              (i ? wires+(i-1) : wires + (numCells - 1)));
#else
      wires[0].setNewValue(BARRIER_VALUE);
      wires[numCells - 1].setNewValue(BARRIER_VALUE);

      for (int i = 1; i < numCells - 1; ++i)
         Cell(&rule, wires[i],
              wires+i+1,
              wires+i,
              wires+i-1);
#endif

      // TODO: B) update values to new values and show (output) all wires.
      // Convert the values and print
      for (int i = 0; i < numCells; ++i)
      {
         wires[i].setValueToNew();
         cout << wires[i];
      }
      cout << endl;
   }

   delete wires;
}

/****************************************************************
 * Main is for you to explore...
 ****************************************************************/
int main(int argc, char* argv[])
{
   // TODO: get program input another (better?) way.
   // one possible way to get input (actually pretty good, certainly scriptable)

   if (argc <= 1)
   {
      cerr << "Usage: " << argv[0] << " function(0-255) <width> <steps> <random 0|1>" << endl
           << "or" << endl
           << "Input a series of commands, one per line, ending with Ctrl-D" << endl;
   }

   // Variables are common between the cases
   int booleanFunctionNumber;
   //Fairly sane defaults
   int numCells = DEFAULT_CELLS;
   int numSteps = DEFAULT_STEPS;
   int randomStart = 0;

   // Handling cli input
   if (argc > 1)
   {
      booleanFunctionNumber = atoi(argv[1]);

      if (argc > 2)
         numCells = atoi(argv[2]);

      if (argc > 3)
         numSteps = atoi(argv[3]);

      if (argc > 4)
         randomStart = atoi(argv[4]);

      runSimulation (booleanFunctionNumber, numCells, numSteps, randomStart);
   }
   // Handle input from terminal
   else
   {
      while (cin.good())
      {
         // Pass a line from cin to a string stream for parsing
         string temp;
         getline(cin, temp);

         stringstream ss (temp);

         // If no number, ignore this line
         ss >> booleanFunctionNumber;
         if (ss.fail()) continue;

         // I noticed a tendency to put an extra space after the input, so I
         // am handling this by ignoring it before each existance test on the
         // stream
         while (isspace(ss.peek())) ss.ignore();

         // If no cells, use default. If bad input, ignore this line
         if (ss.eof())
            numCells = DEFAULT_CELLS;
         else
            ss >> numCells;
         if (ss.fail()) continue;

         if (!ss.eof())
            while (isspace(ss.peek())) ss.ignore();

         // If no steps, use default. If bad input, ignore this line
         if (ss.eof())
            numSteps = DEFAULT_STEPS;
         else
            ss >> numSteps;
         if (ss.fail()) continue;

         if (!ss.eof())
            while (isspace(ss.peek())) ss.ignore();

         // If not specified use single cell start. If bad input, ignore this line
         if (ss.eof())
            randomStart = 0;
         else
            ss >> randomStart;
         if (ss.fail()) continue;

         runSimulation (booleanFunctionNumber, numCells, numSteps, randomStart);
      }
   }

   return 0;
}
