/**
 * Enhanced Air Travel Planning System with JSON compatibility
 * Improvements:
 * 1. Robust error handling for JSON parsing
 * 2. Efficient priority queue (min-heap)
 * 3. Validation for airport codes and flight data
 * 4. Clear error messages
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdbool.h>
 #include <float.h>
 #include <math.h>
 #include <time.h>
 #include <ctype.h>
 #include "cJSON/cJSON.h"
 
 /* Constants */
 #define MAX_AIRPORTS 100
 #define MAX_FLIGHTS 1000
 #define MAX_PATH 50
 #define INFINITY_COST 999999.0
 #define MAX_QUEUE_SIZE 10000
 #define MAX_DAY_LENGTH 10
 #define MAX_TIME_LENGTH 6
 
 /* Data Structures */
 typedef enum {
     CHEAPEST,
     FASTEST,
     OPTIMAL
 } RouteType;
 
 typedef struct {
     char code[4];  // IATA code (3 chars + null)
     char name[100];
     double lat;
     double lon;
     int min_waiting_time;
 } Airport;
 
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
 
 /* Min-Heap Priority Queue */
 typedef struct {
     Node* nodes[MAX_QUEUE_SIZE];
     int size;
 } PriorityQueue;
 
 /* Global Variables */
 Airport airports[MAX_AIRPORTS];
 ScheduledFlight flights[MAX_FLIGHTS];
 int num_airports = 0;
 int num_flights = 0;
 int connection_time_required = 60;
 
 const char* days_of_week[] = {
     "monday", "tuesday", "wednesday", "thursday", 
     "friday", "saturday", "sunday"
 };
 
 /* Priority Queue Implementation (Min-Heap) */
 void pq_init(PriorityQueue* q) {
     q->size = 0;
 }
 
 void pq_swap(PriorityQueue* q, int i, int j) {
     Node* temp = q->nodes[i];
     q->nodes[i] = q->nodes[j];
     q->nodes[j] = temp;
 }
 
 void pq_enqueue(PriorityQueue* q, Node* node) {
     if (q->size >= MAX_QUEUE_SIZE) {
         fprintf(stderr, "Error: Priority queue overflow\n");
         return;
     }
 
     q->nodes[q->size] = node;
     int current = q->size++;
     
     // Heapify up
     while (current > 0 && 
            q->nodes[current]->f_cost < q->nodes[(current-1)/2]->f_cost) {
         pq_swap(q, current, (current-1)/2);
         current = (current-1)/2;
     }
 }
 
 Node* pq_dequeue(PriorityQueue* q) {
     if (q->size == 0) return NULL;
     
     Node* min = q->nodes[0];
     q->nodes[0] = q->nodes[--q->size];
     
     // Heapify down
     int current = 0;
     while (1) {
         int left = 2*current + 1;
         int right = 2*current + 2;
         int smallest = current;
         
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
 
 /* Enhanced JSON Parsing with Error Checking */
 bool validate_airport_code(const char* code) {
     if (strlen(code) != 3) return false;
     for (int i = 0; i < 3; i++) {
         if (!isalpha(code[i])) return false;
     }
     return true;
 }
 
 bool parse_json_input(const char* filename) {
     FILE* file = fopen(filename, "r");
     if (!file) {
         fprintf(stderr, "Error: Cannot open file %s\n", filename);
         return false;
     }
 
     fseek(file, 0, SEEK_END);
     long file_size = ftell(file);
     fseek(file, 0, SEEK_SET);
     
     char* json_str = (char*)malloc(file_size + 1);
     if (!json_str) {
         fclose(file);
         fprintf(stderr, "Error: Memory allocation failed\n");
         return false;
     }
     
     fread(json_str, 1, file_size, file);
     json_str[file_size] = '\0';
     fclose(file);
     
     cJSON* json = cJSON_Parse(json_str);
     free(json_str);
     
     if (!json) {
         fprintf(stderr, "Error: Invalid JSON format\n");
         return false;
     }
 
     /* Parse airports with validation */
     cJSON* airports_json = cJSON_GetObjectItem(json, "airports");
     if (!airports_json) {
         fprintf(stderr, "Error: Missing 'airports' in JSON\n");
         cJSON_Delete(json);
         return false;
     }
 
     int airport_count = cJSON_GetArraySize(airports_json);
     for (int i = 0; i < airport_count && num_airports < MAX_AIRPORTS; i++) {
         cJSON* airport = cJSON_GetArrayItem(airports_json, i);
         
         cJSON* code = cJSON_GetObjectItem(airport, "code");
         cJSON* name = cJSON_GetObjectItem(airport, "name");
         cJSON* lat = cJSON_GetObjectItem(airport, "latitude");
         cJSON* lon = cJSON_GetObjectItem(airport, "longitude");
         
         if (!code || !name || !lat || !lon) {
             fprintf(stderr, "Warning: Incomplete airport data at index %d\n", i);
             continue;
         }
 
         if (!validate_airport_code(code->valuestring)) {
             fprintf(stderr, "Warning: Invalid airport code '%s'\n", code->valuestring);
             continue;
         }
 
         strncpy(airports[num_airports].code, code->valuestring, 3);
         airports[num_airports].code[3] = '\0';
         strncpy(airports[num_airports].name, name->valuestring, 99);
         airports[num_airports].name[99] = '\0';
         airports[num_airports].lat = lat->valuedouble;
         airports[num_airports].lon = lon->valuedouble;
         
         cJSON* min_wait = cJSON_GetObjectItem(airport, "min_waiting_time");
         airports[num_airports].min_waiting_time = min_wait ? min_wait->valueint : connection_time_required;
         
         num_airports++;
     }
 
     /* Parse flights with validation */
     cJSON* flights_json = cJSON_GetObjectItem(json, "flights");
     if (!flights_json) {
         fprintf(stderr, "Error: Missing 'flights' in JSON\n");
         cJSON_Delete(json);
         return false;
     }
 
     int flight_count = cJSON_GetArraySize(flights_json);
     for (int i = 0; i < flight_count && num_flights < MAX_FLIGHTS; i++) {
         cJSON* flight_info = cJSON_GetArrayItem(flights_json, i);
         
         cJSON* from = cJSON_GetObjectItem(flight_info, "from");
         cJSON* to = cJSON_GetObjectItem(flight_info, "to");
         cJSON* base_cost = cJSON_GetObjectItem(flight_info, "base_cost");
         cJSON* schedule = cJSON_GetObjectItem(flight_info, "schedule");
         
         if (!from || !to || !base_cost || !schedule) {
             fprintf(stderr, "Warning: Incomplete flight data at index %d\n", i);
             continue;
         }
 
         if (!validate_airport_code(from->valuestring) || !validate_airport_code(to->valuestring)) {
             fprintf(stderr, "Warning: Invalid airport codes in flight %d\n", i);
             continue;
         }
 
         /* Process schedule for each day */
         for (int day = 0; day < 7; day++) {
             cJSON* day_schedule = cJSON_GetObjectItem(schedule, days_of_week[day]);
             if (!day_schedule) continue;
             
             cJSON* departure = cJSON_GetObjectItem(day_schedule, "departure_time");
             cJSON* arrival = cJSON_GetObjectItem(day_schedule, "arrival_time");
             cJSON* cost_multiplier = cJSON_GetObjectItem(day_schedule, "cost_multiplier");
             cJSON* available = cJSON_GetObjectItem(day_schedule, "available");
             
             if (!departure || !arrival || !cost_multiplier || !available) {
                 fprintf(stderr, "Warning: Incomplete schedule for %s\n", days_of_week[day]);
                 continue;
             }
 
             if (available->valueint || available->type == cJSON_True) {
                 ScheduledFlight* f = &flights[num_flights];
                 
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
                 
                 cJSON* distance = cJSON_GetObjectItem(flight_info, "distance");
                 if (distance) {
                     f->distance = distance->valuedouble;
                 } else {
                     // Calculate distance if not provided
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
 
     /* Parse configuration */
     cJSON* config = cJSON_GetObjectItem(json, "config");
     if (config) {
         cJSON* conn_time = cJSON_GetObjectItem(config, "min_connection_time");
         if (conn_time) connection_time_required = conn_time->valueint;
     }
 
     cJSON_Delete(json);
     
     if (num_airports == 0 || num_flights == 0) {
         fprintf(stderr, "Error: No valid airports or flights found\n");
         return false;
     }
     
     return true;
 }
 
 /* Main A* Algorithm with Enhanced Priority Queue */
 bool find_optimal_path(const char* start_code, const char* goal_code, 
                       const char* start_day, int departure_time, 
                       RouteType route_type, int* path, int* path_size) {
     int start_index = find_airport_index(start_code);
     int goal_index = find_airport_index(goal_code);
     
     if (start_index < 0 || goal_index < 0) {
         fprintf(stderr, "Error: Invalid airport codes (%s or %s not found)\n", 
                 start_code, goal_code);
         return false;
     }
 
     /* Initialize data structures */
     bool* closed_set = (bool*)calloc(num_airports, sizeof(bool));
     PriorityQueue open_set;
     pq_init(&open_set);
     
     /* Track best paths */
     double* best_cost = (double*)malloc(num_airports * sizeof(double));
     int* best_parent = (int*)malloc(num_airports * sizeof(int));
     int* best_flight = (int*)malloc(num_airports * sizeof(int));
     int* best_arrival_time = (int*)malloc(num_airports * sizeof(int));
     char** best_arrival_day = (char**)malloc(num_airports * sizeof(char*));
     
     for (int i = 0; i < num_airports; i++) {
         best_cost[i] = INFINITY_COST;
         best_parent[i] = -1;
         best_flight[i] = -1;
         best_arrival_time[i] = -1;
         best_arrival_day[i] = (char*)malloc(MAX_DAY_LENGTH * sizeof(char));
         strcpy(best_arrival_day[i], "");
     }
     
     /* Create and enqueue start node */
     Node* start_node = (Node*)malloc(sizeof(Node));
     start_node->airport_index = start_index;
     start_node->g_cost = 0.0;
     start_node->h_cost = heuristic(start_index, goal_index, route_type);
     start_node->f_cost = start_node->g_cost + start_node->h_cost;
     start_node->parent_index = -1;
     start_node->flight_index = -1;
     start_node->arrival_time = departure_time;
     strncpy(start_node->arrival_day, start_day, MAX_DAY_LENGTH-1);
     
     best_cost[start_index] = 0.0;
     best_arrival_time[start_index] = departure_time;
     strcpy(best_arrival_day[start_index], start_day);
     
     pq_enqueue(&open_set, start_node);
     
     bool path_found = false;
     int expanded_nodes = 0;
     
     /* Main A* loop */
     while (open_set.size > 0 && expanded_nodes < 10000) {
         Node* current = pq_dequeue(&open_set);
         expanded_nodes++;
         
         if (current->airport_index == goal_index) {
             /* Reconstruct path */
             int current_airport = goal_index;
             int index = 0;
             
             while (current_airport != start_index && index < MAX_PATH) {
                 path[index++] = best_flight[current_airport];
                 current_airport = best_parent[current_airport];
             }
             
             if (current_airport == start_index) {
                 /* Reverse the path */
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
         
         if (closed_set[current->airport_index]) {
             free(current);
             continue;
         }
         
         closed_set[current->airport_index] = true;
         
         /* Explore neighbors */
         for (int i = 0; i < num_flights; i++) {
             if (strcmp(flights[i].from, airports[current->airport_index].code) != 0)
                 continue;
                 
             int next_index = find_airport_index(flights[i].to);
             if (next_index < 0 || closed_set[next_index])
                 continue;
                 
             /* Check connection feasibility */
             int min_connection = airports[current->airport_index].min_waiting_time;
             if (!is_connection_possible(current->arrival_time, flights[i].departure_time,
                                       current->arrival_day, flights[i].day_of_week,
                                       min_connection))
                 continue;
                 
             /* Calculate costs */
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
             
             if (total_cost < best_cost[next_index]) {
                 best_cost[next_index] = total_cost;
                 best_parent[next_index] = current->airport_index;
                 best_flight[next_index] = i;
                 best_arrival_time[next_index] = flights[i].arrival_time;
                 
                 /* Calculate arrival day */
                 if (flights[i].arrival_time < flights[i].departure_time) {
                     get_next_day(flights[i].day_of_week, best_arrival_day[next_index]);
                 } else {
                     strcpy(best_arrival_day[next_index], flights[i].day_of_week);
                 }
                 
                 /* Create and enqueue neighbor node */
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
         
         free(current);
     }
     
     /* Cleanup */
     while (open_set.size > 0) {
         free(pq_dequeue(&open_set));
     }
     
     free(closed_set);
     free(best_cost);
     free(best_parent);
     free(best_flight);
     free(best_arrival_time);
     
     for (int i = 0; i < num_airports; i++) {
         free(best_arrival_day[i]);
     }
     free(best_arrival_day);
     
     if (!path_found) {
         fprintf(stderr, "Error: No viable path found from %s to %s\n", 
                 start_code, goal_code);
     }
     
     return path_found;
 }
 