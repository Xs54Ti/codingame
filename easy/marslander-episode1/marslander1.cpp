#include <iostream>

using namespace std;

int main()
{
    int surfaceN, power = 0;
    cin >> surfaceN; cin.ignore();
    for (int i = 0; i < surfaceN; i++) {
        int landX, landY; 
        cin >> landX >> landY; cin.ignore();
    }
    while (1) {
        int X, Y, hSpeed, vSpeed, fuel, rotate, power; 
        cin >> X >> Y >> hSpeed >> vSpeed >> fuel >> rotate >> power; cin.ignore();

        if(vSpeed < -39) {
            power = 4;
        } else {
            power = 0;
        }
        cout << "0 " << power << endl;
    }
}
