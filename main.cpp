/***************************************************************************
* File: main.cpp
* Author: Venkata Bapanapalli
* Procedures:
* main  - Runs 1000 experiments calling each algorithm function fcfs, rrq1, spn and srt
          and displays the average and relative average turn around time for each algorithm.

 * uniform   - uniform random number generator, used to generate random numbers for the randomization
              of task size and duration.

* normal    - normal random number generator, used to generate random numbers for the randomization
              of task size and duration.

* fcfs  - Function will use the first-fit algorithm to place tasks from the task info stream
          into memory. The function then does scheduling using the first come first serve
          algorithm.

* rrq1 - Function will use the worst-fit algorithm to place tasks from the task info stream
         into memory. The function then does scheduling using the round robin quantum 1 algorithm.

* spn   - Function will use the next-fit algorithm to place tasks from the task info stream
          into memory. The function then does scheduling using the shortest process next algorithm.

* srt   - Function will use the best-fit algorithm to place tasks from the task info stream
          into memory. The function then does scheduling using the shortest remaining time
          algorithm.

* setTasks - Function will set task duration and task size of a value between 1-16. All the tasks
              that have a task size or duration that is less than 1 is set to 1.
***************************************************************************/
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <iomanip>

using namespace std;
int location = 0, numTasks = 1000;
double fcfsTA = 0, fcfsRTA = 0, rrTA = 0, rrRTA = 0, spnTA = 0, spnRTA = 0, srtTA = 0, srtRTA = 0;						    //Variables used for results

struct taskInfo {     																									    //Structure to hold task information
    int size;																										        //Task size
    int duration;																									        //Task duration
    int flag = -1;																											//Task flag used to determine if a task has been processed or not
    int ignore = 0; 																										//Used to make sure quantum remains 1
    int quantumStarted = 0;																									//Used to determine if quantum has started
    int blockLoc;																									        //Location of task in memory
    int spent;																										        //Time spent to execute
    int received = 0;																										//Time of when the task was received
    int start = 0;																											//Holds the time of when task was started
    int finish = 0;																											//Holds the time of when task was finished
    int turnAround = 0;																										//Holds the task's turnaround time
    double relative = 0;																									//Process relative turn around time
};

struct FCFS{																												//Struct to store FCFS results
    double ttat = 0;
    double rtat = 0;
};

struct RRQ1{																												//Struct to store RRQ1 results
    double ttat = 0;
    double rtat = 0;
};

struct SPN{																													//Struct to store SPN results
    double ttat = 0;
    double rtat = 0;
};

struct SRT{																													//Struct to store SRT results
    double ttat = 0;
    double rtat = 0;
};

struct FCFS fcfsResults;																									//Creating structure to hold FCFS results
struct SPN spnResults;																										//Creating structure to hold SPN results
struct RRQ1 rrResults;																										//Creating structure to hold RRQ1 results
struct SRT srtResults;																										//Creating structure to hold SRT results

/***************************************************************************
* void uniform(int lo, int hi)
* Author: Venkata Bapanapalli
* Date: 3 May 2020
* Description: generates uniform random integers of values between lo and hi.
* Parameters:
* lo    I/P int - min value to generate.
* hi    I/P int - max value to generate.
***************************************************************************/

int uniform(int lo, int hi)
{
    int x, y = hi - lo, z = RAND_MAX / y;
    while((x = random()) > y * z);                                                                                          //Uses random() to generate a number between the ideal range
    return x / z + lo;
}

/***************************************************************************
* int normal(int loops, int ulimit)
* Author: Venkata Bapanapalli
* Date: 3 May 2020
* Description: generates normal random integers of values between lo and hi.
* Parameters:
* loops     I/P int - number of loops to do for loop.
* ulimit    I/P int - max  value to generate.
***************************************************************************/

int normal(int loops, int ulimit)
{
    int x = 0;
    for(int i = 0; i < loops; i++)                                                                                          //Loops four times and generates a random number between 1-16
    {
        x += uniform(0, ulimit);
    }
    return x;
}

/***************************************************************************
* void setTasks( struct taskInfo tasks[], int memBlocks[])
* Author: Venkata Bapanapalli
* Date: 3 May 2020
* Description: Initializes the tasks in the tasks array with random size and duration
               of a value between 1-16 and creates an array for memory blocks.
* Parameters:
* tasks[]       I/P struct taskInfo - Array struct of tasks in the task stream
* memBlocks[]   I/P int -  An array that represents the 56 units of memory in blocks
***************************************************************************/

int setTasks(struct taskInfo tasks[], int memBlocks[])
{
    for( int j=0; j < numTasks; j++)                                                                                        // Randomize the sizes and durations of 1000 tasks
    {
        tasks[j] = *new taskInfo;
        tasks[j].size = normal(4,4);
        if( tasks[j].size < 1 )
            tasks[j].size = 1;
        tasks[j].duration = normal(4,4);
        if( tasks[j].duration < 1 )
            tasks[j].duration = 1;
    }
    memBlocks[0] = 16;																									    // Make memory blocks of size 16, 16, 16, and 8 to equal free memory of 56
    memBlocks[1] = 16;
    memBlocks[2] = 16;
    memBlocks[3] = 8;
}

/***************************************************************************
* void srt(int memBlocks[], int m, struct taskInfo process[])
* Author: Venkata Bapanapalli
* Date: 3 May 2020
* Description: Tasks from the taskInfo stream are processed using the memory
               partitioning algorithm and stored into memory. Then scheduling is
               performed on the tasks using the srt algorithm. Finally, the avg
               turnaround time and average relative turn around time is determined
               for all tasks.

* Parameters:
* memBlocks[]   I/P int - Array representing the memory blocks
* m             I/P int - The amount of blocks in memory
* process[]     I/P struct taskInfo - The array of processes filled with random values
***************************************************************************/

void srt(int memBlocks[], int m, struct taskInfo process[]){
    location = 0;
    int newReceivedTime = 0, n = numTasks;;
    bool processed, completed = false;
    while (!completed){																							            //Loop until all tasks are processed
        for (int i = 0; i < n; i++){ 																						//First fit algorithm
            completed = true;
            processed = false;
            if (process[i].flag == -1){																						//Process tasks with flag of -1
                for (int j = 0; j < m; j++){ 																				//For all the blocks in memory
                    if (memBlocks[j] >= process[i].size){ 															        //If allocation is possible
                        process[i].flag = 1;																				//Set to 1 meaning allocated
                        process[i].received = newReceivedTime;																//Set the process received time
                        memBlocks[j] -= process[i].size;																    //Reduce  block size
                        process[i].blockLoc = j;																		    //Update block location in the struct
                        processed = true;
                        break;
                    }
                }
                int min = 2147483640;
                if (!processed){																					        //If the process was unable to be allocated
                    for (int l = 0; l < n; l++)																				//Finds the process in memory the shortest service time
                    {
                        if (process[l].flag == 1){
                            if (process[l].duration < min)
                            {
                                min = process[l].duration;
                                location = l;
                            }
                        }
                    }
                    process[location].flag = 2; 																			//Mark task as completed
                    memBlocks[process[location].blockLoc] += process[location].size; 							            //Update the block
                    process[location].start = newReceivedTime;																//Set the start time
                    process[location].finish = process[location].start + process[location].duration;					    //Set the finish time
                    process[location].turnAround = process[location].finish - process[location].received;					//Calculate the turn around time
                    process[location].relative = (double)process[location].turnAround / process[location].duration;		    //Calculate the relative turn around time
                    newReceivedTime = process[location].finish;																//Update the new received time variable
                }
            }
            for (int j = 0; j < n; j++){																					//Checking if all tasks have been processed
                if (process[j].flag == -1){																					//If everything has not been processed, ensure the loop keeps executing
                    completed = false;
                }
            }
        }
    }
    bool memNotEmpty = true;
    while (memNotEmpty){																							        //Process all tasks left in memory
        int min = 2147483640;
        location = -1;
        memNotEmpty = false;
        for (int l = 0; l < n; l++)																							//Find the process in memory with the shortest service time
        {
            if (process[l].flag == 1){
                if (process[l].duration < min)
                {
                    min = process[l].duration;
                    location = l;
                }
            }
        }
        if (location != -1 && process[location].flag == 1){																	//If process is in memory
            process[location].flag = 2; 																					//Mark task as completed
            memBlocks[process[location].blockLoc] += process[location].size; 									            //Update the block
            process[location].start = newReceivedTime;																		//Set the start time
                process[location].finish = process[location].start + process[location].duration;							//Set the finish time
            process[location].turnAround = process[location].finish - process[location].received;							//Calculate the turn around time
            process[location].relative = (double)process[location].turnAround / process[location].duration;				    //Calculate the relative turn around time
            newReceivedTime = process[location].finish;																		//Update the new received time variable
        }
        for (int i = 0; i < n; i++){																						//Checking if all tasks have been processed
            if (process[i].flag == 1 && location != -1){
                memNotEmpty = true;
            }
        }
    }
    for (int i = 0; i < n; i++){																							//Find the total turn around time and the total relative turn around time
        srtResults.ttat += process[i].turnAround;
        srtResults.rtat += process[i].relative;
    }
    srtResults.ttat /= n;																					                //Calculate turn around time and relative turn around time
    srtResults.rtat /= n;

    srtTA += srtResults.ttat;
    srtRTA += srtResults.rtat;
}


/***************************************************************************
* void rrq1(int memBlocks[], int m, struct taskInfo process[])
* Author: Venkata Bapanapalli
* Date: 3 May 2020
* Description: Tasks from the taskInfo stream are processed using the memory
               partitioning algorithm and stored into memory. Then scheduling is
               performed on the tasks using the rrq1 algorithm. Finally, the avg
               turnaround time and average relative turn around time is determined
               for all tasks.

* Parameters:
* memBlocks[]   I/P int - Array representing the memory blocks
* m             I/P int - The amount of blocks in memory
* process[]     I/P struct taskInfo - The array of processes filled with random values
***************************************************************************/

void rrq1(int memBlocks[], int m, struct taskInfo process[]){
    int rrFinish = 0, rrStart = 0;
    location = 0;
    int newReceivedTime = 0;
    bool processed, completed = false;
    int max, allocatedProcesses = 0, burstProcess = 0, n = numTasks;														//Holds the value of max received time
    while (!completed){																							            //Keep going until all tasks have been processed
        for (int i = 0; i < n; i++){ 																						//For all processes, perform the first fit algorithm
            completed = true;
            processed = false;
            if (process[i].flag == -1){																						//Only process tasks with a flag of -1
                for (int j = 0; j < m; j++){ 																				//For all the blocks in memory
                    if (memBlocks[j] >= process[i].size){ 															        //Check if allocation is possible
                        process[i].flag = 1;																				//Set task as being processed
                        process[i].received = newReceivedTime;																//Set the process received time
                        memBlocks[j] -= process[i].size;																    //lower block size
                        process[i].blockLoc = j;																		    //Update block location in the struct
                        processed = true;
                        allocatedProcesses++;																				//Increment allocated tasks
                        break;
                    }
                }
                if (!processed){																					        //If the process was unable to be allocated
                    max = -2147483640;
                    if (burstProcess == allocatedProcesses){																//Ensures quantum is 1
                        for (int j = 0; j < n; j++){
                            if (process[j].flag == 1){																		//If process is in memory set to 0
                                process[j].ignore = 0;
                            }
                        }
                        burstProcess = 0;
                    }
                    rrStart++;
                    rrFinish++;																								//Variable used to calculate the rrq1 finish time
                    for (int l = n-1; l >= 0; l--)																			//Finds the process with the max received time in memory with ignore flag of 0
                    {
                        if (process[l].flag == 1 && process[l].ignore != 1){
                            if (process[l].received > max)
                            {
                                max = process[l].received;
                                location = l;
                            }
                        }
                    }
                    if (process[location].quantumStarted == 0){
                        process[location].start = rrStart;
                        process[location].quantumStarted = 1;
                    }
                    burstProcess++;																							//Increment burstProcess
                    process[location].ignore = 1;																			//Change the ignore flag to 1
                    if (burstProcess == allocatedProcesses){																//Used to control quantum = 1 logic
                        for (int j = 0; j < n; j++){
                            if (process[j].flag == 1){
                                process[j].ignore = 0;
                            }
                        }
                        burstProcess = 0;
                    }
                    process[location].spent++;																			    //Increment time spent
                    if((process[location].spent - process[location].duration) == 0){ 								        //If time spent equals 0
                        process[location].flag = 2; 																		//Set as completed
                        memBlocks[process[location].blockLoc] += process[location].size; 						            //Update the block
                        allocatedProcesses--; 																				//Decrement allocated tasks
                        process[location].finish = rrFinish;																//Set the finish time
                        process[location].turnAround = process[location].finish - process[location].received;				//Calculate the turn around time
                        process[location].relative = (double)process[location].turnAround / process[location].duration;	    //Calculate the relative turn around time
                        newReceivedTime = process[location].finish;															//Update the new received time variable
                    }
                }
            }
            for (int j = 0; j < n; j++){																					//Checking if all tasks have been processed
                if (process[j].flag == -1){																					//Continue until all tasks are finished executing
                    completed = false;
                }
            }
        }
    }
    bool memNotEmpty = true;
    while (memNotEmpty){																							        //Process all remaining tasks in memory
        max = -2147483640;
        location = -1;
        memNotEmpty = false;
        if (burstProcess == allocatedProcesses){																			//Ensure quantum is 1
            for (int i = 0; i < n; i++){
                if (process[i].flag == 1){
                    process[i].ignore = 0;
                }
            }
            burstProcess = 0;
        }
        rrFinish++;																											//Variable used to calculate the rrq1 finish time
        rrStart++;
        for (int l = n-1; l >= 0; l--)																						//Finds the process in memory with the smallest received time and flag = 0
        {
            if (process[l].flag == 1 && process[l].ignore != 1){
                if (process[l].received > max)
                {
                    max = process[l].received;
                    location = l;
                }
            }
        }
        if (process[location].quantumStarted == 0){
            process[location].start = rrStart;
            process[location].quantumStarted = 1;
        }
        burstProcess++;																										//Increment burstProcess
        process[location].ignore = 1;																						//Set ignore flag to 1
        if (burstProcess == allocatedProcesses){
            for (int i = 0; i < n; i++){
                if (process[i].flag == 1){
                    process[i].ignore = 0;
                }
            }
            burstProcess = 0;
        }
        process[location].spent++;																						    //Increment time spent
        if ((location != -1 && process[location].flag == 1) && ((process[location].spent - process[location].duration) == 0)){
            process[location].flag = 2; 																					//Mark task as completed
            memBlocks[process[location].blockLoc] += process[location].size; 									            //Update the block
            allocatedProcesses--; 																							//Decrement allocated tasks
            process[location].finish = rrFinish;																			//Set the finish time
            process[location].turnAround = process[location].finish - process[location].received;							//Calculate the turn around time and relative turn around time
            process[location].relative = (double)process[location].turnAround / process[location].duration;
        }
        for (int i = 0; i < n; i++){																						//Checking if all tasks have been processed
            if (process[i].flag == 1 && location != -1){
                memNotEmpty = true;
            }
        }
    }
    for (int i = 0; i < n; i++){																							//Find the total turn around time and the total relative turn around time
        rrResults.ttat += process[i].turnAround;
        rrResults.rtat += process[i].relative;
    }
    rrResults.ttat /= n;																						            //Calculate turn around time and relative turn around time
    rrResults.rtat /= n;

    rrTA += rrResults.ttat;
    rrRTA += rrResults.rtat;
}



/***************************************************************************
* void spn(int memBlocks[], int m, struct taskInfo process[])
* Author: Venkata Bapanapalli
* Date: 3 May 2020
* Description: Tasks from the taskInfo stream are processed using the memory
               partitioning algorithm and stored into memory. Then scheduling is
               performed on the tasks using the spn algorithm. Finally, the avg
               turnaround time and average relative turn around time is determined
               for all tasks.

* Parameters:
* memBlocks[]   I/P int - Array representing the memory blocks
* m             I/P int - The amount of blocks in memory
* process[]     I/P struct taskInfo - The array of processes filled with random values
***************************************************************************/

void spn(int memBlocks[], int m, struct taskInfo process[]){
    location = 0;
    int newReceivedTime = 0, n = numTasks;
    bool processed, completed = false;
    while (!completed){																							            //Keep going until all tasks have been processed

        for (int i = 0; i < n; i++){ 																						//For all processes, perform the first fit algorithm
            completed = true;
            processed = false;

            if (process[i].flag == -1){																						//Only process if the task's flag is -1
                for (int j = 0; j < m; j++){ 																				//For all the blocks in memory
                    if (memBlocks[j] >= process[i].size){ 															        //If allocation is possible
                        process[i].flag = 1;																				//Change the process flag to 1 (representing allocated)
                        process[i].received = newReceivedTime;																//Set the process received time
                        memBlocks[j] -= process[i].size;																    //Reduce  block size
                        process[i].blockLoc = j;																		    //Update block location in the struct
                        processed = true;
                        break;
                    }
                }
                int min = 2147483640;
                if (!processed){																					        //If the process was unable to be allocated
                    for (int l = 0; l < n; l++)																				//Finds the process in memory with shortest time
                    {
                        if (process[l].flag == 1){
                            if (process[l].duration < min)
                            {
                                min = process[l].duration;
                                location = l;
                            }
                        }
                    }
                    process[location].flag = 2; 																			//Mark task as completed
                    memBlocks[process[location].blockLoc] += process[location].size; 							            //Update the block
                    process[location].start = newReceivedTime;																//Set the start time
                    process[location].finish = process[location].start + process[location].duration;					    //Set the finish time
                    process[location].turnAround = process[location].finish - process[location].received;					//Calculate the turn around time
                    process[location].relative = (double)process[location].turnAround / process[location].duration;		    //Calculate the relative turn around time
                    newReceivedTime = process[location].finish;																//Update the new received time
                }
            }
            for (int j = 0; j < n; j++){																					//Checking if all tasks have been processed
                if (process[j].flag == -1){																					//Loop till all tasks are processed
                    completed = false;
                }
            }
        }
    }
    bool memNotEmpty = true;
    while (memNotEmpty){																							        //Process all tasks left in memory
        int min = 2147483640;
        location = -1;
        memNotEmpty = false;
        for (int l = 0; l < n; l++)																							//Find the process in memory with the shortest service time
        {
            if (process[l].flag == 1){
                if (process[l].duration < min)
                {
                    min = process[l].duration;
                    location = l;
                }
            }
        }
        if (location != -1 && process[location].flag == 1){																	//If process is in memory
            process[location].flag = 2; 																					//Mark task as completed
            memBlocks[process[location].blockLoc] += process[location].size; 									            //Update the block
            process[location].start = newReceivedTime;																		//Set the start time
            process[location].finish = process[location].start + process[location].duration;							    //Set the finish time
            process[location].turnAround = process[location].finish - process[location].received;							//Calculate the turn around time
            process[location].relative = (double)process[location].turnAround / process[location].duration;				    //Calculate the relative turn around time
            newReceivedTime = process[location].finish;																		//Update the new received time variable
        }
        for (int i = 0; i < n; i++){																						//Checking if all tasks have been processed
            if (process[i].flag == 1 && location != -1){
                memNotEmpty = true;
            }
        }
    }
    for (int i = 0; i < n; i++){																							//Find the total turn around time and the total relative turn around time
        spnResults.ttat += process[i].turnAround;
        spnResults.rtat += process[i].relative;
    }
    spnResults.ttat /= n;																					                //Calculate turn around time and relative turn around time
    spnResults.rtat /= n;

    spnTA += spnResults.ttat;
    spnRTA += spnResults.rtat;
}

/***************************************************************************
* void fcfs(int memBlocks[], int m, struct taskInfo process[])
* Author: Venkata Bapanapalli
* Date: 3 May 2020
* Description: Tasks from the taskInfo stream are processed using the memory
               partitioning algorithm and stored into memory. Then scheduling is
               performed on the tasks using the fcfs algorithm. Finally, the avg
               turnaround time and average relative turn around time is determined
               for all tasks.

* Parameters:
* memBlocks[]   I/P int - Array representing the memory blocks
* m             I/P int - The amount of blocks in memory
* process[]     I/P struct taskInfo - The array of processes filled with random values
***************************************************************************/

void fcfs(int memBlocks[], int m, struct taskInfo process[]){
    location = 0;
    int newReceivedTime = 0, n = numTasks;
    bool processed, completed = false;
    while (!completed){																							            //Keep going until all tasks have been processed
        for (int i = 0; i < n; i++){ 																						//For all processes, perform the first fit algorithm
            completed = true;
            processed = false;
            if (process[i].flag == -1){																						//Only process if the task's flag is -1
                for (int j = 0; j < m; j++){ 																				//For all the blocks in memory
                    if (memBlocks[j] >= process[i].size){ 															        //If allocation is possible
                        process[i].flag = 1;																				//Change the process flag to 1 (representing allocated)
                        process[i].received = newReceivedTime;																//Set the process received time
                        memBlocks[j] -= process[i].size;																    //Reduce  block size
                        process[i].blockLoc = j;																		    //Update block location in the struct
                        processed = true;
                        break;
                    }
                }
                int min = 2147483640;
                if (!processed){																					        //If the process was unable to be allocated
                    for (int l = 0; l < n; l++)																				//Finds the process in memory the smallest received time
                    {
                        if (process[l].flag == 1){
                            if (process[l].received < min)
                            {
                                min = process[l].received;
                                location = l;
                            }
                        }
                    }
                    process[location].flag = 2; 																			//Mark task as completed
                    memBlocks[process[location].blockLoc] += process[location].size; 							            //Update the block
                    process[location].start = newReceivedTime;																//Set the start time
                    process[location].finish = process[location].start + process[location].duration;
                    process[location].turnAround = process[location].finish - process[location].received;
                    process[location].relative = (double)process[location].turnAround / process[location].duration;
                    newReceivedTime = process[location].finish;
                }
            }
            for (int j = 0; j < n; j++){																					//Checking if all tasks have been processed
                if (process[j].flag == -1){																					//If everything has not been processed, ensure the loop keeps executing
                    completed = false;
                }
            }
        }
    }
    bool memNotEmpty = true;
    while (memNotEmpty){																							        //Process all tasks left in memory
        int min = 2147483640;
        location = -1;
        memNotEmpty = false;
        for (int l = 0; l < n; l++)																							//Find the process in memory with the smallest received time
        {
            if (process[l].flag == 1){
                if (process[l].received < min)
                {
                    min = process[l].received;
                    location = l;
                }
            }
        }
        if (location != -1 && process[location].flag == 1){																	//If process is in memory
            process[location].flag = 2; 																					//Mark task as completed
            memBlocks[process[location].blockLoc] += process[location].size; 									            //Update the block
            process[location].start = newReceivedTime;																		//Set the start time
            process[location].finish = process[location].start + process[location].duration;							    //Set the finish time
            process[location].turnAround = process[location].finish - process[location].received;							//Calculate the turn around time
            process[location].relative = (double)process[location].turnAround / process[location].duration;				    //Calculate the relative turn around time
            newReceivedTime = process[location].finish;																		//Update the new received time variable
        }
        for (int i = 0; i < n; i++){																						//Checking if all tasks have been processed
            if (process[i].flag == 1 && location != -1){
                memNotEmpty = true;
            }
        }
    }
    for (int i = 0; i < n; i++){																							//Find the total turn around time and the total relative turn around time
        fcfsResults.ttat += process[i].turnAround;
        fcfsResults.rtat += process[i].relative;
    }
    fcfsResults.ttat /= n;																					                //Calculate turn around time and relative turn around time
    fcfsResults.rtat /= n;

    fcfsTA += fcfsResults.ttat;
    fcfsRTA += fcfsResults.rtat;
}
/***************************************************************************
* int main( int argc, char *argv[] )
* Author: Venkata Bapanapalli
* Date: 3 May 2020
* Description: Initializes the task stream by calling the setTasks() function
			   and call the four process scheduling algorithms a thousand times
			   in order to generate results regarding the average turnaround time
			   and the average relative turnaround time.
			   The results regarding these calculations are displayed at the end.
* Parameters:
* argc I/P int The number of arguments on the command line
* argv I/P char *[] The arguments on the command line
* main O/P int Status code (not currently used)
**************************************************************************/
int main(int argc, char *argv[])
{
    int fcfsBlock[5];																										//Represents memory used for FCFS sequence
    int spnBlock[5];																										//Represents memory used for SPN sequence
    int rrBlock[5];																											//Represents memory used for RR sequence
    int srtBlock[5];																										//Represents memory used for SRT sequence
    int m = 4;																												//Number of blocks in memory
    int loops = 50;

    struct taskInfo fcfsTasks[numTasks];																					//Creating an array of task structures
    struct taskInfo spnTasks[numTasks];
    struct taskInfo rrTasks[numTasks];
    struct taskInfo srtTasks[numTasks];
    cout << endl << "**Processing**" << endl << endl;
    for (int i = 0; i < loops; i++){																					    //Process all tasks for given loops

        setTasks(fcfsTasks, fcfsBlock);																			            //Randomizing all tasks
        setTasks(spnTasks, spnBlock);
        setTasks(rrTasks, rrBlock);
        setTasks(srtTasks, srtBlock);

        for (int j = 0; j < numTasks; j++){

            srtTasks[j].size = spnTasks[j].size;
            srtTasks[j].duration = spnTasks[j].duration;

            rrTasks[j].size = spnTasks[j].size;
            rrTasks[j].duration = spnTasks[j].duration;

            fcfsTasks[j].size = spnTasks[j].size;
            fcfsTasks[j].duration = spnTasks[j].duration;

        }
        fcfs(fcfsBlock, m, fcfsTasks);																					    //Calling FCFS function
        spn(spnBlock, m, spnTasks);																						    //Calling SPN function
        rrq1(rrBlock, m, rrTasks);																						    //Calling RR function
        srt(srtBlock, m, srtTasks);																						    //Calling SRT function
    }

    cout << "Average turn around time for FCFS is " << setprecision(5) << (fcfsTA/loops) << endl;                       //Printing results
    cout << "Average relative turn around time for FCFS is " << setprecision(5) << (fcfsRTA/loops) << endl;

    cout << "Average turn around time for RRq1 is " << setprecision(5) << (rrTA/loops) << endl;
    cout << "Average relative turn around time for RRq1 is " << setprecision(4) << (rrRTA/loops) << endl;

    cout << "Average turn around time for SPN is " << setprecision(5) << (spnTA/loops) << endl;
    cout << "Average relative turn around time for SPN is " << setprecision(4) << (spnRTA/loops) << endl;

    cout << "Average turn around time for SRT is " << setprecision(5) << (srtTA/loops) << endl;
    cout << "Average relative turn around time for SRT is " << setprecision(4) << (srtRTA/loops) << endl;
}