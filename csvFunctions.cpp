#include "CsvFunctions.h"

void writeToCsv(string fileName, vector< pair<string, vector<double>> > data) {
    ofstream outFile(fileName);
    auto dataSize = data.size();
    for (int i = 0; i < dataSize; ++i) {
        outFile << data[i].first;
        if (i < dataSize - 1) {
            outFile << ", ";
        }
    }
    outFile << "\n";

    for (int i = 0; i < data[0].second.size(); ++i) {
        for (int j = 0; j < data.size(); ++j) {
            outFile << data[j].second[i];
            if (j != data.size() - 1) outFile << ",";
        }
        outFile << "\n";
    }

    outFile.close();
}

vector < pair<string, vector<double>> > readFromCsv(string fileName) {
    vector<pair<string, vector<double>> > result;
    ifstream myFile(fileName);

    if (!myFile.is_open()) throw std::runtime_error("Error in opening file");
    string line, colname;
    double val;

    if (myFile.good())
    {
        getline(myFile, line);
        stringstream ss(line);

        while (getline(ss, colname, ',')) {
            result.push_back({ colname, vector<double> {} });
        }

        while (getline(myFile, line))
        {
            stringstream ss(line);
            int colIdx = 0;

            while (ss >> val) {
                result[colIdx].second.push_back(val);
                if (ss.peek() == ',') ss.ignore();

                colIdx++;
            }
        }
        myFile.close();
    }
    return result;
}

vector<vector<double> > readFromCsvWithoutLabels(string fileName) {
    vector<vector<double>> result;
    ifstream myFile(fileName);

    if (!myFile.is_open()) throw std::runtime_error("Error in opening file");
    string line, colname;
    double val;

    if (myFile.good())
    {
        //labels 
        getline(myFile, line);
        //data
        int colIdx = 0;
        while (getline(myFile, line))
        {
            result.push_back(vector<double> {});
            stringstream ss(line);

            while (ss >> val) {
                result[colIdx].push_back(val);
                if (ss.peek() == ',') ss.ignore();
            }
            ++colIdx;
        }
        myFile.close();
    }
    return result;
}