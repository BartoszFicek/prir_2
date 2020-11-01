CC = mpicxx
CFLAGS = -std=c++11
COMMON_CSV = csvFunctions.cpp

STAND = standardization
NORM = normalization
KNN = knn

all: clean $(STAND) $(NORM) $(KNN)

$(STAND): $(STAND).cpp
	$(CC) $(CFLAGS) -o $(STAND) $(COMMON_CSV) $(STAND).cpp
$(NORM): $(NORM).cpp
	$(CC) $(CFLAGS) -o $(NORM) $(COMMON_CSV) $(NORM).cpp
$(KNN): $(KNN).cpp
	$(CC) $(CFLAGS) -o $(KNN) $(COMMON_CSV) $(KNN).cpp

clean:
	rm -f *.o *~ $(STAND) $(NORM) $(KNN) 

# * * * * * * * * * * * * * * * * * * * *
# run stand:
# 	mpirun -n 1 ./standardization
# 
# run norm:
# 	mpirun -n 1 ./normalization
# 
# run knn:
# 	mpirun -n 1 ./normalization
# 
# * * * * * * * * * * * * * * * * * * * *  