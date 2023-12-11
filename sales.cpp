#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <random>
#include <cmath>
#include <cstring>

using namespace std;

// simple structure to store city coordinates
// could also use std::pair<double> 
// or define a class

typedef struct {
  double lon, lat;
} COORD;

const int NMAX=2500;

// fill the array of city locations
int GetData(char* fname, COORD *cities);
void scrambleArray(COORD* array, int size);
double calculateDistance(COORD coordinate1, COORD coordinate2);
double calculateTotalDistance(COORD* array, int size);
void melt(COORD* array, int size, double temperature, double& E);
void swap(COORD* tempArray, int a1, int a2, int a3, int b1, int b2, int b3, double& testE);

int main(int argc, char *argv[]){
  COORD cities[NMAX];
  int ncity=GetData(argv[1],cities);

  double Energy = calculateTotalDistance(cities, ncity);
  double temperature = 2e8;

  cout << "Energy Prescramble: " << Energy << endl;

  scrambleArray(cities, ncity);

  Energy = calculateTotalDistance(cities, ncity);

  cout << "Energy Postscramble: " << Energy << endl;
  /*
  double newEnergy = calculateTotalDistance(cities, ncity);

  while(newEnergy >= Energy-100000) {
    cout << "this runs" << endl;
    scrambleArray(cities, ncity);
    newEnergy = calculateTotalDistance(cities, ncity);
  }
  */

  melt(cities, ncity, temperature, Energy);

  double finalEnergy = calculateTotalDistance(cities, ncity);
  cout << "Final Energy maybe: " << finalEnergy << endl;
  cout << "Final Energy:\t" << Energy << endl;
  
  // creates data file for output
  string outFileName_ = "salesman_cities2.dat";
  const char* outFileName = outFileName_.c_str();
  FILE *outFile = fopen(outFileName, "w");

  // loops through each coordinate and inputs it into the dat file
  for (int index = 0; index < ncity; index++) {
    fprintf(outFile, "%lf %lf\n", cities[index].lon, cities[index].lat);
  }

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

void melt(COORD* array, int size, double temperature, double& E) {

  srand(static_cast<unsigned int>(time(nullptr)));
  double testE = E;
  double ogtemp = temperature;
  COORD tempArray[NMAX];
  for (int i = 0; i < size; i++) {
    tempArray[i] = array[i];
  }

  while (temperature > 100) {
    for (int i = 0; i < 1000000; i++) {
      int r1 = rand() % (size-1);
      int r2 = rand() % size;
      while (r1 >= r2) {
	r2 = rand() % size;
      }
      if (r1 == 0 || r2 == size-1) {
	if (r1 != 0) {
	  swap(tempArray, r1-1, r1, r1+1, r2-1, r2, 0, testE);
	} else if (r2 != size-1) {
	  swap(tempArray, size-1, r1, r1+1, r2-1, r2, r2+1, testE);
	} else {
	  swap(tempArray, size-1, r1, r1+1, r2-1, r2, 0, testE);
	}
      } else {
	swap(tempArray, r1-1, r1, r1+1, r2-1, r2, r2+1, testE);
      }
      
      double metropolisRand = static_cast<double>(rand()) / RAND_MAX;

      if (testE <= E || metropolisRand < exp(-testE/temperature)){
	E = testE;
	for (int i = 0; i < size; i++) {
	  array[i] = tempArray[i];
	}
      } else {
	testE = E;
        for (int i = 0; i < size; i++) {
	  tempArray[i] = array[i];
	}
      }
    }
    temperature-=ogtemp/1000;
    cout << temperature << endl;
  }
}

void swap(COORD* tempArray, int a1, int a2, int a3, int b1, int b2, int b3, double& testE) {
  testE = testE - calculateDistance(tempArray[a1], tempArray[a2]);
  testE = testE - calculateDistance(tempArray[a2], tempArray[a3]);
  testE = testE - calculateDistance(tempArray[b1], tempArray[b2]);
  testE = testE - calculateDistance(tempArray[b2], tempArray[b3]);
	
  COORD temp = tempArray[a2];
  tempArray[a2] = tempArray[b2];
  tempArray[b2] = temp;

  testE = testE + calculateDistance(tempArray[a1], tempArray[a2]);
  testE = testE + calculateDistance(tempArray[a2], tempArray[a3]);
  testE = testE + calculateDistance(tempArray[b1], tempArray[b2]);
  testE = testE + calculateDistance(tempArray[b2], tempArray[b3]);
}
