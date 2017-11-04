#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct square { // one square equal one point on the map
	char type; 
    vector<int> visited; // save loopRound in which the square is visited by Bender
    vector<string> roadSinceLastVisited; // save road between two visits, usefull if there is a loop
};

int main()
{
    int L, C;
    cin >> L >> C; cin.ignore();
    square map[L][C]; // map of the city
    int xPosition = -1, yPosition = -1, xEnd = -1, yEnd = -1, xTeleport1 = -1, yTeleport1 = -1, xTeleport2 = -1,
            yTeleport2 = -1; // variables needed for Bender position, the end point and teleporters
    for (int i = 0; i < L; i++) {
        string row;
        getline(cin, row);
        for (int j = 0; j < C; j++) {
            map[i][j].type = row[j]; // fill the map
            if (row[j] == '@') { // put Bender at the beginning
                xPosition = i;
                yPosition = j;
            }
            
            if (row[j] == '$') { // save the end point
                xEnd = i;
                yEnd = j;
            }
            
            if (row[j] == 'T' && xTeleport1 == -1) { // save teleporter 1
                xTeleport1 = i;
                yTeleport1 = j;
            } else if (row[j] == 'T' && xTeleport1 != -1) { // save teleporter 2
                xTeleport2 = i;
                yTeleport2 = j;
            }
        }
    }
    
    bool breaker = false; // Bender breaker state
    bool normalPriority = true; // Bender priority state
    vector<string> result; // contain save of the entire road
    char currentDirection = 'S';
    bool currentDirectionOK = false;
    int loopRound = 0;
    bool isLooping = false;
    
    while (xPosition != xEnd || yPosition != yEnd) { // loop while Bender isn't at the end
    
        // Loop checker : check if 2 road between visits of the same square are equal
        if (map[xPosition][yPosition].visited.size() >= 2) { // when there are 2 or more saves, we can  save the road between them
            string roadDirections = "";
            for (int i = map[xPosition][yPosition].visited[map[xPosition][yPosition].visited.size() - 2]; 
                    i < map[xPosition][yPosition].visited.back(); i++) { 
                roadDirections += result[i];
            }
            map[xPosition][yPosition].roadSinceLastVisited.push_back(roadDirections); // save the road
            if(map[xPosition][yPosition].visited.size() > 2 
                    && map[xPosition][yPosition].roadSinceLastVisited.back() == 
                    map[xPosition][yPosition].roadSinceLastVisited[map[xPosition][yPosition].roadSinceLastVisited.size() - 2]) {
                isLooping = true; // 2 road between visits of the same square are equal, we are in a loop
                break; // stop our while loop
            }
        }
        map[xPosition][yPosition].visited.push_back(loopRound); // save the loopRound when the square is visited
        
        // Action of the current square
        switch (map[xPosition][yPosition].type) {  
            case 'T':  
                if (xPosition == xTeleport1 && yPosition  == yTeleport1) {
                    xPosition = xTeleport2;
                    yPosition = yTeleport2;
                } else {
                    xPosition = xTeleport1;
                    yPosition = yTeleport1;
                } 
                break;
            
            case 'I':  
                normalPriority = !normalPriority;  
                break;
            
            case 'B':  
                breaker = !breaker;  
                break;
            
            case 'X':  
                map[xPosition][yPosition].type = ' ';  
                break;
            
            case 'N':  
                currentDirection= 'N';  
                break;
            
            case 'S':  
                currentDirection= 'S';  
                break;
                
            case 'E':  
                currentDirection= 'E';  
                break;
                
            case 'W':  
                currentDirection= 'W';  
                break;
        }  
        
        // CurrentDirection checker : just check if Bender can continue moving  toward his current direction
        switch (currentDirection) 
        {
            case 'N':  
                if (map[xPosition - 1][yPosition].type != '#' && (map[xPosition - 1][yPosition].type != 'X' || breaker)) {
                    xPosition += -1;
                    currentDirectionOK = true;
                    result.push_back("NORTH");
                }
                break;
            
            case 'S':  
                if (map[xPosition + 1][yPosition].type != '#' && (map[xPosition + 1][yPosition].type != 'X' || breaker)) {
                    xPosition += 1;
                    currentDirectionOK = true;
                    result.push_back("SOUTH");
                }  
                break;
                
            case 'E':  
                if (map[xPosition][yPosition + 1].type != '#' && (map[xPosition][yPosition + 1].type != 'X' || breaker)) {
                    yPosition += 1;
                    currentDirectionOK = true;
                    result.push_back("EAST");
                }  
                break;
                
            case 'W':  
                if (map[xPosition][yPosition - 1].type != '#' && (map[xPosition][yPosition - 1].type != 'X' || breaker)) {
                    yPosition += -1;
                    currentDirectionOK = true;
                    result.push_back("WEST");
                }  
                break;
        }
        
        // Priority direction choice : check each direction with the current priority of Bender
        if (normalPriority && !currentDirectionOK) {
            if (map[xPosition + 1][yPosition].type != '#' && (map[xPosition + 1][yPosition].type != 'X' || breaker)) { 
                xPosition += 1;
                result.push_back("SOUTH");
                currentDirection = 'S';
            } else if (map[xPosition][yPosition + 1].type != '#' && (map[xPosition][yPosition + 1].type != 'X' || breaker)) { 
                yPosition += 1;
                result.push_back("EAST");
                currentDirection = 'E';
            } else if (map[xPosition - 1][yPosition].type != '#' && (map[xPosition - 1][yPosition].type != 'X' || breaker)) { 
                xPosition += -1;
                result.push_back("NORTH");
                currentDirection = 'N';
            } else if (map[xPosition][yPosition - 1].type != '#' && (map[xPosition][yPosition - 1].type != 'X' || breaker)) { 
                yPosition += -1;
                result.push_back("WEST");
                currentDirection = 'W';
            }
        } else if (!normalPriority && !currentDirectionOK) {
            if (map[xPosition][yPosition - 1].type != '#' && (map[xPosition][yPosition - 1].type != 'X' || breaker)) { 
                yPosition += -1;
                result.push_back("WEST");
                currentDirection = 'W';
            } else if (map[xPosition - 1][yPosition].type != '#' && (map[xPosition - 1][yPosition].type != 'X' || breaker)) { 
                xPosition += -1;
                result.push_back("NORTH");
                currentDirection = 'N';
            } else if (map[xPosition][yPosition + 1].type != '#' && (map[xPosition][yPosition + 1].type != 'X' || breaker)) { 
                yPosition += 1;
                result.push_back("EAST");
                currentDirection = 'E';
            } else if (map[xPosition + 1][yPosition].type != '#' && (map[xPosition + 1][yPosition].type != 'X' || breaker)) { 
                xPosition += 1;
                result.push_back("SOUTH");
                currentDirection = 'S';
            }
        }
        
        currentDirectionOK = false;
        loopRound++;
    }
    
    // Result display
    if (!isLooping) {
        for(int i = 0; i < result.size(); i++) {
            cout << result[i] << endl;
        }
    } else {
        cout << "LOOP" << endl;
    }
    
}
