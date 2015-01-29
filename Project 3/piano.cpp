#include <iostream>
#include <string>
#include <cctype>

using namespace std;

bool isTuneWellFormed (string tune) // determines if the tune is well formed; returns true if it is, returns false if it is not
{
    int length = tune.length(); // gives us length of tune

    if (length == 0) // checks if tune is simply zero beats
        return true;

    if (tune[length-1] != '/') // ensure that terminating character of tune is a / - mandatory due to requirement that all beats end with /
        return false;

    int i = 0; // will iterate through tune for us

    while (i < length) // will keep running until entire tune processed
    {
        char x = tune[i]; // this represents current character in the tune
        if (x == '/') // checks if current character is a /; if so, move on to next character
        {
                i++;
        }
        else if (x == 'A' || x == 'B' || x == 'C' || x == 'D' || x == 'E' || x == 'F' || x == 'G') // checks if character is one of possible piano keys
        {
            if (i+1 < length) // ensure we're not breaking the program by checking next character although last character test above should prevent from being a problem, just in case
            {
                x = tune[i+1]; // next character
                if (x == 'b' || x == '#') // sees if next input is accidental
                {
                    if (i+2 < length) // ensures again that program won't have runtime error by leaving string
                    {
                        x = tune[i+2]; // second note after letter
                        if (isdigit(x)) // checks if second character after key letter is relevant
                            i+=3; // assuming second after character was relevant and valid, adjusts so next check is after this key
                        else
                            i+=2; // if second character was not relevant, adjusts so program analyzing next possible key
                    }
                    else
                        i+=2; // will end loop
                }
                else if (isdigit(x)) // checks case of note followed by octave
                    i+=2;
                else
                    i++; // will end loop as we are at/near end
            }
            else // moves on to next key in string for analysis
                i++;
        }
        else // if character does not follow parameters of a well formed tune, return false
            return false;
    }

    return true; // as no objections to being well-formed occurred, return true
}

//*************************************
//  translateNote
//*************************************

// Given an octave number, a note letter, and an accidental sign, return
// the character that the indicated note translates to if it is playable.
// Return a space character if it is not playable.
//
// First parameter:   the octave number (the integer 4 is the number of the
//                    octave that starts with middle C, for example).
// Second parameter:  an upper case note letter, 'A' through 'G'
// Third parameter:   '#', 'b', or ' ' (meaning no accidental sign)
//
// Examples:  translateNote(4, 'A', ' ') returns 'Q'
//            translateNote(4, 'A', '#') returns '%'
//            translateNote(4, 'H', ' ') returns ' '

char translateNote(int octave, char noteLetter, char accidentalSign)
{
      // This check is here solely to report a common CS 31 student error.
    if (octave > 9)
    {
        cerr << "********** translateNote was called with first argument = "
             << octave << endl;
    }

      // Convert Cb, C, C#/Db, D, D#/Eb, ..., B, B#
      //      to -1, 0,   1,   2,   3, ...,  11, 12

    int note;
    switch (noteLetter)
    {
      case 'C':  note =  0; break;
      case 'D':  note =  2; break;
      case 'E':  note =  4; break;
      case 'F':  note =  5; break;
      case 'G':  note =  7; break;
      case 'A':  note =  9; break;
      case 'B':  note = 11; break;
      default:   return ' ';
    }
    switch (accidentalSign)
    {
      case '#':  note++; break;
      case 'b':  note--; break;
      case ' ':  break;
      default:   return ' ';
    }

      // Convert ..., A#1, B1, C2, C#2, D2, ... to
      //         ..., -2,  -1, 0,   1,  2, ...

    int sequenceNumber = 12 * (octave - 2) + note;

    string keymap = "Z1X2CV3B4N5M,6.7/A8S9D0FG!H@JK#L$Q%WE^R&TY*U(I)OP";
    if (sequenceNumber < 0  ||  sequenceNumber >= keymap.size())
        return ' ';
    return keymap[sequenceNumber];
}

int translateTune(string tune, string& instructions, int& badBeat) // accepts a tune and, if well-formed and playable, will set passed-by-reference "instructions" equal to the translated tune
{
    if(isTuneWellFormed(tune) == false) // ensuring tune is well formed
        return 1;

    if (tune == "") // special case check
    {
        instructions = "";
        return 0;
    }

    string translatedTune = ""; // will hold translated tune until finished; used to account for fact that instructions should be unchanged unless tune was playable
    int i = 0; // tracks position in string
    int numNotesInChord = 0; // used for determining bracket usage
    string tempHolder = ""; // will hold translated notes in case brackets needed
    char currentPlace; // represents char at current string position given by i above
    int currentBeat = 1; // tracks which beat the translation is currently occurring in - used for purposes of setting badBeat if tune is not playable

    while (i < tune.length()) // will keep running through tune until out of bounds
    {
        currentPlace = tune[i];
        if (currentPlace == '/') // checks first instance - current character being checked is a /
        {
            if (numNotesInChord == 0)
            {
                translatedTune+= ' ';
            }
            else if (numNotesInChord > 1) // if more than 1 note was present, adds them to temporary translation-holder with proper brackets
            {
                translatedTune = translatedTune + "[" + tempHolder + "]";
            }
            else if (numNotesInChord == 1) // if only 1 note was present, does not bracket anything
            {
                translatedTune += tempHolder;
            }
            i++; // cycles on to next step in tune
            tempHolder = ""; // resets string holding temp translations as it has been used
            numNotesInChord = 0; // resets notes in chord tracker, used for bracketing case
            currentBeat++; // as each slash indicates a 1 beat increase, cycle it forward for badBeat purposes
        }
        else // no need for else-if as we know tune is well-formed
        {
            char noteLetter = currentPlace;
            char accidental = ' '; // default case - no accidental
            int octave = 4; // default case - octave 4
            if (i+1 < tune.length() && (tune[i+1] == 'b' || tune[i+1] == '#')) // checks for accidental presence
            {
                accidental = tune[i+1]; // eliminates default no-accidental space
                if (i+2 < tune.length() && (isdigit(tune[i+2]))) // checks for an octave digit and handles/cycles as needed
                {
                    octave = tune[i+2] - '0';
                    i+=3;
                }
                else // if no digit, keeps default 4th octave and moves on
                    i+=2;
            }
            else if (i+1 < tune.length() && isdigit(tune[i+1])) // checks for note-octave case, changes octave value from default, cycles as needed
            {
                octave = tune[i+1] - '0';
                i+=2;
            }
            else // for note-only case; keeps default 4th octave
                i++;


            char note = translateNote(octave, noteLetter, accidental); // calls translateNote to get the ButtonBass keyboard note corresponding
            if (note == ' ') // checks for non-playability; sets badBeat to current beat (aka where first non-playable note was) and returns proper value
            {
                badBeat = currentBeat;
                return 2;
            }
            tempHolder = tempHolder + note; // if note was playable, add it to what is temporarily containing our notes in case brackets needed
            numNotesInChord++; // since this was a note within a chord, increases the tracker to help account for bracketing need
        }
    }
    instructions = translatedTune; // since there was no playability concern, fixes instructions to translation
    return 0; // returns proper value for a playable tune
}

int main()
{
    cout << "Enter a tune: ";
    string tune;
    getline(cin, tune);

    string translated = "String did not translate";
    int blah = 0;

    int x = translateTune(tune, translated, blah);
    cout << translated << endl;

    return 0;
}
