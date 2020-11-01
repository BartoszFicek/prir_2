#include "CsvFunctions.h"
#include "mpi.h"


double findMax(vector<double> data);
double findMin(vector<double> data);
void normalize(string outFile, vector< pair<string, vector<double>> >& data);

int threadNum = 0;

int main(int argc, char* argv[])
{	
    vector<pair<string, vector<double>> > out;
    out = readFromCsv("bigheartdata.csv");
    
    MPI_Init(&argc, &argv);
    normalize("normalized_mpi.csv", out);
    MPI_Finalize();
}


void normalize(string outFile, vector< pair<string, vector<double>> >& data) {
    
    double min;
    double max;
    
    int rank;
    int size;
    int tag = 99;
    MPI_Status stats;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int numThread = MPI_Comm_size(MPI_COMM_WORLD, &size);
    numThread = size;
    int chunks = data.size() - 1;
    int chunkSize = data[0].second.size();
    std::vector<int> numOfChunksForThread(numThread, 0);
    int chunkPerThread = chunks / numThread;
    for (int i = 0; i < numOfChunksForThread.size(); ++i) {
        numOfChunksForThread[i] = chunkPerThread;
    }
    if (chunks > (chunkPerThread * numThread)){
        for (int i = 0; i < chunks - (chunkPerThread * numThread); ++i){
                numOfChunksForThread[i] += 1;
        }
    }
       
    if (rank == 0) {
            double startTime = MPI_Wtime();
           
            // sending chunks to available processors
            int chunkToSend = numOfChunksForThread[0]; //we don't send chunks which have to be processed by first thread
            for (int j = 1; j < numOfChunksForThread.size(); ++j){
                for (int k = 0; k < numOfChunksForThread[j]; ++k){
                        MPI_Send(&data[chunkToSend].second[0], chunkSize, MPI_DOUBLE, j, tag, MPI_COMM_WORLD);
                        chunkToSend++;
                }
            }
 
            // count standarization for main process (rank = 0)
            for (int i = 0; i < numOfChunksForThread[0]; ++i){
                min = findMin(data[i].second);
		max = findMax(data[i].second);
		//normalizowanie
		for (int j = 0; j < data[i].second.size(); ++j) {
		     data[i].second[j] = (data[i].second[j] - min) / (max - min);
		}
            }
 
            // read data from other processors
            int receivedChunkNumber = numOfChunksForThread[0]; //we don't receive chunks which was processed by first thread
            for (int j = 1; j < numOfChunksForThread.size(); ++j) {
                for (int k = 0; k < numOfChunksForThread[j]; ++k) {
                        MPI_Recv(&data[receivedChunkNumber].second[0], chunkSize, MPI_DOUBLE, j, tag, MPI_COMM_WORLD, &stats);
                        receivedChunkNumber++;
                }
            }
           
            double endTime = MPI_Wtime();
            std::cout << endTime - startTime << std::endl;
    } else {
 
        //receive
        for (int i = 0; i < numOfChunksForThread[rank]; ++i) {
                MPI_Recv(&data[i].second[0], chunkSize, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &stats);
        }
       
 
        
        for (int i = 0; i < numOfChunksForThread[rank]; ++i) {
            min = findMin(data[i].second);
            max = findMax(data[i].second);
            //normalizowanie
            for (int j = 0; j < data[i].second.size(); ++j) {
                data[i].second[j] = (data[i].second[j] - min) / (max - min);
            }
        }
       
        //send
        for (int i = 0; i < numOfChunksForThread[rank]; ++i) {
                MPI_Send(&data[i].second[0], chunkSize, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);
        }      
       
    }

    // double endTime = omp_get_wtime();
    // cout << endTime - startTime << endl;	
    // // zapisywanie do pliku
    // writeToCsv(outFile, data);
}

double findMin(vector<double> data) {
    double min = 10000000;

    for (int i = 0; i < data.size(); ++i) {
	
        if (data[i] < min) {
            min = data[i];
        }
    }
    return min;
}

double findMax(vector<double> data) {
    double max = -10000000;

    for (int i = 0; i < data.size(); ++i) {
	
        if (data[i] > max) {
            max = data[i];
        }
    }
    return max;
}

