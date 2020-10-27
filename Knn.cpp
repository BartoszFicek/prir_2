#include "CsvFunctions.h"
#include <algorithm> 
#include <mpi.h>

// mpicxx –o program program.cpp
// mpirun –n 4 ./program

class Knn {
private:
	int k_numbers;
	int metric;
	int targetColumn;
	int threadNum;
public:
	vector<vector<double>> trainData;
	vector<vector<double>> learningData;
	Knn(int k = 1, int m = 1, int thrd = 1) {
		k_numbers = k;
		metric = m;
		targetColumn = 0;
		threadNum = thrd;
	}

	void setMetric(int number) {
		metric = number;
	}

	void setK(int k) {
		k_numbers = k;
	}

	void loadData(string file, int targetColumnNumber, int trainingPercent = 30) {
		targetColumn = targetColumnNumber - 1;
		vector<vector<double>> data = readFromCsvWithoutLabels(file);
		std::random_shuffle(data.begin(), data.end());
		int startIndex = (trainingPercent / 100.0) * data.size();
		vector<vector<double>> train(data.end() - startIndex, data.begin() + data.size());
		data.erase(data.end() - startIndex, data.begin() + data.size());
		learningData = data;
		trainData = train;
	}

	int predict(vector<double> features) {
		
		vector<pair<double, int>> distancesAndLabels = {};
		
		
		for (int i = 0; i < learningData.size(); ++i) {
			double dist = euclideanDistance(learningData[i], features);
			int ff = (int)learningData[i][targetColumn];

			distancesAndLabels.push_back({ dist, ff });
		}

		sort(distancesAndLabels.begin(), distancesAndLabels.end());
		vector<int> nearestResults = {0, 0};
		// #pragma omp parallel for num_threads(threadNum)
		for (int i = 0; i < k_numbers; ++i) {
			nearestResults[(int)distancesAndLabels[i].second]++;
		}
		if (nearestResults[0] > nearestResults[1]) {
			return 0;
		}
		else {
			return 1;
		}
	}

	double checkAccuracy() {
		int good = 0;
		int bad = 0;
		double time = omp_get_wtime();
		// #pragma omp parallel for num_threads(threadNum)
		for (int i = 0; i < trainData.size(); ++i) {
			int predictedTarget = predict(trainData[i]);
			if (predictedTarget == trainData[i][targetColumn]) {
				++good;
			}
			else {
				++bad;
			}
		}
		double time2 = omp_get_wtime();
		cout << time2-time;
		return good / (double)(good + bad);
	}

	double euclideanDistance(vector<double> learning, vector<double> target) {
		vector<double> distanceSquares = {};
		double euclideanDistance = 0;
		// #pragma omp parallel for num_threads(threadNum)
		for (int i = 0; i < learning.size(); ++i) {
			if (i != targetColumn) {
				double diff = learning[i] - target[i];
				distanceSquares.push_back(diff * diff);
			}
		}
		
		// #pragma omp parallel for num_threads(threadNum)
		for (int i = 0; i < distanceSquares.size(); ++i) {
			euclideanDistance += distanceSquares[i];
		}

		euclideanDistance = sqrt(euclideanDistance);
		return euclideanDistance;
	}
};


int main(int argc, char* argv[]) {
	int threadNum = stoi(argv[1]);
	string fileName = argv[2];
	
	Knn* knn = new Knn(5,0, threadNum);
	knn->loadData(fileName, 14, 30);
	double accuracy = knn->checkAccuracy();
	cout << endl << accuracy;
	delete knn;
}
