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
        cerr << "Cannot open file: " << filePath << endl;
        exit(1);
    }
    stringstream buffer;
    buffer << file.rdbuf(); // read file content
    return buffer.str(); // tra ve buffer duoi dang string
}

// function to create BWT from text
string BWT(const string& text) {
    int n = text.size(); 
    vector<string> rotations;

    for (int i = 0; i < n; ++i) {
        rotations.push_back(text.substr(i) + text.substr(0, i)); // tao ra cac chuoi rotation
    }
    sort(rotations.begin(), rotations.end()); // sap xep cac chuoi rotation

    string bwt;
    for (const auto& rotation : rotations) {
        bwt += rotation[n - 1]; // lay ky tu cuoi cung cua moi chuoi rotation
    }
    return bwt; // tra ve BWT
}

// function to create inverse BWT from text
string inverseBWT(const string& input) {
    int n = input.size();
    vector<string> table(n, string(n, ' '));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            table[j] = input[j] + table[j].substr(0, n - 1); // them ky tu dau tien cua input vao moi dong cua table
        }
        sort(table.begin(), table.end()); // sap xep lai table sau moi lan them ky tu
    }

    for (const auto& row : table) {
        if (row.back() == '$') { // tim dong co ky tu cuoi cung la '$' => no chinh la original string
            return row;
        }
    }

    return "";
}

// create suffix array from text
vector<int> createSuffixArray(const string& text) {
    int n = text.size();
    // luu tru cac suffix cua text dang pair<string, int> voi int la vi tri cua suffix do, string la suffix do
    vector<pair<string, int>> suffixes;

    for (int i = 0; i < n; ++i) {
        suffixes.emplace_back(text.substr(i), i); // tao ra cac suffix cua text bang cach cat tu vi tri i den het chuoi
    }
    sort(suffixes.begin(), suffixes.end()); // sap xep cac suffix theo thu tu tu dien

    vector<int> suffixArray;
    for (const auto& suffix : suffixes) {
        suffixArray.push_back(suffix.second); // lay vi tri cua cac suffix sau khi da sap xep
    }
    return suffixArray;
}

// create LF mapping (Last to First) from BWT and suffix array
vector<int> createLFMapping(const string& bwt, const vector<int>& suffixArray) {
    int n = bwt.size();
    vector<int> lf(n);              // luu tru LF mapping
    vector<int> count(256, 0);      // luu tru so lan xuat hien cua cac ky tu
    vector<int> cumCount(256, 0);   // luu tru so lan xuat hien cua cac ky tu truoc do

    for (char c : bwt) {
        count[c]++; // dem so lan xuat hien cua cac ky tu trong BWT
    }

    for (int i = 1; i < 256; ++i) {
        cumCount[i] = cumCount[i - 1] + count[i - 1]; // tinh so lan xuat hien cua cac ky tu truoc do
    }

    // tinh LF mapping tu suffix array va BWT bang cach tim vi tri cua suffix[i] trong BWT va gan cho LF[i]
    for (int i = 0; i < n; ++i) {
        // LF[i] is assigned the current count of the character bwt[i] in cumCount
        // cumCount[bwt[i]] is then incremented to reflect the next occurrence of bwt[i]
        lf[i] = cumCount[bwt[i]]++;
    }

    return lf; // tra ve LF mapping
}

// find positions of a pattern in BWT using LF mapping
vector<int> findPatternPositionsBWT(const string& bwt, const string& pattern, const vector<int>& suffixArray, const vector<int>& lf) {
    int n = bwt.size();
    int m = pattern.size();
    int l = 0, r = n - 1; // khoang tim kiem trong BWT

    // tim kiem pattern tu phai sang trai trong BWT
    for (int i = m - 1; i >= 0; --i) {
        char c = pattern[i];        // lay ky tu cuoi cung cua pattern
        bool found = false;         // kiem tra xem ky tu c co xuat hien trong BWT hay khong
        int newL = -1, newR = -1;   // khoang tim kiem moi

        // tim vi tri cua ky tu c trong BWT
        for (int j = l; j <= r; ++j) {
            if (bwt[j] == c) {
                if (!found) {
                    newL = lf[j]; // gan vi tri dau tien cua ky tu c trong BWT cho newL
                    found = true;
                }
                newR = lf[j]; // gan vi tri cuoi cung cua ky tu c trong BWT cho newR
            }
        }

        if (!found) {
            return {};
        }

        l = newL;
        r = newR;

        if (l > r || l < 0 || r >= n) { // neu khoang tim kiem moi khong hop le thi tra ve rong
            return {};
        }
    }

    vector<int> positions;
    for (int i = l; i <= r; ++i) {
        positions.push_back(suffixArray[i]); // lay vi tri cua pattern trong original string
    }

    sort(positions.begin(), positions.end()); // sap xep lai vi tri cua pattern
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
    string paragraph = readFile(paragraphFile); // doc noi dung file paragraph
    if (paragraph.back() != '$') {
        paragraph += '$';
    }

    string patternsContent = readFile(patternsFile);        // doc noi dung file patterns
    istringstream patternsStream(patternsContent);          // chuyen noi dung file patterns sang stream
    string pattern;                                         // luu tru pattern doc duoc tu file patterns
    vector<pair<string, vector<int>>> results;              // luu tru ket qua tim kiem pattern trong BWT

    string bwt = BWT(paragraph);                            // tao BWT tu paragraph
    vector<int> suffixArray = createSuffixArray(paragraph); // tao suffix array tu paragraph
    vector<int> lf = createLFMapping(bwt, suffixArray);     // tao LF mapping tu BWT va suffix array

    // tim kiem cac pattern trong BWT
    while (getline(patternsStream, pattern)) {
        vector<int> positions = findPatternPositionsBWT(bwt, pattern, suffixArray, lf); // tim vi tri cua pattern trong BWT
        results.emplace_back(pattern, positions); // them ket qua tim kiem vao results
    }

    writeOutput(outputFile, results); // ghi ket qua tim kiem ra file output
}

void processBWT(const string& inputFile, const string& outputFile, bool isBwt) {
    ifstream filein(inputFile, ios::in);
    if (!filein.is_open()) {
        cout << "Error opening file: " << inputFile << endl;
        exit(1);
    }

    ofstream fileout(outputFile, ios::out);
    if (!fileout.is_open()) {
        cout << "Error opening file: " << outputFile << endl;
        exit(1);
    }

    string line;
    while (getline(filein, line)) {
        if (isBwt) {
            fileout << BWT(line) << endl;
        } else {
            fileout << inverseBWT(line) << endl;
        }
    }

    fileout.close();
    filein.close();
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: ./main -p paragraph.txt patterns.txt output.txt\n";
        cerr << "       ./main -c input.txt output.txt [--bwt]\n";
        return 1;
    }

    if (string(argv[1]) == "-p") {
        if (argc != 5) {
            cerr << "Usage: ./main -p paragraph.txt patterns.txt output.txt\n";
            return 1;
        }
        string paragraphFile = argv[2]; // lay ten file paragraph (paragraph.txt)
        string patternsFile = argv[3];  // lay ten file patterns (patterns.txt)
        string outputFile = argv[4];    // lay ten file output (output.txt)
        processFiles(paragraphFile, patternsFile, outputFile);
    } 
    else if (string(argv[1]) == "-c") {
        if (argc != 4 && argc != 5) {
            cerr << "Usage: ./main -c input.txt output.txt [--bwt]\n";
            return 1;
        }
        string inputFile = argv[2];     // lay ten file input (input.txt)
        string outputFile = argv[3];    // lay ten file output (output.txt)
        bool isBwt = (argc == 5 && string(argv[4]) == "--bwt");
        processBWT(inputFile, outputFile, isBwt);
    } 
    else {
        cerr << "Invalid option: " << argv[1] << endl;
        return 1;
    }

    return 0;
}