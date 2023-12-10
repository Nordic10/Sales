#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <random>
#include <cmath>

//
#include <cstring>

using namespace std;

// simple structure to store city coordinates
// could also use std::pair<double> 
// or define a class

typedef struct {
  double lon, lat;
} COORD;

// fill the array of city locations
int GetData(char* fname, COORD *cities);
void scrambleArray(COORD* array, int size);
double calculateDistance(COORD coordinate1, COORD coordinate2);
double calculateTotalDistance(COORD* array, int size);

int main(int argc, char *argv[]){
  const int NMAX=2500;
  COORD cities[NMAX];
  int ncity=GetData(argv[1],cities);

  double Energy = calculateTotalDistance(cities, ncity);

  cout << "Energy Prescramble: " << Energy << endl;

  double newEnergy = Energy;

  while(newEnergy >= Energy-100000) {
    cout << "this runs" << endl;
    scrambleArray(cities, ncity);
    newEnergy = calculateTotalDistance(cities, ncity);
  }


  //
  double finalEnergy = calculateTotalDistance(cities, ncity);
  cout << "Final Energy:\t" << finalEnergy << endl;

  // creates data file for output
  std::string outFileName_ = "salesman_cities.dat";
  const char* outFileName = outFileName_.c_str();
  FILE *outFile = fopen(outFileName, "w");

  // loops through each coordinate and inputs it into the dat file
  for (int index = 0; index < ncity; index++){

    fprintf(outFile, "%lf %lf\n", cities[index].lon, cities[index].lat);
  }

  cout << "Energy Postscramble: " << newEnergy << endl;

  printf("Read %d cities from data file\n",ncity);
  printf("Longitude  Latitude\n");
  for (int i=0; i<ncity; i++)
    printf("%lf %lf\n",	cities[i].lon,cities[i].lat); 

  
  return 0;
}

int GetData(char* fname, COORD *cities){
  FILE* fp=fopen(fname,"r");
  const int bufsiz=1000;
  char line[bufsiz+1];
  int ncity=0;
  while(1){
    fgets(line,bufsiz,fp);
    if (line[0]=='#') continue;  // skip comments
    if (feof(fp)) break;
    // we only scan for two numbers at start of each line
    sscanf(line,"%lf %lf",&cities[ncity].lon,&cities[ncity].lat);    
    ncity++;
  }
  fclose(fp);
  return ncity;
}

double calculateDistance(COORD coordinate1, COORD coordinate2){

  double rE = 6371 * pow(10, 3);
  double deltaLat = coordinate2.lat - coordinate1.lat;
  double deltaLon = coordinate2.lon - coordinate1.lon;

  
  double a = pow(std::sin(0.5 * deltaLat * M_PI / 180.0), 2) + ( std::cos(coordinate1.lat * M_PI / 180.0) * std::cos(coordinate2.lat * M_PI / 180.0) * pow(std::sin(0.5 * deltaLon * M_PI / 180.0), 2) );
  double c = 2*std::atan2(std::sqrt(a), std::sqrt(1 - a));

  return rE * c;
}

double calculateTotalDistance(COORD* array, int size) {
  double E = 0;

  for (int i = 0; i < size; i++) {
    if (i != size-1) {
      E += calculateDistance(array[i], array[i+1]);
    } else {
      E += calculateDistance(array[0], array[i]);
    }
  }
  return E;
}

void scrambleArray(COORD* array, int size) {

  srand(static_cast<unsigned int>(time(nullptr)));

  for (int i = size - 1; i > 0; --i) {
    int randomIndex = rand() % (i + 1);

    COORD temp = array[i];
    array[i] = array[randomIndex];
    array[randomIndex] = temp;
  }
}
