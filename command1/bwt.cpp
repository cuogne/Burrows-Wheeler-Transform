#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;

string bwt(const string &input) {
    int n = input.size();
    vector<string> rotations;

    for (int i = 0; i < n; i++) {
        rotations.push_back(input.substr(i) + input.substr(0, i));
    }

    sort(rotations.begin(), rotations.end());

    string result;
    for (const auto &rotation : rotations) {
        result += rotation[n - 1];
    }

    return result;
}

string inverse_bwt(const string &input) {
    int n = input.size();
    vector<string> table(n, string(n, ' '));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            table[j] = input[j] + table[j].substr(0, n - 1);
        }
        sort(table.begin(), table.end());
    }

    for (const auto &row : table) {
        if (row.back() == '$') {
            return row;
        }
    }

    return "";
}

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        cout << "Error: Invalid number of arguments" << endl;
        return 1;
    }

    string inputFile = argv[1];
    string outputFile = argv[2];
    bool isBwt = (argc == 4 && string(argv[3]) == "--bwt");

    ifstream filein(inputFile, ios::in);
    if (!filein.is_open()) {
        cout << "Error opening file: " << inputFile << endl;
        return 1;
    }

    ofstream fileout(outputFile, ios::out);
    if (!fileout.is_open()) {
        cout << "Error opening file: " << outputFile << endl;
        return 1;
    }

    string line;
    while (getline(filein, line)) {
        if (isBwt) {
            fileout << bwt(line) << endl;
        } else {
            fileout << inverse_bwt(line) << endl;
        }
    }

    fileout.close();
    filein.close();
    return 0;
}