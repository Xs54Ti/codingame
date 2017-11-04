#include <iostream>
#include <vector>

using namespace std;

int main()
{
    int L, C, N;
    cin >> L >> C >> N; cin.ignore();
    
    vector<int> groups;
    for (int i = 0; i < N; i++) {
        int Pi;
        cin >> Pi; cin.ignore();
        groups.push_back(Pi);
    }
    
    vector<vector<int>> savedSituations, savedEndSituations;
    vector<int> savedResults;
    bool allResultsSaved = false;
    int indexForSaved = 0, loopsPossible = 0, placesTaken = 0, groupsIn = 0, j = 0;
    long long result = 0;
    
    for (int i = 0; i < C; i++) {
        if (!allResultsSaved) {
            for (int k = 0; k < savedSituations.size(); k++) {
                if (groups == savedSituations[k]) {
                    allResultsSaved = true;
                    savedSituations.erase(savedSituations.begin(), savedSituations.begin() + k);
                    savedEndSituations.erase(savedEndSituations.begin(), savedEndSituations.begin() + k);
                    savedResults.erase(savedResults.begin(), savedResults.begin() + k);
                    loopsPossible = (C-i) / savedSituations.size();
                    break;
                }
            }
        }
        
        if (allResultsSaved) {
            if (loopsPossible > 0) {
                long long sumSavedResults = 0;
                for (int k = 0; k < savedResults.size(); k++) {
                    sumSavedResults += savedResults[k];
                }
                result += (loopsPossible * sumSavedResults);
                i += (loopsPossible * savedSituations.size());
                loopsPossible = 0;
                if ((C - i) % savedSituations.size() == 0) {
                    continue;
                }
            }
            
            result += savedResults[indexForSaved];
            groups = savedEndSituations[indexForSaved];
            indexForSaved++;
            if(indexForSaved == savedEndSituations.size()){
                indexForSaved = 0;
            }
            continue;
        }
         
        savedSituations.push_back(groups);
        while (placesTaken < L && j < N) {
            if ((placesTaken + groups[j]) <= L) {
                placesTaken += groups[j];
                groupsIn++;
            } else {
                break;
            }
            j++;
        }
        j = 0;
        
        vector<int> v2(groups.begin(), groups.begin() + groupsIn);
        groups.insert (groups.end(),v2.begin(), v2.end());
        groups.erase(groups.begin(), groups.begin() + groupsIn);
        
        savedEndSituations.push_back(groups);
        groupsIn = 0;
        savedResults.push_back(placesTaken);
        result += placesTaken;
        placesTaken = 0;
    }
    cout << result << endl;
}
