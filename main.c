/**
 * Air Travel Planning System using Modified A* Algorithm
 * 
 * This program implements a modified A* algorithm for planning optimal air travel routes,
 * considering both time and cost factors. It reads flight data from a JSON file,
 * calculates the best route, and outputs the results to another JSON file.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdbool.h>
 #include <float.h>
 #include <math.h>

                                                                                                                                                                                               
 // Include JSON parsing library
 #include "cJSON/cJSON.h"
 
 #define MAX_AIRPORTS 100
 #define MAX_FLIGHTS 500
 #define MAX_PATH 50
 #define INFINITY_COST 999999.0
 
 // Structure to represent an airport
 typedef struct {
     char code[4];
     char name[100];
     double lat;
     double lon;
 } Airport;
 
 // Structure to represent a flight
 typedef struct {
     char from[4];
     char to[4];
     int departure_time; // minutes since midnight
     int arrival_time;   // minutes since midnight
     int duration;       // in minutes
     double cost;        // in currency units
     double distance;    // in km
 } Flight;
 
 // Structure for nodes in the A* algorithm
 typedef struct Node {
     int airport_index;
     double g_cost;      // Cost to reach this node from start
     double h_cost;      // Heuristic estimate to goal
     double f_cost;      // Total cost (g_cost + h_cost)
     int parent_index;   // Index of the parent node
     int flight_index;   // Index of the flight used to reach this node
     int arrival_time;   // Arrival time at this node (minutes since midnight)
     struct Node* next;  // For priority queue
 } Node;
 
 // Global variables
 Airport airports[MAX_AIRPORTS];
 Flight flights[MAX_FLIGHTS];
 int num_airports = 0;
 int num_flights = 0;
 int connection_time_required = 60; // Default minimum connection time in minutes
 
 // Function to calculate distance between two airports using Haversine formula
 double calculate_distance(double lat1, double lon1, double lat2, double lon2) {
     double earth_radius = 6371.0; // km
     double lat1_rad = lat1 * 3.14 / 180.0;
     double lat2_rad = lat2 * 3.14 / 180.0;
     double delta_lat = (lat2 - lat1) * 3.14 / 180.0;
     double delta_lon = (lon2 - lon1) * 3.14 / 180.0;
     
     double a = sin(delta_lat/2) * sin(delta_lat/2) +
                cos(lat1_rad) * cos(lat2_rad) * 
                sin(delta_lon/2) * sin(delta_lon/2);
     double c = 2 * atan2(sqrt(a), sqrt(1-a));
     
     return earth_radius * c;
 }
 
 // Find airport index by code
 int find_airport_index(const char* code) {
     for (int i = 0; i < num_airports; i++) {
         if (strcmp(airports[i].code, code) == 0) {
             return i;
         }
     }
     return -1;
 }
 
 // Convert time string "HH:MM" to minutes since midnight
 int time_to_minutes(const char* time_str) {
     int hours, minutes;
     sscanf(time_str, "%d:%d", &hours, &minutes);
     return hours * 60 + minutes;
 }
 
 // Convert minutes since midnight to time string "HH:MM"
 void minutes_to_time(int minutes, char* time_str) {
     int hours = minutes / 60;
     int mins = minutes % 60;
     sprintf(time_str, "%02d:%02d", hours, mins);
 }
 
 // Calculate time difference considering day wrapping
 int time_difference(int time1, int time2) {
     if (time2 >= time1) {
         return time2 - time1;
     } else {
         // Next day
         return (24 * 60 - time1) + time2;
     }
 }
 
 // Priority queue functions for A* algorithm
 void enqueue(Node** head, Node* new_node) {
     if (*head == NULL || new_node->f_cost < (*head)->f_cost) {
         new_node->next = *head;
         *head = new_node;
         return;
     }
     
     Node* current = *head;
     while (current->next != NULL && current->next->f_cost <= new_node->f_cost) {
         current = current->next;
     }
     
     new_node->next = current->next;
     current->next = new_node;
 }
 
 Node* dequeue(Node** head) {
     if (*head == NULL) {
         return NULL;
     }
     
     Node* temp = *head;
     *head = (*head)->next;
     temp->next = NULL;
     return temp;
 }
 
 // Parse the JSON input file
 bool parse_json_input(const char* filename) {
     FILE* file = fopen(filename, "r");
     if (!file) {
         fprintf(stderr, "Error opening file: %s\n", filename);
         return false;
     }
     
     // Read file into a string
     fseek(file, 0, SEEK_END);
     long file_size = ftell(file);
     fseek(file, 0, SEEK_SET);
     
     char* json_str = (char*)malloc(file_size + 1);
     if (!json_str) {
         fclose(file);
         return false;
     }
     
     fread(json_str, 1, file_size, file);
     json_str[file_size] = '\0';
     fclose(file);
     
     // Parse JSON
     cJSON* json = cJSON_Parse(json_str);
     free(json_str);
     
     if (!json) {
         fprintf(stderr, "Error parsing JSON: %s\n", cJSON_GetErrorPtr());
         return false;
     }
     
     // Parse airports
     cJSON* airports_json = cJSON_GetObjectItem(json, "airports");
     if (airports_json) {
         int size = cJSON_GetArraySize(airports_json);
         for (int i = 0; i < size && i < MAX_AIRPORTS; i++) {
             cJSON* airport = cJSON_GetArrayItem(airports_json, i);
             
             cJSON* code = cJSON_GetObjectItem(airport, "code");
             cJSON* name = cJSON_GetObjectItem(airport, "name");
             cJSON* lat = cJSON_GetObjectItem(airport, "latitude");
             cJSON* lon = cJSON_GetObjectItem(airport, "longitude");
             
             if (code && name && lat && lon) {
                 strncpy(airports[num_airports].code, code->valuestring, 3);
                 airports[num_airports].code[3] = '\0';
                 strncpy(airports[num_airports].name, name->valuestring, 99);
                 airports[num_airports].name[99] = '\0';
                 airports[num_airports].lat = lat->valuedouble;
                 airports[num_airports].lon = lon->valuedouble;
                 num_airports++;
             }
         }
     }
     
     // Parse flights
     cJSON* flights_json = cJSON_GetObjectItem(json, "flights");
     if (flights_json) {
         int size = cJSON_GetArraySize(flights_json);
         for (int i = 0; i < size && i < MAX_FLIGHTS; i++) {
             cJSON* flight = cJSON_GetArrayItem(flights_json, i);
             
             cJSON* from = cJSON_GetObjectItem(flight, "from");
             cJSON* to = cJSON_GetObjectItem(flight, "to");
             cJSON* departure = cJSON_GetObjectItem(flight, "departure_time");
             cJSON* arrival = cJSON_GetObjectItem(flight, "arrival_time");
             cJSON* cost = cJSON_GetObjectItem(flight, "cost");
             cJSON* distance = cJSON_GetObjectItem(flight, "distance");
             
             if (from && to && departure && arrival && cost) {
                 strncpy(flights[num_flights].from, from->valuestring, 3);
                 flights[num_flights].from[3] = '\0';
                 
                 strncpy(flights[num_flights].to, to->valuestring, 3);
                 flights[num_flights].to[3] = '\0';
                 
                 flights[num_flights].departure_time = time_to_minutes(departure->valuestring);
                 flights[num_flights].arrival_time = time_to_minutes(arrival->valuestring);
                 flights[num_flights].duration = time_difference(
                     flights[num_flights].departure_time, 
                     flights[num_flights].arrival_time
                 );
                 flights[num_flights].cost = cost->valuedouble;
                 
                 if (distance) {
                     flights[num_flights].distance = distance->valuedouble;
                 } else {
                     // Calculate distance if not provided
                     int from_idx = find_airport_index(flights[num_flights].from);
                     int to_idx = find_airport_index(flights[num_flights].to);
                     
                     if (from_idx >= 0 && to_idx >= 0) {
                         flights[num_flights].distance = calculate_distance(
                             airports[from_idx].lat, airports[from_idx].lon,
                             airports[to_idx].lat, airports[to_idx].lon
                         );
                     } else {
                         flights[num_flights].distance = 0;
                     }
                 }
                 
                 num_flights++;
             }
         }
     }
     
     // Parse configuration
     cJSON* config = cJSON_GetObjectItem(json, "config");
     if (config) {
         cJSON* conn_time = cJSON_GetObjectItem(config, "min_connection_time");
         if (conn_time) {
             connection_time_required = conn_time->valueint;
         }
     }
     
     cJSON_Delete(json);
     return (num_airports > 0 && num_flights > 0);
 }
 
 // Check if an airport is already in the closed set
 bool in_closed_set(bool* closed_set, int airport_index) {
     return closed_set[airport_index];
 }
 
 // Check if there's a flight connection possible
 bool is_connection_possible(int arrival_time, int next_departure_time) {
     int wait_time = time_difference(arrival_time, next_departure_time);
     return wait_time >= connection_time_required;
 }
 
 // Heuristic function for A* algorithm (based on distance or average cost per km)
 double heuristic(int current_index, int goal_index, double avg_cost_per_km) {
     // Use straight-line distance as heuristic
     if (current_index >= 0 && goal_index >= 0) {
         double distance = calculate_distance(
             airports[current_index].lat, airports[current_index].lon,
             airports[goal_index].lat, airports[goal_index].lon
         );
         
         // Return estimated cost based on average cost per kilometer
         return distance * avg_cost_per_km;
     }
     
     return 0.0;
 }
 
 // Calculate average cost per km from all flights
 double calculate_avg_cost_per_km() {
     double total_cost = 0.0;
     double total_distance = 0.0;
     
     for (int i = 0; i < num_flights; i++) {
         if (flights[i].distance > 0) {
             total_cost += flights[i].cost;
             total_distance += flights[i].distance;
         }
     }
     
     return (total_distance > 0) ? (total_cost / total_distance) : 1.0;
 }
 
 // Modified A* algorithm for flight planning
 bool find_optimal_path(const char* start_code, const char* goal_code, 
                        int departure_time, int* path, int* path_size) {
     int start_index = find_airport_index(start_code);
     int goal_index = find_airport_index(goal_code);
     
     if (start_index < 0 || goal_index < 0) {
         fprintf(stderr, "Invalid airport codes\n");
         return false;
     }
     
     // Calculate average cost per km for heuristic
     double avg_cost_per_km = calculate_avg_cost_per_km();
     
     // Initialize data structures for A*
     bool* closed_set = (bool*)calloc(num_airports, sizeof(bool));
     Node* open_set = NULL;
     
     // Create start node and add to open set
     Node* start_node = (Node*)malloc(sizeof(Node));
     start_node->airport_index = start_index;
     start_node->g_cost = 0.0;
     start_node->h_cost = heuristic(start_index, goal_index, avg_cost_per_km);
     start_node->f_cost = start_node->g_cost + start_node->h_cost;
     start_node->parent_index = -1;
     start_node->flight_index = -1;
     start_node->arrival_time = departure_time;
     start_node->next = NULL;
     
     enqueue(&open_set, start_node);
     
     // Keep track of best path to each airport
     double* best_cost = (double*)malloc(num_airports * sizeof(double));
     int* best_parent = (int*)malloc(num_airports * sizeof(int));
     int* best_flight = (int*)malloc(num_airports * sizeof(int));
     int* best_arrival = (int*)malloc(num_airports * sizeof(int));
     
     for (int i = 0; i < num_airports; i++) {
         best_cost[i] = INFINITY_COST;
         best_parent[i] = -1;
         best_flight[i] = -1;
         best_arrival[i] = -1;
     }
     
     best_cost[start_index] = 0.0;
     best_arrival[start_index] = departure_time;
     
     bool path_found = false;
     
     // A* main loop
     while (open_set != NULL) {
         Node* current = dequeue(&open_set);
         
         // Goal check
         if (current->airport_index == goal_index) {
             path_found = true;
             
             // Reconstruct path
             int current_airport = goal_index;
             int index = 0;
             
             // Trace back the path
             while (current_airport != start_index) {
                 path[index++] = best_flight[current_airport];
                 current_airport = best_parent[current_airport];
                 
                 if (index >= MAX_PATH || current_airport < 0) {
                     fprintf(stderr, "Path reconstruction error\n");
                     path_found = false;
                     break;
                 }
             }
             
             // Reverse the path
             for (int i = 0; i < index / 2; i++) {
                 int temp = path[i];
                 path[i] = path[index - i - 1];
                 path[index - i - 1] = temp;
             }
             
             *path_size = index;
             break;
         }
         
         // Mark as visited
         closed_set[current->airport_index] = true;
         
         // Check all possible flights from current airport
         for (int i = 0; i < num_flights; i++) {
             if (strcmp(flights[i].from, airports[current->airport_index].code) == 0) {
                 int next_index = find_airport_index(flights[i].to);
                 
                 // Skip if already in closed set
                 if (next_index < 0 || in_closed_set(closed_set, next_index)) {
                     continue;
                 }
                 
                 // Check if connection is possible (enough time between arrival and departure)
                 if (!is_connection_possible(current->arrival_time, flights[i].departure_time)) {
                     continue;
                 }
                 
                 // Calculate waiting time at airport
                 int wait_time = time_difference(current->arrival_time, flights[i].departure_time);
                 
                 // Calculate total cost (combines time and financial cost)
                 // You can adjust this formula based on preferences (time vs. money)
                 double time_cost_factor = 0.1;  // Cost unit per minute
                 double total_cost = current->g_cost + 
                                     flights[i].cost + 
                                     (flights[i].duration + wait_time) * time_cost_factor;
                 
                 // If this path is better
                 if (total_cost < best_cost[next_index]) {
                     best_cost[next_index] = total_cost;
                     best_parent[next_index] = current->airport_index;
                     best_flight[next_index] = i;
                     best_arrival[next_index] = flights[i].arrival_time;
                     
                     // Create and add new node to open set
                     Node* neighbor = (Node*)malloc(sizeof(Node));
                     neighbor->airport_index = next_index;
                     neighbor->g_cost = total_cost;
                     neighbor->h_cost = heuristic(next_index, goal_index, avg_cost_per_km);
                     neighbor->f_cost = neighbor->g_cost + neighbor->h_cost;
                     neighbor->parent_index = current->airport_index;
                     neighbor->flight_index = i;
                     neighbor->arrival_time = flights[i].arrival_time;
                     neighbor->next = NULL;
                     
                     enqueue(&open_set, neighbor);
                 }
             }
         }
         
         free(current);
     }
     
     // Clean up
     while (open_set != NULL) {
         Node* temp = dequeue(&open_set);
         free(temp);
     }
     
     free(closed_set);
     free(best_cost);
     free(best_parent);
     free(best_flight);
     free(best_arrival);
     
     return path_found;
 }
 
 // Write results to output JSON file
 bool write_json_output(const char* filename, int* path, int path_size, 
                        const char* start_code, const char* goal_code, int departure_time) {
     cJSON* root = cJSON_CreateObject();
     cJSON* journey = cJSON_CreateObject();
     cJSON* segments = cJSON_CreateArray();
     
     cJSON_AddItemToObject(root, "journey", journey);
     cJSON_AddStringToObject(journey, "origin", start_code);
     cJSON_AddStringToObject(journey, "destination", goal_code);
     
     char time_str[6];
     minutes_to_time(departure_time, time_str);
     cJSON_AddStringToObject(journey, "departure_time", time_str);
     
     double total_cost = 0.0;
     int total_duration = 0;
     int total_wait_time = 0;
     int arrival_time = departure_time;
     
     for (int i = 0; i < path_size; i++) {
         int flight_idx = path[i];
         Flight flight = flights[flight_idx];
         
         // Calculate waiting time
         int wait_time = 0;
         if (i == 0) {
             wait_time = time_difference(departure_time, flight.departure_time);
         } else {
             wait_time = time_difference(arrival_time, flight.departure_time);
         }
         
         cJSON* segment = cJSON_CreateObject();
         cJSON_AddStringToObject(segment, "from", flight.from);
         cJSON_AddStringToObject(segment, "to", flight.to);
         
         minutes_to_time(flight.departure_time, time_str);
         cJSON_AddStringToObject(segment, "departure_time", time_str);
         
         minutes_to_time(flight.arrival_time, time_str);
         cJSON_AddStringToObject(segment, "arrival_time", time_str);
         
         cJSON_AddNumberToObject(segment, "duration", flight.duration);
         cJSON_AddNumberToObject(segment, "cost", flight.cost);
         cJSON_AddNumberToObject(segment, "distance", flight.distance);
         
         if (i > 0) {
             cJSON_AddNumberToObject(segment, "connection_wait_time", wait_time);
         }
         
         cJSON_AddItemToArray(segments, segment);
         
         // Update totals
         total_cost += flight.cost;
         total_duration += flight.duration;
         if (i > 0) {
             total_wait_time += wait_time;
         }
         
         arrival_time = flight.arrival_time;
     }
     
     cJSON_AddItemToObject(journey, "segments", segments);
     cJSON_AddNumberToObject(journey, "total_cost", total_cost);
     cJSON_AddNumberToObject(journey, "total_flight_duration", total_duration);
     cJSON_AddNumberToObject(journey, "total_connection_wait_time", total_wait_time);
     cJSON_AddNumberToObject(journey, "total_journey_duration", total_duration + total_wait_time);
     
     minutes_to_time(arrival_time, time_str);
     cJSON_AddStringToObject(journey, "arrival_time", time_str);
     
     // Write JSON to file
     char* json_str = cJSON_Print(root);
     FILE* file = fopen(filename, "w");
     if (!file) {
         fprintf(stderr, "Error opening output file: %s\n", filename);
         cJSON_Delete(root);
         free(json_str);
         return false;
     }
     
     fputs(json_str, file);
     fclose(file);
     
     cJSON_Delete(root);
     free(json_str);
     return true;
 }
 
 int main(int argc, char* argv[]) {
     if (argc < 5) {
         printf("Usage: %s <input_json> <output_json> <start_airport> <goal_airport> [departure_time]\n", argv[0]);
         return 1;
     }
     
     const char* input_file = argv[1];
     const char* output_file = argv[2];
     const char* start_airport = argv[3];
     const char* goal_airport = argv[4];
     
     int departure_time = 480; // Default: 08:00
     if (argc > 5) {
         departure_time = time_to_minutes(argv[5]);
     }
     
     // Parse input JSON
     if (!parse_json_input(input_file)) {
         fprintf(stderr, "Failed to parse input file\n");
         return 1;
     }
     
     printf("Loaded %d airports and %d flights\n", num_airports, num_flights);
     
     // Find optimal path using A*
     int path[MAX_PATH];
     int path_size = 0;
     
     if (!find_optimal_path(start_airport, goal_airport, departure_time, path, &path_size)) {
         fprintf(stderr, "No viable path found from %s to %s\n", start_airport, goal_airport);
         return 1;
     }
     
     printf("Found optimal path with %d flights\n", path_size);
     
     // Write results to output JSON
     if (!write_json_output(output_file, path, path_size, start_airport, goal_airport, departure_time)) {
         fprintf(stderr, "Failed to write output file\n");
         return 1;
     }
     
     printf("Results written to %s\n", output_file);
     return 0;
 }