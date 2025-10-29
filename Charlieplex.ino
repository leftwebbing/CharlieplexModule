#include <Arduino.h>

int numbers[] = {150, 100, 100, 100, 50, 50, 50, 50, 20, 20, 20}; // this works
int length = sizeof(numbers) / 2; // need to divide by 2
// for now use pins A1-A5 as outputs


// Define the characters and their corresponding binary values
const char characters[] = {
   '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
   'A', 'B', 'C', 'D', 'E', 'F', '-'
};

const uint16_t binaryValues[] = {
   0b1111110000000000, // '0'
   0b0110000000000000, // '1'
   0b1101101000000000, // '2'
   0b1111001000000000, // '3'
   0b0110011000000000, // '4'
   0b1011011000000000, // '5'
   0b1011111000000000, // '6'
   0b1110000000000000, // '7'
   0b1111111000000000, // '8'
   0b1111011000000000, // '9'
   0b1110111000000000, // 'A'
   0b0011111000000000, // 'B'
   0b1001110000000000, // 'C'
   0b0111101000000000, // 'D'
   0b1001111000000000, // 'E'
   0b1000111000000000, // 'F'
   0b0000001000000000  // '-'  // dont need space or period
};


// define segment-to-pin map
const uint8_t pinRows[] = {4, 3, 3, 2, 2, 2, 5, 4, 3, 1, 1, 1, 5, 4, 3, 2};
const uint8_t pinCols[] = {5, 5, 4, 5, 4, 3, 2, 2, 2, 4, 3, 2, 1, 1, 1, 1};


// Function to map characters to their corresponding binary values
uint16_t mapCharacterToBinary(char input) {
 Serial.print("char input "); Serial.println(input );
 for (int i = 0; i < sizeof(characters); i++) {
   if (characters[i] == input) {
     // Serial.print("bin output "); Serial.println(binaryValues[i], BIN); // debug only
     return binaryValues[i]; // Return the corresponding binary value
   }
 }
 return 0; // Return 0 for unmapped characters
}


// the setup function runs once when you press reset or power the board
void setup() {
 // initialize digital pin LED_BUILTIN as an output.
 pinMode(LED_BUILTIN, OUTPUT);


 pinMode(A1, INPUT);  // initialize pins to high impedance INPUT state
 pinMode(A2, INPUT);
 pinMode(A3, INPUT);
 pinMode(A4, INPUT);
 pinMode(A5, INPUT);


 Serial.begin(9600);
 Serial.print("# numbers in LED blink array ");
 Serial.println(length);
}


void printBits(unsigned int b) {  // pretty format for bytes
 int bits = sizeof(b) * 8; // length in bits (use local variable, don't clobber global 'length')
 Serial.print("input size in bits ");
 Serial.print(bits);
 Serial.print("   ");


 for(int i = bits-1; i >= 0; i--)  { // starts at MOST significant bit, proceeds in reverse order
   Serial.print(bitRead(b,i));
   if(i % 4 == 0) Serial.print(" "); // space in middle or fourths (set to 4)
 } 
 Serial.println();
}


// example sends "0 " to c-plex module, at left digit
uint8_t* exampleLeft() {
 static uint8_t myArray[5] = {0}; // static so it lives after function returns
 // LEAST significant bit = pin 1
 myArray[0] = 0b00000110;
 myArray[1] = 0b00000001;
 myArray[2] = 0b00000001;
 myArray[3] = 0b00000001;
 myArray[4] = 0b00000001;


 return myArray;
}


// example sends " 0" to c-plex module, at right digit
uint8_t* exampleRight() {
 static uint8_t myArray[5] = {0}; // static so it lives after function returns
 myArray[0] = 0b00000000;
 myArray[1] = 0b00011100;
 myArray[2] = 0b00001000;
 myArray[3] = 0b00000010;
 myArray[4] = 0b00000010;
  return myArray;
}


// accepts array of 5 unsigned ints which represent pin states, then sends to C-Plex
void writePins(const uint8_t *pins)
{
 // pinStates is an array of 5 ONLY bits 0-5 are pin states
 // pins A1-A5 are used, A1 is pinStates[0], A5 is pinStates[4]
 // for (int i = 0; i < 5; i++) { printBits(pins[i]);  }  // debug only
 // first safety check, avoid any positive diagonals
 for (int i = 0; i < 5; i++) {
   if (bitRead(pins[i], i) == 1) {
     Serial.print("oops L on diagonal ");  // debug only
     Serial.println(i);
     return;
   }
 }


 for (int n = 0; n < 50; n++) { // TODO optimize n
   for (int i = 0; i < 5; i++)  {    // for each byte, run a row
     pinMode(A1 + i, OUTPUT);    // set row pin to OUTPUT
     digitalWrite(A1 + i, HIGH); // turn row pin HIGH
     for (int b = 0; b < 5; b++) { // activate each relevant bit pin in row
       if (bitRead(pins[i], b) == 1) { // if bit is 1 set pin LOW
         pinMode(A1 + b, OUTPUT);
         digitalWrite(A1 + b, LOW);
       }
     }
     delay(5);  // let them shine for POV effect
     for (int b = 0; b < 5; b++)  { // deactovate each bit pin
       if (bitRead(pins[i], b) == 1) {  // if bit is 1 set pin LOW
         pinMode(A1 + b, INPUT); // restore to INPUT
       }
     }
     pinMode(A1 + i, INPUT); // deactivate row pin to INPUT
   }
 }
}


// receive input string and parse it into characters and decimal points
uint16_t charToSegs(const char* input){
 char SL = NULL;  // left symbol
 char SR = NULL;  // right symbol
 bool DL = false; // left decimal
 bool DR = false; // right decimal


 if (input == NULL) {
   Serial.println("charToSegs: null input");
 return 0;
 }


 // Walk the input and collect up to 2 symbols and up to 2 decimals.
 // Valid symbol characters: 0-9, A-F (uppercase), a-f (accept lowercase), and '-'
 int len = strlen(input);
 int symCount = 0; // number of symbol characters found (0..2)
 int dotCount = 0; // decimal points found (0..2)


 for (int i = 0; i < len && (symCount < 2 || dotCount < 2); ++i) {
   char c = input[i];


   if (c == '.') {
     // assign decimal to the next available slot: left decimal if left symbol exists and its decimal not set,
     // else right decimal.
     if (symCount == 0) {
       // No symbol yet: treat this as a left decimal with empty left symbol
       DL = true;
       dotCount++;
     } else if (symCount == 1) {
       // left symbol exists, assign DL if not set, otherwise DR
       if (!DL) { DL = true; }
       else { DR = true; }
       dotCount++;
     } else { // symCount == 2
       // two symbols already found, assign DR if empty else ignore
       if (!DR) { DR = true; dotCount++; }
     }
     continue;
   }


   // symbol characters: digits, letters A-F/a-f, or minus
   bool isSymbol = false;
   if ((c >= '0' && c <= '9') || c == '-' || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) isSymbol = true;

   if (isSymbol && symCount < 2) {
     if (symCount == 0) {
       SL = c;
     } else {
       SR = c;
     }
     symCount++;
     continue;
   }
   // Any other character: ignore for now
 }


 // Edge cases: input like ".5" -> no left symbol but decimal then symbol; current loop will set DL then SL.
 // If SL is still ' ' but DL true and SR set, move SR to SL (prefer left alignment)
 if (SL == ' ' && SR != ' ') {
   SL = SR;
   SR = ' ';
   bool tmp = DL; DL = DR; DR = tmp; // swap decimals to keep association with digits
 }


 // // Print parsed result for debugging
 // Serial.print("charToSegs input=\"");
 // Serial.print(input);
 // Serial.print("\": SL="); Serial.print(SL);
 // Serial.print(" DL="); Serial.print(DL ? "1" : "0");
 // Serial.print("  SR="); Serial.print(SR);
 // Serial.print(" DR="); Serial.println(DR ? "1" : "0");
  uint16_t mySegs = 0; // store overall segment map
 if (!SL == NULL) mySegs = mySegs |  mapCharacterToBinary(SL); // add left symbol 
 if (!SR == NULL) mySegs = mySegs | (mapCharacterToBinary(SR) >> 8); // add right symbol 
 if (DL) mySegs = mySegs | 0b0000000100000000; // add left decimal
 if (DR) mySegs = mySegs | 0b0000000000000001; // add right decimal
  // Serial.print("overall bits =");  printBits(mySegs); // debug only
 return mySegs;
}


// convert list of segments to pin states in uint8_t[5] array
uint8_t* segsToPins(uint16_t segs) {
 // Serial.print("segsToPins input "); Serial.println(segs, BIN);  // debug only
 // segment format 0bABCDEFGHABCDEFGH where ABCDEFG=segments; H=decimal points
 // return NULL; // debug only
 static uint8_t myArray[5]; // static so it lives after function returns
 // clear array each call
 for (int i = 0; i < 5; ++i) myArray[i] = 0;


 // myArray[] holds bitmasks for pins A1..A5, LSB=pin0
 // pinRows and pinCols are 1-based in the original mapping; convert to 0-based index and bitmask
 for (int i = 0; i <= 15; i++) { // iterate through segments
   if (bitRead(segs, i)) {
     int row = pinRows[i] - 1; // convert to 0-based index (A1 -> index 0)
     int col = pinCols[i] - 1; // convert to 0-based and use as bit index
     if (row >= 0 && row < 5 && col >= 0 && col < 8) {
       myArray[row] |= (1u << col);
     }
   }
 }


 // for (int i = 0; i < 5; i++) {// debug only
 //   Serial.print("myArray i="); Serial.print(i );
 //   Serial.print(" "); printBits(myArray[i]);
 // } 
 return myArray;
}


// Combined function: parse input string, map to segments, convert to pin masks and drive pins
void CPlex(const char* input) {
 if (input == NULL) return;
 uint16_t segs = charToSegs(input);
 uint8_t* pins = segsToPins(segs);
 writePins(pins);
}


void loop() { // loop runs forever
for (int x : numbers) {
   digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
   delay(x);                      // wait for x
   digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
   delay(x);                      // wait for x
   }

  // for (int i = length-1; i > 0; i--) { // backwards
//     digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
//     delay(numbers[i]);                      // wait for x
//     digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
//     delay(numbers[i]);                      // wait for x
//     }

// debug examples
 // CPlex("0.3");
 // delay(500);
 // writePins(segsToPins(charToSegs("0.3")));
 // delay(500);
 // CPlex("1.4.");
 // delay(500);
 // writePins(segsToPins(charToSegs("1.4.")));
 // delay(500);
 // CPlex("-.-.");
 // delay(500);
 // writePins(segsToPins(charToSegs("-.-.")));
 // delay(500);
 // CPlex("8. ");
 // delay(500);
 // writePins(segsToPins(charToSegs("8. ")));
 // delay(500);
 // CPlex("9.0.");
 // delay(500);
 // writePins(segsToPins(charToSegs("9.0.")));
 // delay(500);


 const char* messages[] = {"AB", "BC", "CD", "DE", "F", "12", "3.4", "5.6",
   "A", "B", "C", "D", "E", "1", "2", "3", "4", "0", "--", ". .", "0.-.", "-.0."};


 for (const auto& m : messages){
   CPlex(m);
 }


}


