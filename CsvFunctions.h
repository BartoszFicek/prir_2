#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <string>
#include <vector>
#include <utility> // std::pair
#include <stdexcept> // std::runtime_error
#include <sstream> // std::stringstream
#include <numeric>
#include <cmath>
using namespace std;

vector < pair<string, vector<double>> > readFromCsv(string fileName);
void writeToCsv(string fileName, vector< pair<string, vector<double>> > data);
vector<vector<double> > readFromCsvWithoutLabels(string fileName);
