#include "CsvFunctions.h"
#include <omp.h>

double findAverage(vector<double> data);
double findDeviation(vector<double> data);
void standardize(string outFile, vector< pair<string, vector<double>> >& data);
int threadNum = 0;

int main(int argc, char* argv[])
{
    threadNum = stoi(argv[1]);
    string fileName = argv[2];
    vector<pair<string, vector<double>> > out;
    out = readFromCsv(fileName);
    
    standardize("standarized_" + fileName, out);
}


void standardize(string outFile, vector< pair<string, vector<double>> >& data) {
    double startTime = omp_get_wtime();
    int i;
    #pragma omp parallel for num_threads(threadNum) private(i)
    for ( i = 0; i < data.size(); ++i) {
        if (data[i].first != "target") {
            double avg = findAverage(data[i].second);
            double deviation = findDeviation(data[i].second);
            //standaryzowanie
	    #pragma omp parallel for num_threads(threadNum)	
            for (int j = 0; j < data[i].second.size(); ++j) {
                data[i].second[j] = (data[i].second[j] - avg) / deviation;
            }
        }
    }
    double endTime = omp_get_wtime();
    cout << endTime - startTime << endl;	
    // zapisywanie do pliku
    writeToCsv(outFile, data);
}

double findAverage(vector<double> data) {
    double average = accumulate(data.begin(), data.end(), 0.0) / data.size();
    return average;
}

// https://www.statisticshowto.com/probability-and-statistics/standard-deviation/#HFSSD - pattern
double findDeviation(vector<double> data) {
    double sum = accumulate(data.begin(), data.end(), 0.0);
	
    double square = sum * sum / data.size();
    double squareSum = 0;

    #pragma omp parallel for num_threads(threadNum)
    for (int i = 0; i < data.size(); ++i) {
        squareSum += data[i] * data[i];
    }
    double diff = squareSum - square;
    double variance = diff / (data.size() - 1);
    double deviation = sqrt(variance);

    return deviation;
}
