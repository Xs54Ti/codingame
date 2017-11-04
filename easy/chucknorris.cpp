#include <iostream>
#include <string>

using namespace std;

int main()
{
    string MESSAGE;
    getline(cin, MESSAGE);
    
    //transform Message to Binary Message
    string binaryMESSAGE = "", binaryLetter = "", unaryMessage = "";
    int letterInNumber;
    for(int i = 0; i < MESSAGE.size(); i++) {
        letterInNumber = MESSAGE[i];
        binaryLetter = "";
        while(letterInNumber != 0) {
            binaryLetter = to_string(letterInNumber % 2) + binaryLetter;
            letterInNumber = letterInNumber / 2;
        }
        while(binaryLetter.size() != 7) {
            binaryLetter = "0" + binaryLetter;
        }
        binaryMESSAGE = binaryMESSAGE + binaryLetter;
    }
    
    //transform Binary Message to Unary Message
    char currentBitSerie = '2';
    int currentNumberSerie = 0;
    for(int i = 0; i < binaryMESSAGE.size(); i++) {
        if(binaryMESSAGE[i] != currentBitSerie) {
            for(int j = 0; j < currentNumberSerie; j++) {
                unaryMessage = unaryMessage + '0';
            }
            currentBitSerie = binaryMESSAGE[i];
            if(currentBitSerie == '0') {
                unaryMessage = unaryMessage + " 00 ";
            } else {
                unaryMessage = unaryMessage + " 0 ";
            }
            currentNumberSerie = 1;
        }  else {
            currentNumberSerie++;
        }
        
        if(i == binaryMESSAGE.size() - 1) {
            for(int j = 0; j < currentNumberSerie; j++) {
                unaryMessage = unaryMessage + '0';
            }
        }
    }
    cout << unaryMessage.substr(1) << endl; //remove first char of the string (space)
}
