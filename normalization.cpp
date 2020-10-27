#include "CsvFunctions.h"
#include <omp.h>

void normalize(string outFile, vector< pair<string, vector<double>> >& data);
double findMin(vector<double> data);
double findMax(vector<double> data);
int threadNum = 0;

int main(int argc, char* argv[])
{	
    threadNum = stoi(argv[1]);
    string fileName = argv[2];
    vector<pair<string, vector<double>> > out;
    out = readFromCsv(fileName);
    normalize("normalized_" + fileName, out);
}


void normalize(string outFile, vector< pair<string, vector<double>> >& data) {
    double startTime = omp_get_wtime();
    double min;
    double max;
    #pragma omp parallel for num_threads(threadNum) private(min, max)	
    for (int i = 0; i < data.size(); ++i) {
        if (data[i].first != "target") {
            min = findMin(data[i].second);
            max = findMax(data[i].second);
            //normalizowanie
	    #pragma omp parallel for num_threads(threadNum)	
            for (int j = 0; j < data[i].second.size(); ++j) {
                data[i].second[j] = (data[i].second[j] - min) / (max - min);
            }
        }
    }
    double endTime = omp_get_wtime();
    cout << endTime - startTime << endl;	
    // zapisywanie do pliku
    writeToCsv(outFile, data);
}

double findMin(vector<double> data) {
    double min = 10000000;
    #pragma omp parallel for num_threads(threadNum)
    for (int i = 0; i < data.size(); ++i) {
	
        if (data[i] < min) {
            min = data[i];
        }
    }
    return min;
}

double findMax(vector<double> data) {
    double max = -10000000;
    #pragma omp parallel for num_threads(threadNum)
    for (int i = 0; i < data.size(); ++i) {
	
        if (data[i] > max) {
            max = data[i];
        }
    }
    return max;
}

