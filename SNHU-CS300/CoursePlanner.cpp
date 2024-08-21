/*
// Name        : CoursePlanner.cpp
// Author      : Austin Henley
// Date        : 4/16/2022
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <climits>
#include <sstream>

using namespace std;


const unsigned int DEFAULT_SIZE = 179;

struct Course {
    string courseID;
    string courseName;
    vector<string> prerequisites;

    Course() = default;

    Course(string courseID, string courseName, vector<string> prerequisites) {
        this->courseID = courseID;
        this->courseName = courseName;
        this->prerequisites = prerequisites;
    }
};


/**
 * Define a class containing data members and methods to
 * implement a hash table with chaining.
 */
class HashTable {

private:
    // Define structures to hold courses
    struct Node {
        Course course;
        unsigned int key;
        Node* next;

        // default constructor
        Node() {
            key = UINT_MAX;
            next = nullptr;
        }

        // initialize with a course
        Node(Course aCourse) : Node() {
            course = aCourse;
        }

        // initialize with a course and a key
        Node(Course aCourse, unsigned int aKey) : Node(aCourse) {
            key = aKey;
        }
    };

    vector<Node> nodes;
    unsigned int tableSize = DEFAULT_SIZE;
    unsigned int hash(string key);

public:
    HashTable();
    HashTable(unsigned int size);
    virtual ~HashTable();
    void Insert(Course course);
    void printCourse(Course course);
    void printCourseList();
    Course* Search(string courseId);
    
};

/**
 * Default constructor
 */
HashTable::HashTable() {
    // Initalize node structure by resizing tableSize
    nodes.resize(tableSize);
}

/**
 * Constructor for specifying size of the table
 * Use to improve efficiency of hashing algorithm
 * by reducing collisions without wasting memory.
 */
HashTable::HashTable(unsigned int size) {
    // invoke local tableSize to size with this->
    this->tableSize = size;

    // resize nodes size
    nodes.resize(tableSize);
}


/**
 * Destructor
 */
HashTable::~HashTable() {
    // erase nodes beginning
    nodes.clear();
}

/**
 * Calculate the hash value of a given key.
 * Note that key is specifically defined as
 * unsigned int to prevent undefined results
 * of a negative list index.
 *
 * @param key The key to hash as string
 * @return The calculated hash
 */
unsigned int HashTable::hash(string key) {
    unsigned int hashVal = 0;
    // Loop through each character in the key
    for (int i = 0; i < key.length(); i++) {
        char c = key[i];
        // Multiply the current hash value by 33 and add the ASCII value of the current character
        // Modulo from tablesize to ensure it falls within range
        // Hash function sourced from https://theartincode.stanis.me/008-djb2/
        hashVal = (hashVal * 33 + (int)c) % tableSize;
    }
    // Return the final hash value
    return hashVal;
}


/*
* Insert course into hashtable
* 
* @param course Course to be inserted
*
*/
void HashTable::Insert(Course course) {
    // Check if course already exists
    if (Search(course.courseID) != nullptr) {
        std::cout << "Course " << course.courseID << " already exists." << std::endl;
        return;
    }

    // create the key for the given course
    int key = hash(course.courseID);

    // retrieve node using key
    Node* prevNode = &nodes[key];

    // if no entry found for the key
    if (prevNode->key == UINT_MAX) {
        // initialize the new node
        Node* newNode = new Node(course, key);
        // assign new node to the key position
        *prevNode = *newNode;
    }
    // else if node is not used
    else {
        // find next open node
        while (prevNode->next != nullptr) {
            prevNode = prevNode->next;
        }

        // add new node to end
        prevNode->next = new Node(course, key);
    }
}

/**
 * Search for the specified courseId
 *
 * @param courseId The course id to search for
 * @return the course object for the course
 */
Course* HashTable::Search(string courseId) {
    Course course;

    // create the key for the given course
    int key = hash(courseId);

    // retrieve node using key
    Node* currNode = &nodes[key];

    // search for node with matching courseId
    while (currNode != nullptr && currNode->course.courseID != courseId) {
        currNode = currNode->next;
    }

    // if node found, return course
    if (currNode != nullptr) {
        return &(currNode->course);
    }

    return nullptr;
}

/**
 * Print a specific course with prerequisites
 *
 * @param course The specific course to print
 */
void HashTable::printCourse(Course course) {
    // print course details
    cout << "Course: " << course.courseID << " - " << course.courseName << endl;

    // check if there are not prerequisites
    if (course.prerequisites.size() == 0) {
        cout << "No prerequisites" << endl;
    }
    else {
        // print prerequisites
        cout << "Prerequisites: ";
        for (int i = 0; i < course.prerequisites.size() - 1; i++) {
            cout << course.prerequisites[i] << " ";
        }
        cout << course.prerequisites[course.prerequisites.size() - 1] << endl;
    }
}

/*
* Sorts courselist hashtable into vector and prints in order
*
*/
void HashTable::printCourseList() {
    // Create a vector of nodes
    vector<Node*> nodeVector;

    // Iterate over nodes and add them to vector
    for (Node& node : nodes) {
        // Traverse linked list, if there is one
        while (node.key != UINT_MAX) {
            nodeVector.push_back(&node);
            if (node.next != nullptr) {
                node = *(node.next);
            }
            else {
                break;
            }
        }
    }

    // Sort the vector of nodes by courseID using insertion sort
    // Sourced from ZyBookz Insertion Sort Chapter https://learn.zybooks.com/zybook/CS-300-T4187-OL-TRAD-UG.23EW4/chapter/3/section/15
    int n = nodeVector.size();
    for (int i = 1; i < n; i++) {
        Node* key = nodeVector[i];
        int j = i - 1;
        while (j >= 0 && nodeVector[j]->course.courseID > key->course.courseID) {
            nodeVector[j + 1] = nodeVector[j];
            j = j - 1;
        }
        nodeVector[j + 1] = key;
    }

    // Print the course list
    for (Node* node : nodeVector) {
        cout << "Course: " << node->course.courseID << " - " << node->course.courseName << endl;
    }
}

/**
 * Load lines from file into course objects
 *
 * @param fileName The filename to load from
 * @param hashtable the hashtable to load into
 */
void loadCourses(string fileName, HashTable& hashtable) {
    // Open file for reading
    ifstream file(fileName);

    // Check if file did not open
    if (!file.is_open()) {
        cout << "Error: Could not open file." << endl;
        return;
    }

    // vector to store course IDs
    vector<string> courseIDs;
    string line;

    // First pass through file to store the first value of every line in courseIDs vector
    while (getline(file, line)) {
        //stringstream used to extract strings https://en.cppreference.com/w/cpp/io/basic_stringstream
        stringstream ss(line);
        string CourseID;

        // Get the first value in the line - Course ID, comma used as delimiter
        if (!getline(ss, CourseID, ',')) {
            // Error checking to make sure format is correct
            cout << "Error: Invalid input format." << endl;
            return;
        }

        // Add course to courses vector
        courseIDs.push_back(CourseID);
    }

    // Clear input
    file.clear();
    // Sets the position to the beginning of the file
    file.seekg(0, ios::beg);

    // Second pass to create the course objects and check prerequisites
    while (getline(file, line)) {
        stringstream ss(line);
        string CourseID, courseName, prereqCode;
        vector<string> prerequisites;

        // Store the first item from the line as courseID, comma used as delimiter
        if (!getline(ss, CourseID, ',')) {
            cout << "Error: Invalid input format." << endl;
            return;
        }

        // Store the second item from the line as courseName, comma used as delimiter
        if (!getline(ss, courseName, ',')) {
            cout << "Error: Invalid input format." << endl;
            return;
        }

        // read all prerequisites for a course, comma used as delimiter
        while (getline(ss, prereqCode, ',')) {
            // add prerequisites to the prereq vector
            prerequisites.push_back(prereqCode);
        }

        // Use course constructor to create new course
        Course newCourse(CourseID, courseName, prerequisites);

        // Create boolean for checking if prerequisite exists
        bool allPrereqsExist = true;
        string missingPrereq;
        // Check if all prerequisites exist in the courseIDs vector
        // For every string "prereq" in the prerequisites vector
        // Ranged for loop source https://www.geeksforgeeks.org/range-based-loop-c/#
        for (const string& prereq : prerequisites) {
            if (find(courseIDs.begin(), courseIDs.end(), prereq) == courseIDs.end()) {
                allPrereqsExist = false;
                missingPrereq = prereq;
                break;
            }
        }

        // if all prerequisites exist, add this course to the hashtable
        if (allPrereqsExist) {
            hashtable.Insert(newCourse);
        }
        // otherwise, notify user that course will not be added
        else {
            cout << "Error: Prerequisite " << missingPrereq << " does not exist." << endl;
            cout << newCourse.courseID << " not loaded into table. Please reload with new input file." << endl;
        }
    }

    file.close();
}


/*
* Display the main menu - Loop while user selects options
*
*/
void showMenu() {
    int choice;
    string coursePrompt, fileName;
    Course* result;

    // create an instance of the HashTable class
    HashTable courseTable;


    do {
        cout << "\nMenu:\n";
        cout << "1. Load Data Structure\n";
        cout << "2. Print Course List\n";
        cout << "3. Print Course\n";
        cout << "4. Exit\n";
        cout << "Enter choice: ";
        if (!(cin >> choice)) {
            // Input is not a valid integer
            cout << "Invalid input. Please enter a number.\n";
            // Clear the fail
            cin.clear(); 
            // Ignore remaining characters in the input buffer
            // https://en.cppreference.com/w/cpp/string/basic_string/getline
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            continue;
        }
        switch (choice) {
        case 1:
            // Ask the user for the file name and store it into variable
            cout << "Enter file name: ";
            cin >> fileName;
            // Load courses from the file indicated by the user
            loadCourses(fileName, courseTable);
            break;
        case 2:
            // Print hashtable course list
            courseTable.printCourseList();
            break;
        case 3:
            // Ask user for course ID they are searching for and store in variable
            cout << "Which course do you want to know about?" << endl;
            cin >> coursePrompt;
            // Call search using the user input
            result = courseTable.Search(coursePrompt);
            // If search returns nullptr, notify user
            if (result == nullptr) {
                cout << "Course not found." << endl;
            }
            // If search returns a course, print using printCourse method
            else {
                courseTable.printCourse(*result);
            }
            break;
        case 4:
            // Exit do-while loop on 4
            cout << "Exiting program.\n";
            break;
        default:
            cout << "Invalid choice.\n";
        }
    } while (choice != 4);
}

int main() {


    cout << "Welcome to the course planner!" << endl;

    showMenu();

    cout << "Thank you for using the course planner!" << endl;

    return 0;
}