#include "CsvFunctions.h"
#include <algorithm> 
#include "mpi.h"

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
		vector<int> nearestResults = { 0, 0 };
#pragma omp parallel for num_threads(threadNum)
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

	void checkAccuracy() {
		int good = 0;
		int bad = 0;
		int rank;
		int size;
		int tag = 99;
		MPI_Status stats;
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		int numThread = MPI_Comm_size(MPI_COMM_WORLD, &size);
		numThread = size;
		int chunks = trainData.size() - 1;
		int chunkSize = trainData[0].size();
		std::vector<int> numOfChunksForThread(numThread, 0);
		int chunkPerThread = chunks / numThread;
		for (int i = 0; i < numOfChunksForThread.size(); ++i) {
			numOfChunksForThread[i] = chunkPerThread;
		}
		if (chunks > (chunkPerThread * numThread)) {
			for (int i = 0; i < chunks - (chunkPerThread * numThread); ++i) {
				numOfChunksForThread[i] += 1;
			}
		}

		if (rank == 0) {
			double startTime = MPI_Wtime();
			// sending chunks to available processors
			int chunkToSend = numOfChunksForThread[0]; //we don't send chunks which have to be processed by first thread
			for (int j = 1; j < numOfChunksForThread.size(); ++j) {
				for (int k = 0; k < numOfChunksForThread[j]; ++k) {
					MPI_Send(&trainData[chunkToSend][0], chunkSize, MPI_DOUBLE, j, tag, MPI_COMM_WORLD);
					chunkToSend++;
				}
			}

			// count knn for main process (rank = 0)
			for (int i = 0; i < numOfChunksForThread[0]; ++i) {
				int predictedTarget = predict(trainData[i]);
				if (predictedTarget == trainData[i][targetColumn]) {
					++good;
				}
				else {
					++bad;
				}
			}

			// read data from other processors
			int receivedChunkNumber = numOfChunksForThread[0]; //we don't receive chunks which was processed by first thread
			std::vector<double> goodAndBadValues(2, 0);
			for (int j = 1; j < numOfChunksForThread.size(); ++j) {
				MPI_Recv(&goodAndBadValues[0], 2, MPI_DOUBLE, j, tag, MPI_COMM_WORLD, &stats);
				good += goodAndBadValues[0];
				bad += goodAndBadValues[1];
			}


			double endTime = MPI_Wtime();
			std::cout << endTime - startTime << std::endl;
		}
		else {
			//receive
			std::vector<double> outputData(2);
			for (int i = 0; i < numOfChunksForThread[rank]; ++i) {
				MPI_Recv(&trainData[i][0], chunkSize, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &stats);
			}

			//calculate normalization for process chunks
			for (int i = 0; i < numOfChunksForThread[0]; ++i) {
				int predictedTarget = predict(trainData[i]);
				if (predictedTarget == trainData[i][targetColumn]) {
					outputData[0] += 1;
				}
				else {
					outputData[1] += 1;
				}
			}

			//send

			MPI_Send(&outputData[0], 2, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);

		}


	}

	double euclideanDistance(vector<double> learning, vector<double> target) {
		vector<double> distanceSquares = {};
		double euclideanDistance = 0;

		for (int i = 0; i < learning.size(); ++i) {
			if (i != targetColumn) {
				double diff = learning[i] - target[i];
				distanceSquares.push_back(diff * diff);
			}
		}


		for (int i = 0; i < distanceSquares.size(); ++i) {
			euclideanDistance += distanceSquares[i];
		}

		euclideanDistance = sqrt(euclideanDistance);
		return euclideanDistance;
	}
};


int main(int argc, char* argv[]) {

	MPI_Init(&argc, &argv);
	for (int i = 0; i < 20; ++i) {
		Knn* knn = new Knn(2, 0);
		knn->loadData("./dataset/heart.csv", 14, 30);
		knn->checkAccuracy();
		delete knn;
	}
	MPI_Finalize();
}