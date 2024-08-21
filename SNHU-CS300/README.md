# SNHU-CS300

## Description

This course focused on algorithmic design and problem-solving, with an emphasis on developing and evaluating data structures.

## Essential Questions

#### What was the problem you were solving in the projects for this course?

The main project for this course was to develop an advising assistance program that could efficiently manage a list of courses, their prerequisites, and corresponding course titles. The program allowed users to input data from a file as text seperated by commas, which was then loaded into a hashtable for efficient storage and retrieval. The program provided features to print courses in alphanumerical order and search for specific courses by their course number. Upon finding the course, the program outputted the course number, title, and prerequisites for that course.

#### How did you approach the problem? Consider why data structures are important to understand.

For the problem, the most important part was finding an efficient way to hold the data from the file. I found a hashtable as the most efficient way of storing the data based off the course numbers being unique. Therefore, I could create a key based off that hashtable. By understanding data structures, you can choose the most appropriate structure for the problem and optimize the program's performance. It allows efficient organization and manipulation of data, leading to more effective solutions.

#### How did you overcome any roadblocks you encountered while going through the activities or project?

I faced a significant challenge when implementing a prerequisite check to ensure that each course existed as a class. Initially, I attempted to perform the check in a single pass of the file, but encountered issues where the prerequisite appeared at the end of the list and was not recognized as an existing course. To overcome this hurdle, I decided to utilize more space and create a vector to store the first pass of course IDs. This vector allowed me to check against and verify the existence of each course. The approach that helped was stepping back and walking through it step by step, to make sure I was considering all approaches.

#### How has your work on this project expanded your approach to designing software and developing programs?

This project and course have opened my eyes to various options and ways to write code. Even though I had used many of these data structures before, I didn't fully understand how they were created behind the scenes. This emphasizes the significance of planning code and thinking about the different data structures that can be utilized. Sorting data is also crucial, as it can greatly impact the program's speed and efficiency.

#### How has your work on this project evolved the way you write programs that are maintainable, readable, and adaptable?

It's important not just make the code work, but it's also necessary to make it easy to understand and modify in the future. To achieve this, I started using consistent naming conventions for variables and functions, and added comments throughout the code to explain complex logic. I also separated the code into smaller functions, each with a specific purpose, making it easier to modify or add new features without affecting other parts of the code. Furthermore, I made sure to use data structures and algorithms that were appropriate for the task at hand, and that could easily handle changes or additions to the data. This made the code more adaptable to future changes or updates.
