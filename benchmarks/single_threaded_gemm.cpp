#include "tasks.hpp"
#include "iostream"
#include <chrono>
#

using namespace std;


void task()
{
	int m = 100;
	int n = 100;
	int k = 100;
	int rsA, rsB, rsC, 
	csA, csB, csC;

	rsA = rsC = m;
	rsB = k;
	csA = csB = csC = 1;

	double *A = randomMatrix(m, k);
	double *B = randomMatrix(k, n);
	double *C = randomMatrix(m, n);

	GEMM(m, n, k, 
		A, rsA, csA, 
		B, rsB, csB,
		C, rsC, csC);

	
	free(A);
	free(B);
	free(C);	
}

int main() {
	double total_duration = 0.0;
	int num_tasks = 500;
	for(int i = 0; i < num_tasks; ++i) {
		auto start_time = chrono::high_resolution_clock::now();
		task();
		auto end_time = chrono::high_resolution_clock::now();
		total_duration += chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
	}
	cout << "Average time to complete a single GEMM operation: " << total_duration/num_tasks << "ms/op" << endl;
	cout << "Throughput: " << num_tasks/total_duration*1000 << " ops/second." << endl; 

}