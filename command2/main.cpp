#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

// read file and return content as string
string readFile(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Không thể mở file: " << filePath << endl;
        exit(1);
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// function to create BWT from text
string BWT(const string& text) {
    int n = text.size();
    vector<string> rotations;
    for (int i = 0; i < n; ++i) {
        rotations.push_back(text.substr(i) + text.substr(0, i));
    }
    sort(rotations.begin(), rotations.end());
    string bwt;
    for (const auto& rotation : rotations) {
        bwt += rotation[n - 1];
    }
    return bwt;
}

// create suffix array from text
vector<int> createSuffixArray(const string& text) {
    int n = text.size();
    vector<pair<string, int>> suffixes;
    for (int i = 0; i < n; ++i) {
        suffixes.emplace_back(text.substr(i), i);
    }
    sort(suffixes.begin(), suffixes.end());
    vector<int> suffixArray;
    for (const auto& suffix : suffixes) {
        suffixArray.push_back(suffix.second);
    }
    return suffixArray;
}

// create LF mapping (Last to First) from BWT and suffix array
vector<int> createLFMapping(const string& bwt, const vector<int>& suffixArray) {
    int n = bwt.size();
    vector<int> lf(n);
    vector<int> count(256, 0);
    vector<int> cumCount(256, 0);

    for (char c : bwt) {
        count[c]++;
    }

    for (int i = 1; i < 256; ++i) {
        cumCount[i] = cumCount[i - 1] + count[i - 1];
    }

    for (int i = 0; i < n; ++i) {
        lf[i] = cumCount[bwt[i]]++;
    }

    return lf;
}

// find positions of a pattern in BWT using LF mapping
vector<int> findPatternPositionsBWT(const string& bwt, const string& pattern, const vector<int>& suffixArray, const vector<int>& lf) {
    int n = bwt.size();
    int m = pattern.size();
    int l = 0, r = n - 1;

    for (int i = m - 1; i >= 0; --i) {
        char c = pattern[i];
        bool found = false;
        int newL = -1, newR = -1;

        for (int j = l; j <= r; ++j) {
            if (bwt[j] == c) {
                if (!found) {
                    newL = lf[j];
                    found = true;
                }
                newR = lf[j];
            }
        }

        if (!found) {
            return {};
        }

        l = newL;
        r = newR;

        if (l > r || l < 0 || r >= n) {
            return {};
        }
    }

    vector<int> positions;
    for (int i = l; i <= r; ++i) {
        positions.push_back(suffixArray[i]);
    }

    sort(positions.begin(), positions.end());
    return positions;
}

void writeOutput(const string& outputFile, const vector<pair<string, vector<int>>>& results) {
    ofstream output(outputFile);
    if (!output.is_open()) {
        cerr << "Can't open file: " << outputFile << endl;
        exit(1);
    }
    for (const auto& result : results) {
        output << result.first << ": ";
        for (size_t i = 0; i < result.second.size(); ++i) {
            output << result.second[i];
            if (i < result.second.size() - 1) {
                output << ", ";
            }
        }
        output << "\n";
    }
}

void processFiles(const string& paragraphFile, const string& patternsFile, const string& outputFile) {
    string paragraph = readFile(paragraphFile);
    if (paragraph.back() != '$') {
        paragraph += '$';
    }
    string patternsContent = readFile(patternsFile);
    istringstream patternsStream(patternsContent);
    string pattern;
    vector<pair<string, vector<int>>> results;

    string bwt = BWT(paragraph);
    vector<int> suffixArray = createSuffixArray(paragraph);
    vector<int> lf = createLFMapping(bwt, suffixArray);

    while (getline(patternsStream, pattern)) {
        vector<int> positions = findPatternPositionsBWT(bwt, pattern, suffixArray, lf);
        results.emplace_back(pattern, positions);
    }

    writeOutput(outputFile, results);
}

int main(int argc, char* argv[]) {
    if (argc != 5 || string(argv[1]) != "-p") {
        cerr << "Usage: ./main -p paragraph.txt patterns.txt output.txt\n";
        return 1;
    }

    string paragraphFile = argv[2];
    string patternsFile = argv[3];
    string outputFile = argv[4];

    processFiles(paragraphFile, patternsFile, outputFile);

    return 0;
}
