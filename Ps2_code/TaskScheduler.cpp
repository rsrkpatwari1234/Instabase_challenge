
// Author : Radhika Patwari
// Institute : IIT Kharagpur

// header files
#include <bits/stdc++.h>
#include <iostream>
#include <json/value.h>
#include <jsoncpp/json/json.h>
#include <fstream>

#define ll long long int
#define pp pair<ll,ll>

using namespace std;

// Task structure to store all information related to a specific task of a workflow

// Parameters used :
// task_id - task id defined for the task
// workflow_id - id of workflow to which task belongs
// description - task description 
// cost - task cost
// indegree - # of dependencies of a task
// worker_id - id of the worker allocated this task
// ready_at - time when task is ready for execution since all dependencies have executed
// started_at - time when task execution starts
// completed_at - time when task execution ends
struct Task{
    ll task_id;
    ll workflow_id;
    string description;
    ll cost;
    ll indegree;
    ll worker_id;
    ll ready_at;
    ll started_at;
    ll completed_at;
    Task(){
        indegree = 0;
        worker_id = -1;
        ready_at = -1;
        started_at = 0;
        completed_at = 0;
    }
};

// Creating Directed Acyclic Graph 
// Each node is represented using <workflow_id, task_id> 
map<pp,vector<pp>> edges;

// Assigning each task and workflow with an id
// Helps in space optimization
map<string,ll> taskMapId, workflowMapId;

// Reverse mapping of id to name for tasks and workflows
// Helps in creating output json structure
map<ll,string> taskMapIdReverse, workflowMapIdReverse;

// 2-D matrix for (workflow X task)
// mat[i][j] represents jth task of ith workflow
vector<vector<Task>> mat; 

// computing maximum of 2 64-bit integers
ll max(ll a,ll b){
    return (a>b)?a:b;
}

// computing minimum of 2 64-bit integers
ll min(ll a,ll b){
    return (a<b)?a:b; 
}

// Reading task structure and creating task nodes
// Parameters : 
// workflow_id - id of workflow to which task belongs
// scheduled_at - workflow scheduling time
// task_array - task json list
vector<Task> readTaskList(const ll &workflow_id, const ll &scheduled_at, const Json::Value &task_array){
    ll id = 0;
    string task_name;
    vector<Task> task_list;

    for(auto &task: task_array){
        Task t;
        task_name  = task["name"].asString();

        t.description = task["description"].asString();
        t.cost = task["cost"].asInt64();
        t.workflow_id = workflow_id;
        t.ready_at = scheduled_at;

        //cout<<task_name<<endl;

        taskMapId[task_name] = id;
        taskMapIdReverse[id] = task_name;
        t.task_id = id;

        task_list.push_back(t);
        id++;
    }

    return task_list;
}

// Creating DAG
// Parameters : 
// workflow_id - id of the workflow to which task belongs
// task_array - task json list
void createDAG(ll workflow_id, const Json::Value &task_array){
    ll task_id, dependency_id;
    for(auto &task : task_array){
        task_id = taskMapId[task["name"].asString()];
        for(auto &dependencies : task["dependencies"]){
            dependency_id = taskMapId[dependencies.asString()];
            mat[workflow_id][task_id].indegree++;
            edges[{workflow_id, dependency_id}].push_back({workflow_id, task_id});
        }
    }
}

// Reading the workflow json list
// Parameters:
// cfg_root - workflow json list
void readWorkFlowList(const Json::Value &cfg_root){
    string name;
    ll  scheduled_at, id = 0;
    vector<Task> task_list;
    for(auto  workflow: cfg_root["workflows"]){
        name = workflow["name"].asString();
        scheduled_at = workflow["scheduled_at"].asInt64();
        //cout<<name<<" "<<scheduled_at<<endl;

        workflowMapId[name] = id;
        workflowMapIdReverse[id] = name;

        task_list = readTaskList(id, scheduled_at, workflow["tasks"]);
        mat.push_back(task_list);
        
        createDAG(id, workflow["tasks"]);

        id++;
    }
}

// compare function for tasks priority_queue
struct compare1{
    bool operator()(const pp &a,const pp &b){
        if(mat[a.first][a.second].ready_at > mat[b.first][b.second].ready_at){
            return true;
        }
        if(mat[a.first][a.second].ready_at == mat[b.first][b.second].ready_at
            && mat[a.first][a.second].cost > mat[b.first][b.second].cost){
            return true;
        }
        return false;
    }
};

// compare function for workers priority queue
struct compare2{
    bool operator()(const pp &a,const pp &b){
        return a.second > b.second;
    }
};

// reading number of workers from the json structure
ll readWorkers(const Json::Value &cfg_root){
    ll workers  = cfg_root["workers_count"].asInt64();
    return workers;
}

// Task scheduling algorithm based on greedy approach
void taskScheduler(ll workers){

	// maintaining free workers
    unordered_set<ll> free;

    ll time,workflow_id,task_id,free_worker;

    // maintaining tasks 
    priority_queue<pp,vector<pp>,compare1> ready_tasks;

    // maintaining busy workers
    priority_queue<pp,vector<pp>,compare2> busy_workers;

    for(ll i = 1;i <= workers; i++){
        free.insert(i);
    }

    // taking tasks with indegree  = 0
    for(auto &workflow : mat){
        for(auto &task : workflow){
            if(task.indegree == 0){
                ready_tasks.push({task.workflow_id, task.task_id});
                task.indegree = -1;
            }
        }
    }

    time = 0;

    // using topological sort + greedy approach
    while(!ready_tasks.empty()){
        pp t = ready_tasks.top();
        ready_tasks.pop();

        workflow_id = t.first;
        task_id = t.second;

        // updating current time
        time = max(time+1, mat[workflow_id][task_id].ready_at);

        // update time if no workers are free
        if(free.size()==0){
            if(busy_workers.empty()){
                break;
            }
            time = max(time, busy_workers.top().second);
        }

        // free up workers whose tasks are completed
        while(!busy_workers.empty() && busy_workers.top().second <= time){
            free.insert(busy_workers.top().first);
            busy_workers.pop();
        }

        // sanity check if tasks are left but no workers are available
        if(free.empty())
            break;

        // take any free worker [since all workers are of same capacity]
        free_worker = *free.begin();
        free.erase(free_worker);

        // update start and end timings for current task
        mat[workflow_id][task_id].started_at = time;
        mat[workflow_id][task_id].completed_at = time + mat[workflow_id][task_id].cost;
        mat[workflow_id][task_id].worker_id = free_worker;

        // mark the free worker as busy
        busy_workers.push({free_worker, mat[workflow_id][task_id].completed_at});

        // update the indegree of all tasks which are dependent on current tasks
        for(auto &child : edges[{workflow_id, task_id}]){
            mat[child.first][child.second].indegree--;
            mat[child.first][child.second].ready_at = max(mat[child.first][child.second].ready_at, mat[workflow_id][task_id].completed_at);
            if(mat[child.first][child.second].indegree == 0){
                ready_tasks.push({child.first, child.second});
                mat[child.first][child.second].indegree = -1;
            }
        }
    }
}

// Creating the final json output
Json::Value outputJson(){
    Json::Value workflowList;

    // checking start and end timings for a workflow
    ll scheduled_at, completed_at;

    for(ll workflow_id = 0; workflow_id < mat.size(); workflow_id++){
        Json::Value workflow;
        scheduled_at = LONG_MAX;
        completed_at = LONG_MIN; 
        for(ll task_id = 0; task_id < mat[workflow_id].size(); task_id++){
            Json::Value task;
            task["name"] = taskMapIdReverse[task_id];
            task["started_at"] = mat[workflow_id][task_id].started_at;
            task["completed_at"] = mat[workflow_id][task_id].completed_at;
            task["worker"] = "w" + to_string(mat[workflow_id][task_id].worker_id);

            scheduled_at = min(scheduled_at, mat[workflow_id][task_id].started_at);
            completed_at = max(completed_at, mat[workflow_id][task_id].completed_at);

            workflow["tasks"].append(task);
        }

        workflow["name"] = workflowMapIdReverse[workflow_id];
        workflow["scheduled_at"] = scheduled_at;
        workflow["completed_at"] = completed_at;
        workflowList["workflow"].append(workflow);
    }
    return workflowList;
}

int main()
{
	// Reading input json file
    Json::Reader reader;
    Json::Value cfg_root;
    ifstream cfgfile("input.json");
    cfgfile >> cfg_root;

    // initialising the vectors and maps
    edges.clear();
    mat.clear();
    taskMapId.clear();
    workflowMapId.clear();

    // reading json structure
    readWorkFlowList(cfg_root);

    // reading number of workers
    ll workers = readWorkers(cfg_root);

    // task scheduling algorithm
    taskScheduler(workers);

    // output json
    Json::Value out_root = outputJson();

    // compute execution time for all workflows
    vector<ll> executionTime;
    for(auto &workflow : out_root["workflow"]){
        executionTime.push_back(workflow["completed_at"].asInt64()-workflow["scheduled_at"].asInt64());
    }

    // computing median execution time
    sort(executionTime.begin(),executionTime.end());
    ll medianExecutionTime = (executionTime.size()&1)?executionTime[executionTime.size()/2]:(executionTime.size()/2-1);

    // Problem statement mentions time in seconds
    cout << "Median of Overall execution time of workflows : "<<medianExecutionTime<<" seconds"<<endl;
    cout << "Task schedule can be found in file output.json"<<endl;

    // output the json file
    ofstream cfgfile2("output.json");
    cfgfile2 << out_root;

    return 0;
}       
