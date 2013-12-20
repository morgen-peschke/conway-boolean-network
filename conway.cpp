/* This shares much with the 1D version, so some parts are still attributable to
 * Rick Neff. The degree of departure is much higher, so look to boolnet.cpp to
 * see what was originally there.
 */

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdarg>
using namespace std;

#define DEFAULT_CELLS_ROW 10
#define DEFAULT_CELLS_COL 10
#define DEFAULT_STEPS 10
#define LOOP_EDGES
#define TERM
#define BARRIER_VALUE 0

const bool prettyPrint = true;
#ifdef TERM
const char * pretty1 = "\E[44m \E[0m";
const char * pretty0 = " ";
#else
const char * pretty1 = "#";
const char * pretty0 = ".";
#endif


// Logic string for conways simple rules
const char * gameLogic =
   "00000001000101100001011101111110000101100110100001111110111010000001011001101000011111101110100001101000100000001110100010000000000101100110100001111110111010000110100010000000111010001000000001101000100000001110100010000000100000000000000010000000000000000001011001101000011111101110100001101000100000001110100010000000011010001000000011101000100000001000000000000000100000000000000001101000100000001110100010000000100000000000000010000000000000001000000000000000100000000000000000000000000000000000000000000000";

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
   bool * mValues;
   int mValuesLen;
   int degree;

public:
   BooleanFunction(int deg)
   {
      degree = deg;
      mValues = 0;
      mValuesLen = 0;
   }

   ~BooleanFunction()
   {
      if (mValues)
         delete mValues;
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
      return mValues [index];
   }

   void populate(string values)
   {
      mValuesLen = values.length();
      if (mValues)
         delete mValues;

      mValues = new bool [mValuesLen];
      for (int i = 0; i < mValuesLen; ++i)
         mValues[i] = (values[i] == '1');
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
      va_end(ap);
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

void runSimulation (string edenFile, int numCellsRow, int numCellsCol, int numSteps)
{
   cerr << "F:" << edenFile
        << " on " << numCellsRow << "x" << numCellsCol
        << " cells for ";
   if (~numSteps)
      cerr << numSteps;
   else
      cerr << "unlimited";

   cerr << " generations\n";

   // TODO: create numCells Wires.
   Wire ** wires = new Wire * [numCellsRow];

   for (int i = 0; i < numCellsRow; ++i)
      wires[i] = new Wire[numCellsCol];

   ifstream eden (edenFile.c_str());
   if (eden.fail())
   {
      cerr << "Unable to open start file\n";
      return;
   }

   {
      int x = 0;
      int y = 0;
      string tempS;
      char tempC;
      // I'm doing this this way so that I can accurately detect the end of
      // line. This lets me only fill out part of an eden file if I have a
      // smaller initial condition. I can fill out each line only as far as the
      // last cell I want to mark as live.
      while (eden.good() && x < numCellsRow)
      {
         getline (eden, tempS);
         stringstream ss (tempS);
         while (ss.good() && y < numCellsCol)
         {
            ss >> tempC;
            if (tempC == 'x')
               wires[x][y].setValue(1);
            ++y;
         }
         y = 0;
         ++x;
      }
   }
   eden.close();

   BooleanFunction rule(9);
   rule.populate(gameLogic);

#ifndef LOOP_EDGES
   for (int i = 0; i < numCellsRow; ++i)
   {
      wires[0][i].setValue(BARRIER_VALUE);
      wires[numCellsCol - 1][i].setValue(BARRIER_VALUE);
   }
   for (int i = 0; i < numCellsCol; ++i)
   {
      wires[i][0].setValue(BARRIER_VALUE);
      wires[i][numCellsRow - 1].setValue(BARRIER_VALUE);
   }
#endif

   // Print out the initial board
   for (int x = 0; x < numCellsRow; ++x)
   {
      for (int y = 0; y < numCellsCol; ++y)
         cout << wires[x][y];
      cout << endl;
   }

   for (int step = 0; (!~numSteps) || step < numSteps; ++step)
   {
      // TODO: A) create the network of Cells implementing the BooleanFunction
      //            and connect them to the numCells Wires, as per the write-up.

#ifdef LOOP_EDGES
      for (int x = 0; x < numCellsRow; ++x)
         for (int y = 0; y < numCellsCol; ++y)
            Cell(&rule, wires[x][y],
                 &wires[x ? x-1 : numCellsRow - 1][y ? y-1 : numCellsCol - 1],
                 &wires[x ? x-1 : numCellsRow - 1][y],
                 &wires[x ? x-1 : numCellsRow - 1][(y+1) % numCellsCol],
                 &wires[x][y ? y-1 : numCellsCol - 1],
                 &wires[x][y],
                 &wires[x][(y+1) % numCellsCol],
                 &wires[(x+1) % numCellsRow][y ? y-1 : numCellsCol - 1],
                 &wires[(x+1) % numCellsRow][y],
                 &wires[(x+1) % numCellsRow][(y+1) % numCellsCol]);
#else
      for (int i = 0; i < numCellsRow; ++i)
      {
         wires[0][i].setNewValue(BARRIER_VALUE);
         wires[numCellsCol - 1][i].setNewValue(BARRIER_VALUE);
      }
      for (int i = 0; i < numCellsCol; ++i)
      {
         wires[i][0].setNewValue(BARRIER_VALUE);
         wires[i][numCellsRow - 1].setNewValue(BARRIER_VALUE);
      }

      for (int x = 1; x < numCellsRow - 1; ++x)
         for (int y = 1; y < numCellsCol - 1; ++y)
            Cell(&rule, wires[x][y],
                 &wires[x+1][y+1],
                 &wires[x+1][y],
                 &wires[x+1][y-1],
                 &wires[x][y+1],
                 &wires[x][y],
                 &wires[x][y-1],
                 &wires[x-1][y+1],
                 &wires[x-1][y],
                 &wires[x-1][y-1]);
#endif

      // TODO: B) update values to new values and show (output) all wires.
      // Convert the values and print
      for (int x = 0; x < numCellsRow; ++x)
      {
         for (int y = 0; y < numCellsCol; ++y)
         {
            wires[x][y].setValueToNew();
            cout << wires[x][y];
         }
         cout << endl;
      }
   }

   for (int i = 0; i < numCellsRow; ++i)
      delete wires[i];
   delete wires;
}

/****************************************************************
 * Main is for you to explore...
 ****************************************************************/
int main(int argc, char* argv[])
{
   if (argc <= 1)
   {
      cout << "Usage: " << argv[0] << " function [numCells row] [numCells col] [numSteps]" << endl
           << "or" << endl
           << "Input a series of commands, one per line, ending with Ctrl-D" << endl;
   }

   // Variables are common between the cases
   string sourceFile;
   //Fairly sane defaults
   int numCellsRow = DEFAULT_CELLS_ROW;
   int numCellsCol = DEFAULT_CELLS_COL;
   int numSteps = DEFAULT_STEPS;

   // Handling cli input
   if (argc > 1)
   {
      sourceFile = argv[1];

      if (argc > 2)
         numCellsRow = atoi(argv[2]);

      if (argc > 3)
         numCellsCol = atoi(argv[3]);

      if (argc > 4)
         numSteps = atoi(argv[4]);

      runSimulation (sourceFile, numCellsRow, numCellsCol, numSteps);
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
         ss >> sourceFile;
         if (ss.fail()) continue;

         // I noticed a tendency to put an extra space after the input, so I
         // am handling this by ignoring it before each existance test on the
         // stream
         while (isspace(ss.peek())) ss.ignore();

         // If no cells, use default. If bad input, ignore this line
         if (ss.eof())
            numCellsRow = DEFAULT_CELLS_ROW;
         else
            ss >> numCellsRow;
         if (ss.fail()) continue;

         if (!ss.eof())
            while (isspace(ss.peek())) ss.ignore();

         // If no cells, use default. If bad input, ignore this line
         if (ss.eof())
            numCellsCol = DEFAULT_CELLS_COL;
         else
            ss >> numCellsCol;
         if (ss.fail()) continue;

         if (!ss.eof())
            while (isspace(ss.peek())) ss.ignore();

         // If no steps, use default. If bad input, ignore this line
         if (ss.eof())
            numSteps = DEFAULT_STEPS;
         else
            ss >> numSteps;
         if (ss.fail()) continue;

         runSimulation (sourceFile, numCellsRow, numCellsCol, numSteps);
      }
   }

   return 0;
}
