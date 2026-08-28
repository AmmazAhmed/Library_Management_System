# Library Management System

A **Library Management System** developed in **C++** using **Object-Oriented Programming (OOP)** concepts.

> **Note:** This is a **console/CLI-based project**. It does **not** have a GUI.

## Features

## Admin
- Admin login
- Add, view, search, update & delete books
- Register, view, search, update & delete students
- Issue and return books
- Track overdue books
- Calculate fines
- Generate library reports

## Student
- Student login using Student ID
- Search books
- View available books
- View borrowed books

## OOP Concepts Used

- **Classes & Objects**
- **Inheritance** – `Student` inherits from `Person`
- **Encapsulation**
- **Abstraction**
- **Polymorphism**
- **Constructors**
- **STL Vectors**

The project uses classes such as `Person`, `Book`, `Student`, `Issue`, and `LibrarySystem`.

## Data Storage

The system uses **text files** for data persistence:

| File | Purpose |
|---|---|
| `admin.txt` | Admin credentials |
| `books.txt` | Book records |
| `students.txt` | Student records |
| `issued.txt` | Issue/return records |

Data is automatically loaded when the program starts and saved when changes are made.

## Default Admin Login

If `admin.txt` does not exist:

```text
Username: admin
Password: admin123
```

## Fine System

- Default borrowing period: **7 days**
- Fine: **Rs. 20 per overdue day**



## Technologies

- C++
- OOP
- STL `vector`
- File Handling
- Console Input/Output
- Date & Time Functions



## How to Run

## Clone the Repository
```bash
git clone https://github.com/AmmazAhmed/Library_Management_System
cd Library_Management_System
```

### Compile

```bash
g++ library_management.cpp -o library_management
```

### Run

**Windows:**
```bash
library_management.exe
```

**Linux/macOS:**
```bash
./library_management
```

## Future Improvements

*  Add a **Graphical User Interface (GUI)** for easier interaction
*  Integrate a **database** such as MySQL or PostgreSQL
*  Implement **secure authentication** with password hashing
*  Add **advanced reports, notifications, and book reservation** features

## Author

**Ammaz Ahmed**

---

