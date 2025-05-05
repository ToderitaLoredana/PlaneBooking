 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdbool.h>
 #include <float.h>
 #define _USE_MATH_DEFINES
 #include <math.h>
 #include <time.h>
 #include <ctype.h>
 #include "cJSON/cJSON.h"
 
//Constants used across the program
 #define MAX_AIRPORTS 100
 #define MAX_FLIGHTS 1000
 #define MAX_PATH 50
 #define INFINITY_COST 999999.0
 #define MAX_QUEUE_SIZE 10000
 #define MAX_DAY_LENGTH 10
 #define MAX_TIME_LENGTH 6
 
//structure about the flight options
 typedef enum {
     CHEAPEST,
     FASTEST,
     OPTIMAL
 } RouteType;
 
//structure about the airport information
 typedef struct {
     char code[4];  //IATA code of the airport
     char name[100];
     double lat;
     double lon;
     int min_waiting_time;
 } Airport;
 
//structure about the flight schedule
 typedef struct {
     char from[4];
     char to[4];
     int departure_time;
     int arrival_time;
     int duration;
     double cost;
     double distance;
     char day_of_week[MAX_DAY_LENGTH];
     bool available;
 } ScheduledFlight;
 
 /*structure about the node
 used in the A* algorithm
 where f is a function of g and h
 which are about the cost based on 
 distance and total money
*/
 typedef struct Node {
     int airport_index;
     double g_cost;
     double h_cost;
     double f_cost;
     int parent_index;
     int flight_index;
     int arrival_time;
     char arrival_day[MAX_DAY_LENGTH];
 } Node;
 
//priority queue structure
 typedef struct {
     Node* nodes[MAX_QUEUE_SIZE];
     int size;
 } PriorityQueue;
 
//constants used across the program
 Airport airports[MAX_AIRPORTS];
 ScheduledFlight flights[MAX_FLIGHTS];
 int num_airports = 0;
 int num_flights = 0;
 int connection_time_required = 60;
 
 const char* days_of_week[] = {
     "monday", "tuesday", "wednesday", "thursday", 
     "friday", "saturday", "sunday"
 };
 
//functions used in the program
int time_to_minutes(const char* time_str);
int time_difference(int time1, int time2);
int find_airport_index(const char* code);
double calculate_distance(double lat1, double lon1, double lat2, double lon2);
double heuristic(int current_index, int goal_index, RouteType route_type);
bool is_connection_possible(int arrival_time, int next_departure_time,
                          const char* arrival_day, const char* departure_day,
                          int min_connection_time);
double calculate_route_cost(RouteType route_type, double cost, int duration, double distance);
int calculate_wait_time(int arrival_time, const char* arrival_day,
                      int next_departure_time, const char* departure_day);
void get_next_day(const char* current_day, char* next_day);

// intializes an empty priority
 void pq_init(PriorityQueue* q) {
     q->size = 0;
 }
 
 //function to swap the element sin th priority queue
 //this is used during operations enqueue and dequeue
 void pq_swap(PriorityQueue* q, int i, int j) {
     Node* temp = q->nodes[i];
     q->nodes[i] = q->nodes[j];
     q->nodes[j] = temp;
 }
 
 //adds node in the priority queue and maintains the min-heap property
 void pq_enqueue(PriorityQueue* q, Node* node) {
     if (q->size >= MAX_QUEUE_SIZE) {
         fprintf(stderr, "Error: Priority queue overflow\n");
         return;
     }
     
     //add node at the end
     q->nodes[q->size] = node;
     int current = q->size++;
     
     //heapify up
     while (current > 0 && 
            q->nodes[current]->f_cost < q->nodes[(current-1)/2]->f_cost) {
         pq_swap(q, current, (current-1)/2);
         current = (current-1)/2;
     }
 }
 
 //removes and returns the node with the lowest f_cost from the priority queue
 Node* pq_dequeue(PriorityQueue* q) {
     if (q->size == 0) return NULL;
     
     //get the min element
     Node* min = q->nodes[0];
     //move the last element to the root
     q->nodes[0] = q->nodes[--q->size];
     
     //heapify down
     int current = 0;
     while (1) {
         int left = 2*current + 1;
         int right = 2*current + 2;
         int smallest = current;
         
         //find the smallest among the current, left and right child
         if (left < q->size && 
             q->nodes[left]->f_cost < q->nodes[smallest]->f_cost)
             smallest = left;
             
         if (right < q->size && 
             q->nodes[right]->f_cost < q->nodes[smallest]->f_cost)
             smallest = right;
             
         if (smallest == current) break;
             
         pq_swap(q, current, smallest);
         current = smallest;
     }
     
     return min;
 }

 //converts minutes since midnight to a formated time string
 //minutes - min since midnight
 //time_str - output buffer for the formated time
 void minutes_to_time(int minutes, char* time_str) {
    sprintf(time_str, "%02d:%02d", minutes / 60, minutes % 60);
}
 
//checks if the airport code is correct ( according to the IATA code)
 bool validate_airport_code(const char* code) {
     if (strlen(code) != 3) return false;
     for (int i = 0; i < 3; i++) {
         if (!isalpha(code[i])) return false;
     }
     return true;
 }
 
 //parsed the data.json file which is like our small database
 //containing the airports, flights and flights schedules and other
 bool parse_json_input(const char* filename) {
     FILE* file = fopen(filename, "r");
     if (!file) {
         fprintf(stderr, "Error: Cannot open file %s\n", filename);
         return false;
     }
     
     //get the file size for the memory allocation
     fseek(file, 0, SEEK_END);
     long file_size = ftell(file);
     fseek(file, 0, SEEK_SET);
     
     //allocate memory for the entire file 
     char* json_str = (char*)malloc(file_size + 1);
     if (!json_str) {
         fclose(file);
         fprintf(stderr, "Error: Memory allocation failed\n");
         return false;
     }
     
     //read the file into the memory
     fread(json_str, 1, file_size, file);
     json_str[file_size] = '\0';
     fclose(file);
     
     //parse the json string
     cJSON* json = cJSON_Parse(json_str);
     free(json_str);
     
     if (!json) {
         fprintf(stderr, "Error: Invalid JSON format\n");
         return false;
     }
 
     //parsing the airports + validation
     cJSON* airports_json = cJSON_GetObjectItem(json, "airports");
     if (!airports_json) {
         fprintf(stderr, "Error: Missing 'airports' in JSON\n");
         cJSON_Delete(json);
         return false;
     }
     
     //process each airport in the array
     int airport_count = cJSON_GetArraySize(airports_json);
     for (int i = 0; i < airport_count && num_airports < MAX_AIRPORTS; i++) {
         cJSON* airport = cJSON_GetArrayItem(airports_json, i);
         
         cJSON* code = cJSON_GetObjectItem(airport, "code");
         cJSON* name = cJSON_GetObjectItem(airport, "name");
         cJSON* lat = cJSON_GetObjectItem(airport, "latitude");
         cJSON* lon = cJSON_GetObjectItem(airport, "longitude");
         
         //skipping airports with missing data
         if (!code || !name || !lat || !lon) {
             fprintf(stderr, "Warning: Incomplete airport data at index %d\n", i);
             continue;
         }
         
         //making sure the airport code format is right
         if (!validate_airport_code(code->valuestring)) {
             fprintf(stderr, "Warning: Invalid airport code '%s'\n", code->valuestring);
             continue;
         }
         
         //here storing our airport data in the array
         strncpy(airports[num_airports].code, code->valuestring, 3);
         airports[num_airports].code[3] = '\0';
         strncpy(airports[num_airports].name, name->valuestring, 99);
         airports[num_airports].name[99] = '\0';
         airports[num_airports].lat = lat->valuedouble;
         airports[num_airports].lon = lon->valuedouble;
         
         //get the min waiting time specific for the airport, or use the default time
         cJSON* min_wait = cJSON_GetObjectItem(airport, "min_waiting_time");
         airports[num_airports].min_waiting_time = min_wait ? min_wait->valueint : connection_time_required;
         
         num_airports++;
     }
 
     //here parsing the flights section with validation
     cJSON* flights_json = cJSON_GetObjectItem(json, "flights");
     if (!flights_json) {
         fprintf(stderr, "Error: Missing 'flights' in JSON\n");
         cJSON_Delete(json);
         return false;
     }
 
     //processing each flight in the array
     int flight_count = cJSON_GetArraySize(flights_json);
     for (int i = 0; i < flight_count && num_flights < MAX_FLIGHTS; i++) {
         cJSON* flight_info = cJSON_GetArrayItem(flights_json, i);
         
         //extracting flight properties
         cJSON* from = cJSON_GetObjectItem(flight_info, "from");
         cJSON* to = cJSON_GetObjectItem(flight_info, "to");
         cJSON* base_cost = cJSON_GetObjectItem(flight_info, "base_cost");
         cJSON* schedule = cJSON_GetObjectItem(flight_info, "schedule");
         
         //skiping flights with missing data
         if (!from || !to || !base_cost || !schedule) {
             fprintf(stderr, "Warning: Incomplete flight data at index %d\n", i);
             continue;
         }
         
         //here also we make sure the airport code is right
         if (!validate_airport_code(from->valuestring) || !validate_airport_code(to->valuestring)) {
             fprintf(stderr, "Warning: Invalid airport codes in flight %d\n", i);
             continue;
         }
 
         //processing the schedule for each day of the week
         //form monday to sunday
         for (int day = 0; day < 7; day++) {
             cJSON* day_schedule = cJSON_GetObjectItem(schedule, days_of_week[day]);
             if (!day_schedule) continue;
             
             //extracting schedule details
             cJSON* departure = cJSON_GetObjectItem(day_schedule, "departure_time");
             cJSON* arrival = cJSON_GetObjectItem(day_schedule, "arrival_time");
             cJSON* cost_multiplier = cJSON_GetObjectItem(day_schedule, "cost_multiplier");
             cJSON* available = cJSON_GetObjectItem(day_schedule, "available");
             
             //skip if the schedule is incomplete
             if (!departure || !arrival || !cost_multiplier || !available) {
                 fprintf(stderr, "Warning: Incomplete schedule for %s\n", days_of_week[day]);
                 continue;
             }
             
             //only add the flight if it is available
             if (available->valueint || available->type == cJSON_True) {
                 ScheduledFlight* f = &flights[num_flights];
                 
                 //storing the flight data
                 strncpy(f->from, from->valuestring, 3);
                 f->from[3] = '\0';
                 strncpy(f->to, to->valuestring, 3);
                 f->to[3] = '\0';
                 
                 f->departure_time = time_to_minutes(departure->valuestring);
                 f->arrival_time = time_to_minutes(arrival->valuestring);
                 f->duration = time_difference(f->departure_time, f->arrival_time);
                 f->cost = base_cost->valuedouble * cost_multiplier->valuedouble;
                 strncpy(f->day_of_week, days_of_week[day], MAX_DAY_LENGTH-1);
                 f->available = true;
                 
                 //get the distance if it is provided or it needs to calculate it 
                 cJSON* distance = cJSON_GetObjectItem(flight_info, "distance");
                 if (distance) {
                     f->distance = distance->valuedouble;
                 } else {

                     //calculating distance useing the haversine formula
                     int from_idx = find_airport_index(f->from);
                     int to_idx = find_airport_index(f->to);
                     
                     if (from_idx >= 0 && to_idx >= 0) {
                         f->distance = calculate_distance(
                             airports[from_idx].lat, airports[from_idx].lon,
                             airports[to_idx].lat, airports[to_idx].lon
                         );
                     } else {
                         f->distance = 0;
                         fprintf(stderr, "Warning: Couldn't calculate distance for flight %d\n", num_flights);
                     }
                 }
                 
                 num_flights++;
                 if (num_flights >= MAX_FLIGHTS) break;
             }
         }
     }
 
     //parse the configuration section of the json if it is present
     cJSON* config = cJSON_GetObjectItem(json, "config");
     if (config) {
         cJSON* conn_time = cJSON_GetObjectItem(config, "min_connection_time");
         if (conn_time) connection_time_required = conn_time->valueint;
     }
     
     //clean up the json parser
     cJSON_Delete(json);
     
     //check if we found any valid airports and flights
     if (num_airports == 0 || num_flights == 0) {
         fprintf(stderr, "Error: No valid airports or flights found\n");
         return false;
     }
     
     return true;
 }
 
 /* Implements the A* algorithm in order to find the optimal path between 2 airports
 it uses the priority queue data structure for better performance
 start_code and goal_code -> mean the code of the starting airport and
 the code of the destination airport
 also departure time is in minutes after midnight
 path stored the flight indices in the optimal path
 path_size stores the number of flights in the path
  */
 bool find_optimal_path(const char* start_code, const char* goal_code, 
                       const char* start_day, int departure_time, 
                       RouteType route_type, int* path, int* path_size) {
     //finding indices of the airports                   
     int start_index = find_airport_index(start_code);
     int goal_index = find_airport_index(goal_code);
     
     //validating airport codes
     if (start_index < 0 || goal_index < 0) {
         fprintf(stderr, "Error: Invalid airport codes (%s or %s not found)\n", 
                 start_code, goal_code);
         return false;
     }
 
     //initializing the data structures necessary for the A* algorithm
     bool* closed_set = (bool*)calloc(num_airports, sizeof(bool));
     PriorityQueue open_set;
     pq_init(&open_set);
     
     //tracks the best paths to each airport
     double* best_cost = (double*)malloc(num_airports * sizeof(double));
     int* best_parent = (int*)malloc(num_airports * sizeof(int));
     int* best_flight = (int*)malloc(num_airports * sizeof(int));
     int* best_arrival_time = (int*)malloc(num_airports * sizeof(int));
     char** best_arrival_day = (char**)malloc(num_airports * sizeof(char*));
     
     //initializing the tracking arrays
     for (int i = 0; i < num_airports; i++) {
         best_cost[i] = INFINITY_COST;
         best_parent[i] = -1;
         best_flight[i] = -1;
         best_arrival_time[i] = -1;
         best_arrival_day[i] = (char*)malloc(MAX_DAY_LENGTH * sizeof(char));
         strcpy(best_arrival_day[i], "");
     }
     
     //create and enqueue start node
     Node* start_node = (Node*)malloc(sizeof(Node));
     start_node->airport_index = start_index;
     start_node->g_cost = 0.0;
     start_node->h_cost = heuristic(start_index, goal_index, route_type);
     start_node->f_cost = start_node->g_cost + start_node->h_cost;
     start_node->parent_index = -1;
     start_node->flight_index = -1;
     start_node->arrival_time = departure_time;
     strncpy(start_node->arrival_day, start_day, MAX_DAY_LENGTH-1);
     
     //initialize the best values for the starting airport
     best_cost[start_index] = 0.0;
     best_arrival_time[start_index] = departure_time;
     strcpy(best_arrival_day[start_index], start_day);
     
     //add the start node to the open set
     pq_enqueue(&open_set, start_node);
     
     bool path_found = false;
     //for safety reasons to avoid infinite loops
     int expanded_nodes = 0;
     
     //the main loop of the A* algorithm
     while (open_set.size > 0 && expanded_nodes < 10000) {
         Node* current = pq_dequeue(&open_set);
         expanded_nodes++;
         
         //check if we reached the goal
         if (current->airport_index == goal_index) {
            //reconstruct the path
             int current_airport = goal_index;
             int index = 0;
             
             while (current_airport != start_index && index < MAX_PATH) {
                 path[index++] = best_flight[current_airport];
                 current_airport = best_parent[current_airport];
             }
             
             //check if the path was reconstructed successfully
             if (current_airport == start_index) {
                //reverse the path, because at the moment it is from goal to start
                 for (int i = 0; i < index / 2; i++) {
                     int temp = path[i];
                     path[i] = path[index - i - 1];
                     path[index - i - 1] = temp;
                 }
                 *path_size = index;
                 path_found = true;
             } else {
                 fprintf(stderr, "Error: Path reconstruction failed\n");
             }
             
             free(current);
             break;
         }
         
         //skip if this airport has been visited fully
         if (closed_set[current->airport_index]) {
             free(current);
             continue;
         }
         
         //mark the current airport as visited
         closed_set[current->airport_index] = true;
         
         //explore neightbours, possible flights from the current airport
         for (int i = 0; i < num_flights; i++) {
             if (strcmp(flights[i].from, airports[current->airport_index].code) != 0)
                 continue;
                 
            //find the destination airport index
             int next_index = find_airport_index(flights[i].to);
             //skip if the destination not found or is already fully visited
             if (next_index < 0 || closed_set[next_index])
                 continue;
                 
             //check if it is enought time to make it to the next flight
             int min_connection = airports[current->airport_index].min_waiting_time;
             if (!is_connection_possible(current->arrival_time, flights[i].departure_time,
                                       current->arrival_day, flights[i].day_of_week,
                                       min_connection))
                 continue;
                 
             //total cost depending on the route type
             double route_cost = calculate_route_cost(
                 route_type, 
                 flights[i].cost, 
                 flights[i].duration + calculate_wait_time(
                     current->arrival_time, 
                     current->arrival_day,
                     flights[i].departure_time,
                     flights[i].day_of_week
                 ),
                 flights[i].distance
             );
             
             double total_cost = current->g_cost + route_cost;
             
             //if this path is better than any previous path to this airport
             //update the best path information
             if (total_cost < best_cost[next_index]) {
                 best_cost[next_index] = total_cost;
                 best_parent[next_index] = current->airport_index;
                 best_flight[next_index] = i;
                 best_arrival_time[next_index] = flights[i].arrival_time;
                 
                 //calculate the arrival day
                 if (flights[i].arrival_time < flights[i].departure_time) {
                     get_next_day(flights[i].day_of_week, best_arrival_day[next_index]);
                 } else {
                    //this means the arrival is on the same day
                     strcpy(best_arrival_day[next_index], flights[i].day_of_week);
                 }
                 
                 //create and enqueue neighbor node in order to visit
                 Node* neighbor = (Node*)malloc(sizeof(Node));
                 neighbor->airport_index = next_index;
                 neighbor->g_cost = total_cost;
                 neighbor->h_cost = heuristic(next_index, goal_index, route_type);
                 neighbor->f_cost = neighbor->g_cost + neighbor->h_cost;
                 neighbor->parent_index = current->airport_index;
                 neighbor->flight_index = i;
                 neighbor->arrival_time = flights[i].arrival_time;
                 strcpy(neighbor->arrival_day, best_arrival_day[next_index]);
                 
                 pq_enqueue(&open_set, neighbor);
             }
         }
         //free the curent node after it is been visited
         free(current);
     }
     
     //free all allocated memory
     while (open_set.size > 0) {
         free(pq_dequeue(&open_set));
     }
     
     //I want break free
     //I want to break free from the
     //DSA Project
     free(closed_set);
     free(best_cost);
     free(best_parent);
     free(best_flight);
     free(best_arrival_time);
     
     for (int i = 0; i < num_airports; i++) {
         free(best_arrival_day[i]);
     }
     free(best_arrival_day);
     
     //Error if no path was found
     if (!path_found) {
         fprintf(stderr, "Error: No viable path found from %s to %s\n", 
                 start_code, goal_code);
     }
     
     return path_found;
 }

 //converts the time string to minutes since midnight
int time_to_minutes(const char* time_str) {
    int hours, minutes;
    if (sscanf(time_str, "%d:%d", &hours, &minutes) != 2) {
        fprintf(stderr, "Invalid time format: %s\n", time_str);
        return -1;
    }
    return hours * 60 + minutes;
}

//calculates the time difference between 2 times the start and end time
int time_difference(int time1, int time2) {
    //same day
    if (time2 >= time1) return time2 - time1;
    //overnight
    return (24 * 60 - time1) + time2;
}

//calculates the distance between 2 points using the Haversine formula
double calculate_distance(double lat1, double lon1, double lat2, double lon2) {
    //converting from degrees to radians
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;
    
    //Haversine formula
    double a = sin(dLat/2) * sin(dLat/2) +
               cos(lat1 * M_PI / 180.0) * cos(lat2 * M_PI / 180.0) *
               sin(dLon/2) * sin(dLon/2);
    
    //each radius is 6371 km
    return 6371 * 2 * atan2(sqrt(a), sqrt(1-a));
}

//the heuristics is another function that is taken in consideration during the
//calculation of the total cost
//depending on the route type, different heuristics are used
double heuristic(int current_index, int goal_index, RouteType route_type) {
    //calculate straight-line distance
    double distance = calculate_distance(
        airports[current_index].lat, airports[current_index].lon,
        airports[goal_index].lat, airports[goal_index].lon
    );
    
    switch (route_type) {
        case CHEAPEST: return distance * 0.1;
        case FASTEST: return distance / 800;
        case OPTIMAL: return distance * 0.05;
        default: return distance;
    }
}

//checks if a connection between flights is possible given arrival and departure times
bool is_connection_possible(int arrival_time, int next_departure_time,
                          const char* arrival_day, const char* departure_day,
                          int min_connection_time) {
    //if the flights are on the same day, check if there is enought time between them
    if (strcmp(arrival_day, departure_day) == 0) {
        return (next_departure_time - arrival_time) >= min_connection_time;
    }
    //if on different days, conncection possible
    return true;
}

// calculates the waiting time between glights in minutes, handling cases where
//flights are on the same or on differents days
int calculate_wait_time(int arrival_time, const char* arrival_day,
                      int next_departure_time, const char* departure_day) {
    //if the flights are on the same day, simple time difference
    if (strcmp(arrival_day, departure_day) == 0) {
        return next_departure_time - arrival_time;
    }
    //if flights are on different days
    //calculate minutes from arrival to midnight
    //and add minutes from midnight to next flight
    return (24 * 60 - arrival_time) + next_departure_time;
}

//day of the week calculator
//calculates the next time of the week based on the curent day
void get_next_day(const char* current_day, char* next_day) {
    //iterate through the days of the week
    for (int i = 0; i < 6; i++) {
        if (strcmp(current_day, days_of_week[i]) == 0) {
            //set the next day
            strcpy(next_day, days_of_week[i+1]);
            return;
        }
    }
    //if day not found, default to monday 
    //or if sunday
    strcpy(next_day, "monday");
}

//write output in the json form
//for all 3 route options cheapest, fastest, optimal
//the path contains the flight indices coresponding to that path
bool write_json_output(const char* filename, 
    int* cheapest_path, int cheapest_path_size,
    int* fastest_path, int fastest_path_size,
    int* optimal_path, int optimal_path_size,
    const char* from, const char* to, const char* day,
    int departure_time) {
//creating the root json object
cJSON* root = cJSON_CreateObject();
cJSON* journeys = cJSON_CreateObject();

//adding simple information to the top level
cJSON_AddStringToObject(root, "origin", from);
cJSON_AddStringToObject(root, "destination", to);
cJSON_AddStringToObject(root, "departure_day", day);

//format departure time from minutes to string
char time_str[6];
minutes_to_time(departure_time, time_str);
cJSON_AddStringToObject(root, "departure_time", time_str);

//if available, the cheapest journey
if (cheapest_path_size > 0) {
cJSON* cheapest_journey = cJSON_CreateObject();
cJSON* cheapest_segments = cJSON_CreateArray();
double total_cost = 0;
int total_duration = 0;

//adding each flight node
for (int i = 0; i < cheapest_path_size; i++) {
ScheduledFlight* f = &flights[cheapest_path[i]];
cJSON* segment = cJSON_CreateObject();

//add node details
cJSON_AddStringToObject(segment, "from", f->from);
cJSON_AddStringToObject(segment, "to", f->to);
cJSON_AddStringToObject(segment, "day", f->day_of_week);

minutes_to_time(f->departure_time, time_str);
cJSON_AddStringToObject(segment, "departure_time", time_str);

minutes_to_time(f->arrival_time, time_str);
cJSON_AddStringToObject(segment, "arrival_time", time_str);

cJSON_AddNumberToObject(segment, "duration", f->duration);
cJSON_AddNumberToObject(segment, "cost", f->cost);
cJSON_AddNumberToObject(segment, "distance", f->distance);

cJSON_AddItemToArray(cheapest_segments, segment);

//calculate the total cost and flight duration
total_cost += f->cost;
total_duration += f->duration;
}

//add the total cost and flight duration and the nodes
cJSON_AddNumberToObject(cheapest_journey, "total_cost", total_cost);
cJSON_AddNumberToObject(cheapest_journey, "total_duration", total_duration);
cJSON_AddItemToObject(cheapest_journey, "segments", cheapest_segments);
cJSON_AddItemToObject(journeys, "cheapest", cheapest_journey);
}

//if available the fastest journey
if (fastest_path_size > 0) {
cJSON* fastest_journey = cJSON_CreateObject();
cJSON* fastest_segments = cJSON_CreateArray();
double total_cost = 0;
int total_duration = 0;

//adding each flight node
for (int i = 0; i < fastest_path_size; i++) {
ScheduledFlight* f = &flights[fastest_path[i]];
cJSON* segment = cJSON_CreateObject();

//node information
cJSON_AddStringToObject(segment, "from", f->from);
cJSON_AddStringToObject(segment, "to", f->to);
cJSON_AddStringToObject(segment, "day", f->day_of_week);

minutes_to_time(f->departure_time, time_str);
cJSON_AddStringToObject(segment, "departure_time", time_str);

minutes_to_time(f->arrival_time, time_str);
cJSON_AddStringToObject(segment, "arrival_time", time_str);

cJSON_AddNumberToObject(segment, "duration", f->duration);
cJSON_AddNumberToObject(segment, "cost", f->cost);
cJSON_AddNumberToObject(segment, "distance", f->distance);

cJSON_AddItemToArray(fastest_segments, segment);

total_cost += f->cost;
total_duration += f->duration;
}

cJSON_AddNumberToObject(fastest_journey, "total_cost", total_cost);
cJSON_AddNumberToObject(fastest_journey, "total_duration", total_duration);
cJSON_AddItemToObject(fastest_journey, "segments", fastest_segments);
cJSON_AddItemToObject(journeys, "fastest", fastest_journey);
}

//if avialble, the optimal journey
if (optimal_path_size > 0) {
cJSON* optimal_journey = cJSON_CreateObject();
cJSON* optimal_segments = cJSON_CreateArray();
double total_cost = 0;
int total_duration = 0;

//each flight node
for (int i = 0; i < optimal_path_size; i++) {
ScheduledFlight* f = &flights[optimal_path[i]];
cJSON* segment = cJSON_CreateObject();

//node details
cJSON_AddStringToObject(segment, "from", f->from);
cJSON_AddStringToObject(segment, "to", f->to);
cJSON_AddStringToObject(segment, "day", f->day_of_week);

minutes_to_time(f->departure_time, time_str);
cJSON_AddStringToObject(segment, "departure_time", time_str);

minutes_to_time(f->arrival_time, time_str);
cJSON_AddStringToObject(segment, "arrival_time", time_str);

cJSON_AddNumberToObject(segment, "duration", f->duration);
cJSON_AddNumberToObject(segment, "cost", f->cost);
cJSON_AddNumberToObject(segment, "distance", f->distance);

cJSON_AddItemToArray(optimal_segments, segment);

total_cost += f->cost;
total_duration += f->duration;
}

cJSON_AddNumberToObject(optimal_journey, "total_cost", total_cost);
cJSON_AddNumberToObject(optimal_journey, "total_duration", total_duration);
cJSON_AddItemToObject(optimal_journey, "segments", optimal_segments);
cJSON_AddItemToObject(journeys, "optimal", optimal_journey);
}

//add all journeys to the root
cJSON_AddItemToObject(root, "journeys", journeys);

//writing to the file in the json format
char* json_str = cJSON_Print(root);
FILE* fp = fopen(filename, "w");
if (!fp) {
    //handles the file oppening error
cJSON_Delete(root);
free(json_str);
return false;
}

fputs(json_str, fp);
fclose(fp);

//frees up the allocated memory
cJSON_Delete(root);
free(json_str);
return true;
}

//function which finds the airport index
//it searches an airport in tha array by its code
int find_airport_index(const char* code) {
    //linear search throught the airports array
    for (int i = 0; i < num_airports; i++) {
        if (strcmp(airports[i].code, code) == 0) {
            return i;
        }
    }
    //error
    fprintf(stderr, "Airport not found: %s\n", code);
    return -1;  
}

//calculates the cost of a flight journey based on the route type
//the different route types use other criteria to detrmine the best path
double calculate_route_cost(RouteType route_type, double cost, int duration, double distance) {
    switch (route_type) {
        case CHEAPEST:
        //just about the money
            return cost;  
            
        case FASTEST:
        //time efficiency
            return duration; 
            
        case OPTIMAL:
        //balanced
        //(0.1 cost units per minute)
            return cost + (duration * 0.1);
            
        default:
            fprintf(stderr, "Invalid route type\n");
            return cost;
    }
}

int main(int argc, char* argv[]) {
    //validating the command line arguments
    if (argc < 6) {
        printf("Usage: %s <input.json> <output.json> <from> <to> <day> [departure_time]\n", argv[0]);
        printf("Example: %s flights.json result.json JFK LAX monday 480\n", argv[0]);
        return 1;
    }

    //parsing the command line arguments
    const char* input_file = argv[1];
    const char* output_file = argv[2];
    const char* from_airport = argv[3];
    const char* to_airport = argv[4];
    const char* day = argv[5];
    
    //default departure time: 08:00 (480 minutes)
    int departure_time = (argc > 6) ? atoi(argv[6]) : 480; 

    //load and parse the data from the JSON data.json file
    if (!parse_json_input(input_file)) {
        fprintf(stderr, "Failed to parse input file %s\n", input_file);
        return 1;
    }

    printf("Loaded %d airports and %d flights\n", num_airports, num_flights);

    //Initializing the path array for the 3 route types
    int cheapest_path[MAX_PATH];
    int cheapest_path_size = 0;
    
    int fastest_path[MAX_PATH];
    int fastest_path_size = 0;
    
    int optimal_path[MAX_PATH];
    int optimal_path_size = 0;
    
    //find the routes according to the route type criteria
    bool found_cheapest = find_optimal_path(from_airport, to_airport, day, departure_time, 
                                          CHEAPEST, cheapest_path, &cheapest_path_size);
    
    bool found_fastest = find_optimal_path(from_airport, to_airport, day, departure_time, 
                                         FASTEST, fastest_path, &fastest_path_size);
    
    bool found_optimal = find_optimal_path(from_airport, to_airport, day, departure_time, 
                                         OPTIMAL, optimal_path, &optimal_path_size);

    //check if any valid paths were found
    if (!found_cheapest && !found_fastest && !found_optimal) {
        fprintf(stderr, "No viable paths found from %s to %s\n", from_airport, to_airport);
        return 1;
    }

    //tell the console about the found paths
    printf("Found paths:\n");
    if (found_cheapest) printf("- Cheapest: %d flight segments\n", cheapest_path_size);
    if (found_fastest) printf("- Fastest: %d flight segments\n", fastest_path_size);
    if (found_optimal) printf("- Optimal: %d flight segments\n", optimal_path_size);

    //writing the results to the json output file
    if (!write_json_output(output_file, 
                          cheapest_path, cheapest_path_size,
                          fastest_path, fastest_path_size,
                          optimal_path, optimal_path_size,
                          from_airport, to_airport, day, departure_time)) {
        fprintf(stderr, "Failed to write output file %s\n", output_file);
        return 1;
    }

    printf("Results successfully written to %s\n", output_file);
    return 0;
}