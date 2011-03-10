#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>

/******************************************************
  Basic iterative implementation
 *****************************************************/
int bSearch(int ele, int arr[], int size){

	int low = 0, high = size-1, mid;

	while(low <= high){

		mid = (low + high)/2;

		if(ele == arr[mid]){
			return mid;
		}
		else if( ele < arr[mid]){
			high = mid -1;
		}
		else{
			low = mid + 1;
		}	
	}	

	return -1;
}

/******************************************************
  Recursive implementation
 *****************************************************/

int bSearchRecur(int ele, int arr[], int low, int high){

	if(low > high){
		return -1;	
	}

	int mid = (low+high)/2;

	if(ele == arr[mid]){
		return mid;
	}
	else if( ele < arr[mid]){
		return bSearchRecur(ele, arr, low, mid-1);
	}
	else{
		return bSearchRecur(ele, arr, mid+1, high);
	}
}

/******************************************************
  Pthread solution
  The idea is to spawn the threads by splitting the input array into 2 halves.
  Each thread inturn goes ahead and spawns more threads if the key is outside the low/high range
  NOTE : Mutex is not really required as we modify "found" only if the key is found.
 *****************************************************/

typedef struct thread_args{
	int low;
	int high;
	int ele;
	int size;
	const int *a;
} Thread_Args;

int found = -1;

void *bSearchWorker(void* t_args){

	Thread_Args *r_args = (Thread_Args *) t_args;

	int low = r_args->low;
	int high = r_args->high;

	if(low < 0 || low > r_args->size-1 || high < 0 || high > r_args->size-1){
		pthread_exit(NULL);
	}

	if( r_args->a[low] > r_args->ele || r_args->a[high]< r_args->ele){
		pthread_exit(NULL);
	}

	if( r_args->a[low] == r_args->ele){
		found = low;
		pthread_exit(NULL);
	}

	pthread_t threads[2];
	Thread_Args args[2];

	int mid = (high + low)/2;
	args[0].low = 0;
	args[0].high = mid;
	args[0].ele = r_args->ele;
	args[0].size = r_args->size;
	args[0].a = r_args->a;

	args[1].low = mid+1;
	args[1].high = high;
	args[1].ele = r_args->ele;
	args[1].size = r_args->size;
	args[1].a = r_args->a;

	pthread_create(&threads[0], NULL, bSearchWorker, (void *)&args[0]);
	pthread_create(&threads[1], NULL, bSearchWorker, (void *)&args[1]);

	void *status;
	int rc = pthread_join(threads[0], &status);
	
	if (rc) {
		printf("ERROR; return code from pthread_join() is %d\n", rc);
		exit(-1);
	}
	rc = pthread_join(threads[1], &status);

	if (rc) {
		printf("ERROR; return code from pthread_join() is %d\n", rc);
		exit(-1);
	}
	pthread_exit(NULL);
}

int bSearchRecur_threaded(int ele, int arr[], int size){
	pthread_t threads[2];
	Thread_Args args[2];

	found = -1;

	int mid = (size - 1 + 0 )/2;
	args[0].low = 0;
	args[0].high = mid;
	args[0].ele = ele;
	args[0].size = size;
	args[0].a = &arr[0];

	args[1].low = mid+1;
	args[1].high = size-1;
	args[1].ele = ele;
	args[1].size = size;
	args[1].a = &arr[0];

	pthread_create(&threads[0], NULL, bSearchWorker, (void *)&args[0]);
	pthread_create(&threads[1], NULL, bSearchWorker, (void *)&args[1]);

	void *status;
	int rc = pthread_join(threads[0], &status);

	if (rc) {
		printf("ERROR; return code from pthread_join() is %d\n", rc);
		exit(-1);
	}


	pthread_join(threads[1], &status);

	if (rc) {
		printf("ERROR; return code from pthread_join() is %d\n", rc);
		exit(-1);
	}

	return found;

}


/******************************************************
  Recursive bsearch wrapper to suit the test cases
 *****************************************************/
int bSearchRecur_Wrap(int ele, int arr[], int size){
	return bSearchRecur(ele, arr, 0, size-1);
}

/******************************************************
  Tester 
 *****************************************************/
void test_bSearch(int (*f_bSearch)(int,int[],int), char* test_func_name){

	int arr[] = {1,3,5,7};

	printf("Testing : %s \n",test_func_name);
 
	assert( f_bSearch(3,arr,0) == -1);
	assert( f_bSearch(3,arr,1) == -1);
	assert( f_bSearch(1,arr,1) == 0);

	assert( f_bSearch(1,arr,3) == 0);
	assert( f_bSearch(3,arr,3) == 1);
	assert( f_bSearch(5,arr,3) == 2);
	assert( f_bSearch(0,arr,3) == -1);
	assert( f_bSearch(2,arr,3) == -1);
	assert( f_bSearch(4,arr,3) == -1);
	assert( f_bSearch(6,arr,3) == -1);

	assert( f_bSearch(1,arr,4) == 0);
	assert( f_bSearch(3,arr,4) == 1);
	assert( f_bSearch(5,arr,4) == 2);
	assert( f_bSearch(7,arr,4) == 3);
	assert( f_bSearch(0,arr,4) == -1);
	assert( f_bSearch(2,arr,4) == -1);
	assert( f_bSearch(4,arr,4) == -1);
	assert( f_bSearch(6,arr,4) == -1);
	assert( f_bSearch(6,arr,4) == -1);
 
}

int main(){

	test_bSearch(&bSearch, "Iterative");
	test_bSearch(&bSearchRecur_Wrap,"Recursive");
	test_bSearch(&bSearchRecur_threaded,"Threaded");

	return 0;
}
