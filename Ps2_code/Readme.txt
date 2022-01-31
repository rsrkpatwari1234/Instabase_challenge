Radhika Patwari
Indian Institute of Technology Kharagpur (IIT Kharagpur)
Problem Statement 2

######################################################################

Technology stack :

1. Ubuntu 16.04
2. jsoncpp
3. cpp

######################################################################

Folder Structure :

-- input.json  					:   workflow with no. of workers
-- Makefile                     :   code exceuting commands
-- TaskScheduler.cpp            :   distributed workflow processing code
-- output.json 					:   output of sample workflows
-- Readme.txt                   :   information regarding this folder

######################################################################

Setup and Running Instructions:

1.Create the input.json file. Format should be the same as followed in this folder

2.Installing jsoncpp on Ubuntu 16.04

$ sudo apt-get install libjsoncpp-dev

3.Run the makefile. It contains all the instructions to compile the file TaskScheduler.cpp

$ make

4.Execute the code using the executable as shown below 

$ ./taskscheduler

######################################################################

Assumptions :

1. All workers have same capacity
2. Tasks dependency graph forms a directed acyclic graph. Otherwise there will be cycles and it will not be possible to come up with a solution
3. Cost of each task is finite

######################################################################

Time complexity : 

Let M be number of workers and N be total number of tasks across all workflows

TC = O(MlongM + NlogN)

######################################################################

Space complexity : O(M + N)

######################################################################

Linkedin handle : https://www.linkedin.com/in/radhika-patwari-334541172/

######################################################################

References : 

1. jsoncpp tutorial : https://sites.cs.ucsb.edu/~pconrad/github/jsoncpp/dist/doxygen/jsoncpp-api-html-0.6.0-dev/class_json_1_1_value.html

2. https://www.researchgate.net/publication/321226091_Improving_Max-Min_scheduling_Algorithm_for_Reducing_the_Makespan_of_Workflow_Execution_in_the_Cloud

3. https://eng.libretexts.org/Courses/Delta_College/Operating_System%3A_The_Basics/09%3A_CPU_Scheduling/9.2%3A_Scheduling_Algorithms

4. https://link.springer.com/chapter/10.1007/978-3-030-02613-4_2