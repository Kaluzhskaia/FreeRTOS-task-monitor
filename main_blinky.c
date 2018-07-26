#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "time.h"
#include "uthash.h"

//#pragma comment(lib,"gtk-win32-2.0.lib")
//#pragma comment(lib,"gobject-2.0.lib")

//#include <gtk/gtk.h>
#include <gtk-2.0\gtk\gtk.h>
//#include <glib/gprintf.h>

FILE *fptr;
FILE * fp;
char ptrTaskList[250];
int count=0;
int count_d = 0;
int count_o = 0;
char space = ' ';
int max_len = 0;

#define tskBLOCKED_CHAR		( 'B' )
#define tskREADY_CHAR		( 'R' )
#define tskDELETED_CHAR		( 'D' )
#define tskSUSPENDED_CHAR	( 'S' )
#define tskRUNNING_CHAR     ( 'N' )

SemaphoreHandle_t xMutex;

TaskHandle_t xHandleA = NULL;
TaskHandle_t xHandleB = NULL;
TaskHandle_t xHandleC = NULL;
TaskHandle_t xHandleVerifier = NULL;
TaskHandle_t xHandleGetStates = NULL;
TaskHandle_t xHandleGetSwitcher = NULL;


GHashTable* deadlines;
GHashTable* deadline_hash;
GHashTable* order_hash;

GArray *log;
GArray *log_all_events;
GList* global_log;


int fib(int n)
{
	if (n == 0 || n == 1)
		return n;
	else
		return (fib(n - 1) + fib(n - 2));
}

int digit_to_int(char d)
{
	char str[2];

	str[0] = d;
	str[1] = '\0';
	return (int)strtol(str, NULL, 10);
}

void look_order_hash(gpointer key, gpointer value, gpointer user_data) {

	//printf("\n%c:  ", key);
	fprintf(fptr, "\n%c:  ", key);
	GSList* temp = NULL;
	temp = g_hash_table_lookup(order_hash, key);
	char* ptr;
	ptr = g_hash_table_lookup(order_hash, key);

	for (; *ptr != '\0'; ptr++)
		//printf("%c", *ptr);
		fprintf(fptr, "%c", *ptr);

}
void enableFlushAfterPrintf()
{
	setvbuf(stdout, 0, _IONBF, 0);
	setvbuf(stdin, 0, _IONBF, 0);
}


void TaskA() {
	TickType_t xTimeA;
	
	//fptr = fopen("C:\\Users\\Alina\\Desktop\\newRTOS\\FreeRTOSv10.0.0\\FreeRTOS\\Demo\\WIN32-MSVC\\file.txt", "w+");
	while (1)
 {
		int times[2];
		xTimeA = xTaskGetTickCount();
		times[0] = xTimeA;

		char buf[5];
		itoa(count, buf, 10);

		char state[] = "sA";
		strcat(state, buf);


		xSemaphoreTake(xMutex, portMAX_DELAY);
		fprintf(fptr, "\nSystem time: %d. Task A started\n", xTimeA);
		printf("\nSystem time: %d. Task A started\n", xTimeA);
	
		xSemaphoreGive(xMutex);

		count += 1;
		fprintf(fptr, "Count changed by task A: %d\n", count);

		xTimeA = xTaskGetTickCount();

		int fibonacci = fib(30);

		xSemaphoreTake(xMutex, portMAX_DELAY);
		fprintf(fptr, "30th Fibonacci: %d\n", fibonacci);
		printf("30th Fibonacci: %d\n", fibonacci);
		xSemaphoreGive(xMutex);



		//printf("Current time: %d, This is task A, iteration number %d \n", xTimeA, count);
		fflush(stdout);


		xTimeA = xTaskGetTickCount();
		times[1] = xTimeA;

		char str[] = "A";
		
		strcat(str, buf);

		g_hash_table_insert(deadlines, str, times);
		char stateF[] = "fA";
		strcat(stateF, buf);

		g_array_append_val(log, str[0]);
		g_array_append_val(log, space);

		g_array_append_val(log_all_events, str[0]);
		g_array_append_val(log_all_events, space);


	


		xSemaphoreTake(xMutex, portMAX_DELAY);
		fprintf(fptr, "%d: Task A finished\n\n", xTimeA);
		printf("System time: %d. Task A finished\n\n", xTimeA);
		xSemaphoreGive(xMutex);

		vTaskDelay(200);
	}
	vTaskDelete(NULL);
};

void TaskB() {
	TickType_t xTimeB;
	TaskHandle_t xHandle;


	//fptr = fopen("C:\\Users\\Alina\\Desktop\\newRTOS\\FreeRTOSv10.0.0\\FreeRTOS\\Demo\\WIN32-MSVC\\file.txt", "w+");
	while(1) {

		int times[2];
		xTimeB = xTaskGetTickCount();
		times[0] = xTimeB;


		char buf[5];
		itoa(count, buf, 10);

		char state[] = "sB";
		strcat(state, buf);
	

		xSemaphoreTake(xMutex, portMAX_DELAY);
		fprintf(fptr, "\n%d: Task B started\n", xTimeB);
		printf("\nSystem time: %d. Task B started\n", xTimeB);
		xSemaphoreGive(xMutex);

		count += 1;
		fprintf(fptr, "Count changed by task B: %d\n", count);

		xTimeB = xTaskGetTickCount();

		fprintf(fptr, "Current time: %d, This is task B, iteration number %d \n", xTimeB, count);
		
		int fibonacci = fib(30);
		xSemaphoreTake(xMutex, portMAX_DELAY); 
		fprintf(fptr, "30th Fibonacci: %d\n", fibonacci);
		printf("30th Fibonacci: %d\n", fibonacci);
		xSemaphoreGive(xMutex);

		//printf("Current time: %d, This is task B, iteration number %d \n", xTimeB, count);

		fflush(stdout);
		 

		xTimeB = xTaskGetTickCount();

		times[1] = xTimeB;

		char str[] = "B";
		
		strcat(str, buf);

		g_hash_table_insert(deadlines, str, times);

		char stateF[] = "fB";
		strcat(stateF, buf);
		g_array_append_val(log, str[0]);
		g_array_append_val(log, space);


		g_array_append_val(log_all_events, str[0]);
		g_array_append_val(log_all_events, space);

		xSemaphoreTake(xMutex, portMAX_DELAY);
		fprintf(fptr, "%d: Task B finished\n\n", xTimeB);
		printf("System time: %d. Task B finished\n\n", xTimeB);
		xSemaphoreGive(xMutex);


	    vTaskDelay(400);
	}
	vTaskDelete(NULL);
};


void TaskC() {
	TickType_t xTimeC;
	TaskHandle_t xHandle;


	//fptr = fopen("C:\\Users\\Alina\\Desktop\\newRTOS\\FreeRTOSv10.0.0\\FreeRTOS\\Demo\\WIN32-MSVC\\file.txt", "w+");
	while (1) {

		int times[2];
		xTimeC = xTaskGetTickCount();
		times[0] = xTimeC;


		char buf[5];
		itoa(count, buf, 10);

		char state[] = "sC";
		strcat(state, buf);
	

		xSemaphoreTake(xMutex, portMAX_DELAY);
		fprintf(fptr, "\n%d: Task C started\n", xTimeC);
		printf("\nSystem time: %d. Task C started\n", xTimeC);
		xSemaphoreGive(xMutex);

		count += 1;
		fprintf(fptr, "Count changed by task C: %d\n", count);

		xTimeC = xTaskGetTickCount();

		fprintf(fptr, "Current time: %d, This is task B, iteration number %d \n", xTimeC, count);

		//monitor {
		//task(F), variables($fibonacci: int)
		// watch{
			/*int fibonacci = fib(30);
			fprintf(fptr, "30th Fibonacci: %d\n", fibonacci);*/
		//}}

		int fibonacci = fib(30);
		xSemaphoreTake(xMutex, portMAX_DELAY);
		fprintf(fptr, "30th Fibonacci: %d\n", fibonacci);
		printf("30th Fibonacci: %d\n", fibonacci);
		xSemaphoreGive(xMutex);

		//printf("Current time: %d, This is task B, iteration number %d \n", xTimeC, count);

		fflush(stdout);


		xTimeC = xTaskGetTickCount();

		times[1] = xTimeC;

		char str[] = "C";

		strcat(str, buf);

		g_hash_table_insert(deadlines, str, times);

		char stateF[] = "fC";
		strcat(stateF, buf);
		g_array_append_val(log, str[0]);
		g_array_append_val(log, space);


		g_array_append_val(log_all_events, str[0]);
		g_array_append_val(log_all_events, space);

		xSemaphoreTake(xMutex, portMAX_DELAY);
		fprintf(fptr, "%d: Task C finished\n\n", xTimeC);
		printf("System time: %d. Task C finished\n\n", xTimeC);
		xSemaphoreGive(xMutex);

	
		vTaskDelay(400);
	}
	vTaskDelete(NULL);
};


void check_deadline(gpointer key, gpointer value, gpointer user_data) {
	GSList* temp = NULL;
	temp = g_hash_table_lookup(deadlines, key);

	int first = GPOINTER_TO_INT(temp->data);
	int second = GPOINTER_TO_INT(temp->next);


	if (second-first > digit_to_int(g_hash_table_lookup(deadline_hash, key))) {
		xSemaphoreTake(xMutex, portMAX_DELAY);
		printf("***************************\n");
		printf("Deadline missed for task %s!\n", key);
		printf("***************************\n\n");
		xSemaphoreGive(xMutex);

	}
	xSemaphoreTake(xMutex, portMAX_DELAY);
	//printf("There are %d keys in the hash\n", g_hash_table_size(deadlines));
	gboolean removed = g_hash_table_remove(deadlines, key);
	//printf("There are %d keys in the hash\n", g_hash_table_size(deadlines));
	xSemaphoreGive(xMutex);
}

void check_order() {
	int t = 0;

	while (t < log->len)
	{

		//printf("The list is now %d items long\n", log->len);
		if (g_array_index(log, char, t) == ' ') { t++; continue; };

		char* ptr;
		ptr = g_hash_table_lookup(order_hash, g_array_index(log, char, t));

		if (t >= strlen(ptr)) {
			char* before = 0;
			if (before != 0) {
				before = (char*)realloc(before, t * sizeof(char));
			}
			else {
				before = (char*)malloc(t * sizeof(char));
			}
			for (int i = 0; i < t; i++)
			{
				before[i] = g_array_index(log, char, i);
			}
			before[t - 1] = '\0';
			if (strlen(before) == 0) { t++;  continue; }
			int ind = 0;
			for (int k = strlen(before) - strlen(ptr); k < t - 1; k++)
			{

				if (before[k] != ptr[ind]) {
					printf("\n**********************************\n");
					printf("Requirement %c:%s is missed in ""%s"" ",
						g_array_index(log, char, t), ptr, before);

					printf("%c\n", g_array_index(log, char, t));
					printf("**********************************\n");
					break;
				}
				ind++;
			}

		}
		if (t >= max_len) {
			g_array_remove_range(log, 0, max_len);
			t = 0;
		}
		else
		{
			t++;
		}

	}
}


void verifier() {

	while (1) {

		g_hash_table_foreach(deadlines, (GHFunc)check_deadline, NULL);
		check_order();



		vTaskDelay(300);
	}

}



void write_states(gpointer data, gpointer user_data) {

	fprintf(fptr, "%s|", data);
}
void print_all_log(gpointer data, gpointer user_data) {

	fprintf(fptr, data);
}

void switcher() {

	while(1)
	{
		xSemaphoreTake(xMutex, portMAX_DELAY);
		//printf("This is iteration number %d\n", count);
		xSemaphoreGive(xMutex);

		if (count >= 1000) {
			fprintf(fptr, "\nTasks log:\n");
			for (int i = 0; i < log_all_events->len; i++)
				fprintf(fptr, "%c", g_array_index(log_all_events, char, i));
			
			fclose(fptr);			
			vTaskEndScheduler();
			//system("pause");
		}
		vTaskDelay(400);
	}
	
}


void main_blinky(void)
{
	//enableFlushAfterPrintf();
	fptr = fopen("C:\\Users\\Alina\\Desktop\\newRTOS\\FreeRTOSv10.0.0\\FreeRTOS\\Demo\\WIN32-MSVC\\file.txt", "w+");
	fp = fopen("C:\\Users\\Alina\\Desktop\\newRTOS\\FreeRTOSv10.0.0\\FreeRTOS\\Demo\\WIN32-MSVC\\requirements.txt", "r");

	xMutex = xSemaphoreCreateMutex();
	deadlines = g_hash_table_new(NULL, NULL);
	deadline_hash = g_hash_table_new(NULL, NULL);
	order_hash = g_hash_table_new(NULL, NULL);
	log = g_array_new(FALSE, FALSE, sizeof(gchar));
	log_all_events = g_array_new(FALSE, FALSE, sizeof(gchar));
	global_log = NULL;

	char line[20];
	char file[10][20];


	int n = 0;

	while (fgets(line, sizeof(line), fp)) {
		strcpy(file[n], line);
		file[n][strlen(file[n]) - 1] = '\0';
		if (file[n][0] == 'o') {
			count_o++;
		}
		if (file[n][0] == 'd') {
			count_d++;
		}
		n++;
	}

	fclose(fp);

	char req[10][20];
	char all_rules[10][20];
	int line_n = 0;
	for (int i = 0; i < n; i++)
	{
		int j = 0;

		if (file[i][j] == 'd') {
			g_hash_table_insert(deadline_hash, file[i][2], file[i][4]);

		}

		if (file[i][j] == 'o') {
			int k = 0;

			j += 4;
			while (j < strlen(file[i]))
			{
				int ind1, ind2;
				if (file[i][j] == '(') {
					j++;
					ind1 = j;
					req[line_n][k] = file[i][j];
					all_rules[line_n][k] = file[i][j];
					j++;
					k++;
				}
				else if (file[i][j] == ')') {
					ind2 = j;
					int kv;
					kv = digit_to_int(file[i][ind2 + 2]);
					for (int m = 1; m < kv; m++)
					{
						req[line_n][k] = ' ';
						all_rules[line_n][k] = ' ';
						k++;
						for (int l = ind1; l < ind2; l++)
						{
							req[line_n][k] = file[i][l];
							all_rules[line_n][k] = file[i][l];
							k++;
						}
					}
					break;
				}

				else {

					req[line_n][k] = file[i][j];
					all_rules[line_n][k] = file[i][j];
					j++;
					k++;
				}
			}

			all_rules[line_n][k] = ' ';
			all_rules[line_n][k + 1] = file[i][2];
			all_rules[line_n][k + 2] = '\0';
			req[line_n][k] = '\0';
			g_hash_table_insert(order_hash, file[i][2], req[line_n]);
			line_n++;
		}
	}
	
	int flag = 1;
	for (int i = 0; i < count_o; i++)
	{
		for (int j = 0; j < strlen(all_rules[i]); j++)
		{
			if (all_rules[i][j] == ' ') continue;
			char last_el = all_rules[i][strlen(all_rules[i]) - 1];
			int boolean = g_hash_table_lookup_extended(order_hash, all_rules[i][j], NULL, NULL);
			if (boolean && all_rules[i][j] != last_el) {
				char* ptr;
				ptr = g_hash_table_lookup(order_hash, all_rules[i][j]);
				if (j >= strlen(ptr)) {
					char* before = 0;
					if (before != 0) {
						before = (char*)realloc(before, j * sizeof(char));
					}
					else {
						before = (char*)malloc(j * sizeof(char));
					}
					memcpy(before, all_rules[i], j * sizeof(char));
					before[j - 1] = '\0';
					int ind = 0;
					for (int k = strlen(before) - strlen(ptr); k < j - 1; k++)
					{
						if (before[k] != ptr[ind]) {
							printf("Requirement %c:%s is conflicting with requirement %c:%s\n",
								all_rules[i][j], ptr, last_el, g_hash_table_lookup(order_hash, last_el));

							flag = 0;
							break;
						}
						ind++;
					}
				}
			}
		}
	}
	if (flag) printf("All requirements are correct\n");

	
	for (int i = 0; i < count_o; i++)
	{
		if (strlen(req[i]) > max_len) max_len = strlen(req[i]);
	}


	xTaskCreate(TaskA, (signed char*) "tA", 1500, NULL, 3, &xHandleA);
	xTaskCreate(TaskB, (signed char*) "tB", 1500, NULL, 1, &xHandleB);
	xTaskCreate(TaskC, (signed char*) "tC", 1500, NULL, 1, &xHandleC);
	xTaskCreate(switcher, (signed char*) "switch", 1500, NULL, 1, &xHandleGetSwitcher);
	xTaskCreate(verifier, (signed char*) "ver", 1500, NULL, 2, &xHandleVerifier);

	if(flag)
	vTaskStartScheduler();
	else {
		printf("Please fix the requirements before running the system");
	}

}