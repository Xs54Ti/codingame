#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>

using namespace std;

double distance(double x, double y, double x2, double y2) {
    return sqrt(pow(x-x2, 2.0) + pow(y-y2, 2.0));
}

double distance2(double x, double y, double x2, double y2) {
    return pow(x-x2, 2.0) + pow(y-y2, 2.0);
}

double speedOf(double vx, double vy) {
    return sqrt(pow(vx, 2.0) + pow(vy, 2.0));
}

bool nextTurnCollision(double x, double y, double vx , double vy, double opponentX, double opponentY, double vOpponentX, double vOpponentY) {
    double nextX = x + vx;
    double nextY = y + vy;
    double nextOpponentX = opponentX + vOpponentX;
    double nextOpponentY = opponentY + vOpponentY;
    if(distance2(nextX, nextY, nextOpponentX, nextOpponentY) < pow(850.0,2.0)) {
        return true;
    } else {
        return false;
    }
}

struct Pod {
    double x, y, vx, vy, angle, nextCPId, angleWithNextPoint, angleWithTarget, laps, thrust = 0, fartherCheckpoint = 1, lap = 0, engineRecovery = 0, nextCheckpointDist = 0, speed = 0;
    bool boost = true, shield = true, firstEntry = true, newTarget = false, arrivalCheckpoint = false, startCheckpoint = false;
    pair<double,double> target = {0,0}, currentTarget = {0,0};
    const vector<pair<double,double>> &checkpoints;

    Pod(const vector<pair<double,double>> &checkpoints, double laps) : checkpoints(checkpoints), laps(laps) { }

    void status() {
        speed = speedOf(vx, vy);
        currentTarget.first = checkpoints[nextCPId].first;
        currentTarget.second = checkpoints[nextCPId].second;
        nextCheckpointDist = distance(x, y, checkpoints[nextCPId].first, checkpoints[nextCPId].second);

        if(!shield && engineRecovery < 3) {
            engineRecovery++;
        } else {
            shield = true;
            engineRecovery = 0;
        }

        if(target.first != checkpoints[nextCPId].first || target.second != checkpoints[nextCPId].second) {
            newTarget = true;
            target.first = checkpoints[nextCPId].first;
            target.second = checkpoints[nextCPId].second; 
        }

        if(newTarget && checkpoints[1].first == checkpoints[nextCPId].first 
                && checkpoints[1].second == checkpoints[nextCPId].second) {
            lap++;
        }
        
        startCheckpoint = false;
        if(nextCPId == checkpoints.size() - 1 && lap == laps) {
            arrivalCheckpoint = true;
        } else if(nextCPId == 1 && lap == 1) {
            startCheckpoint = true;
        }

        if (newTarget) {
            fartherCheckpoint++;
            if(fartherCheckpoint >= checkpoints.size()) {
                fartherCheckpoint = 0;
            }  
        }
        

        angleWithTarget = (180.0/M_PI) * acos(
                ( (checkpoints[nextCPId].first - x) * cos(angle * M_PI / 180.0) + (checkpoints[nextCPId].second - y) * ((sin(angle * M_PI / 180.0))))
                / (nextCheckpointDist)
        );

        angleWithNextPoint = (180.0/M_PI) * acos(
                ((x - checkpoints[nextCPId].first) * (checkpoints[fartherCheckpoint].first - checkpoints[nextCPId].first) 
                + (y - checkpoints[nextCPId].second) * (checkpoints[fartherCheckpoint].second - checkpoints[nextCPId].second))
                /
                (nextCheckpointDist *
                sqrt(pow(checkpoints[nextCPId].first - checkpoints[fartherCheckpoint].first,2.0) +
                pow(checkpoints[nextCPId].second - checkpoints[fartherCheckpoint].second,2.0)))
        );
    }

    void action(const Pod &oPod1, const Pod &oPod2) {
        if (abs(angleWithTarget) <= 45) {
            thrust = 100;
            if (angleWithNextPoint < 90 && 4 * speed > nextCheckpointDist && !arrivalCheckpoint){
                thrust = 0;
                currentTarget.first = checkpoints[fartherCheckpoint].first;
                currentTarget.second = checkpoints[fartherCheckpoint].second;
            }
        } else {
            thrust = 0;
        }
        
        
        if(!arrivalCheckpoint && !startCheckpoint && (((speed > 400 || oPod1.speed > 400) && nextTurnCollision(x, y, vx, vy, oPod1.x, oPod1.y, oPod1.vx, oPod1.vy))
                || ((speed > 400 || oPod2.speed > 400) && nextTurnCollision(x, y, vx, vy, oPod2.x, oPod2.y, oPod2.vx, oPod2.vy)))) {
            cout << currentTarget.first << " " << currentTarget.second << " " << "SHIELD" << endl;
            shield = false;
        } else if(boost && abs(angleWithTarget) <= 20 && nextCheckpointDist > 4700 ) {
            cout << currentTarget.first << " " << currentTarget.second << " " << "BOOST" << endl;
            boost = false;
        } else if(firstEntry) {
            firstEntry = false;
            cout << currentTarget.first << " " << currentTarget.second << " " << 100 << endl;
        } else {
            cout << currentTarget.first << " " << currentTarget.second << " " << thrust << endl;
        }
        newTarget = false;
    }
};


int main()
{
    vector<pair<double,double>> checkpoints;
    double laps, checkpointCount;
    cin >> laps; cin.ignore();
    cin >> checkpointCount; cin.ignore();
    for (double i = 0; i < checkpointCount; i++) {
        pair<double,double> checkpoint;
        cin >> checkpoint.first >> checkpoint.second; cin.ignore();
        checkpoints.push_back(checkpoint);
    }
    
    Pod pod1 = Pod(checkpoints, laps);
    Pod pod2 = Pod(checkpoints, laps);
    Pod oPod1 = Pod(checkpoints, laps);
    Pod oPod2 = Pod(checkpoints, laps);
    
    while (1) {
        cin >> pod1.x >> pod1.y >> pod1.vx >> pod1.vy >> pod1.angle >> pod1.nextCPId; cin.ignore();
        cin >> pod2.x >> pod2.y >> pod2.vx >> pod2.vy >> pod2.angle >> pod2.nextCPId; cin.ignore();
        cin >> oPod1.x >> oPod1.y >> oPod1.vx >> oPod1.vy >> oPod1.angle >> oPod1.nextCPId; cin.ignore();
        cin >> oPod2.x >> oPod2.y >> oPod2.vx >> oPod2.vy >> oPod2.angle >> oPod2.nextCPId; cin.ignore();

        oPod1.status();
        oPod2.status();
        pod1.status();
        pod2.status();
        pod1.action(oPod1, oPod2);
        pod2.action(oPod1, oPod2);
    }
}
