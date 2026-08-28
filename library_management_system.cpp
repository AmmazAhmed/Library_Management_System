
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
#include <sstream>

using namespace std;

// ---------------- Global Constants ----------------
const string ADMIN_FILE = "admin.txt";
const string BOOKS_FILE = "books.txt";
const string STUDENTS_FILE = "students.txt";
const string ISSUED_FILE = "issued.txt";
const int FINE_PER_DAY = 20;

// ---------------- Utility Functions ----------------

// Returns today's date as dd/mm/yyyy
string getCurrentDate()
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    stringstream ss;
    ss << setfill('0') << setw(2) << ltm->tm_mday << "/"
       << setfill('0') << setw(2) << (1 + ltm->tm_mon) << "/"
       << (1900 + ltm->tm_year);
    return ss.str();
}

// Returns a due date "days" days from today (default 7 days)
string getDueDate(int days = 7)
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    ltm->tm_mday += days;
    mktime(ltm); // normalizes the date (handles month/year rollover)
    stringstream ss;
    ss << setfill('0') << setw(2) << ltm->tm_mday << "/"
       << setfill('0') << setw(2) << (1 + ltm->tm_mon) << "/"
       << (1900 + ltm->tm_year);
    return ss.str();
}

// Returns number of days between date1 and date2 (both dd/mm/yyyy)
int daysBetween(const string &date1, const string &date2)
{
    int d1, m1, y1, d2, m2, y2;
    char slash;
    stringstream ss1(date1), ss2(date2);
    ss1 >> d1 >> slash >> m1 >> slash >> y1;
    ss2 >> d2 >> slash >> m2 >> slash >> y2;

    tm tm1 = {};
    tm1.tm_mday = d1;
    tm1.tm_mon = m1 - 1;
    tm1.tm_year = y1 - 1900;

    tm tm2 = {};
    tm2.tm_mday = d2;
    tm2.tm_mon = m2 - 1;
    tm2.tm_year = y2 - 1900;

    time_t t1 = mktime(&tm1);
    time_t t2 = mktime(&tm2);

    return (int)(difftime(t2, t1) / (60 * 60 * 24));
}

// ================================================================
// Base Person Class (Book does NOT inherit from this, only Student)
// ================================================================
class Person
{
protected:
    int id;
    string name;
    string phone;
    string email;

public:
    Person() : id(0) {}
    Person(int id, string name, string phone, string email)
        : id(id), name(name), phone(phone), email(email) {}

    virtual void display() = 0; // must be implemented by child class

    int getId() const { return id; }
    string getName() const { return name; }
    string getPhone() const { return phone; }
    string getEmail() const { return email; }
};

// ================================================================
// Book Class
// ================================================================
class Book
{
private:
    int bookID;
    string title;
    string author;
    string category;
    string publication;
    int edition;
    int quantity;
    int available;

public:
    Book() : bookID(0), edition(1), quantity(0), available(0) {}

    Book(int id, string t, string a, string c, string p, int e, int q)
        : bookID(id), title(t), author(a), category(c), publication(p),
          edition(e), quantity(q), available(q) {}

    // Ask the admin for book details from the keyboard
    void addBook()
    {
        cout << "\n--- Add New Book ---\n";
        cout << "Book ID: ";
        cin >> bookID;
        cin.ignore();
        cout << "Title: ";
        getline(cin, title);
        cout << "Author: ";
        getline(cin, author);
        cout << "Category: ";
        getline(cin, category);
        cout << "Publication: ";
        getline(cin, publication);
        cout << "Edition: ";
        cin >> edition;
        cout << "Quantity: ";
        cin >> quantity;
        available = quantity;
    }

    // Let the admin change one field (or all fields) of the book
    void updateBook()
    {
        int choice;
        cout << "\n--- Update Book ---\n";
        cout << "1. Title\n2. Author\n3. Category\n4. Publication\n";
        cout << "5. Edition\n6. Quantity\n7. All\nEnter choice: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1)
        {
            cout << "New Title: ";
            getline(cin, title);
        }
        else if (choice == 2)
        {
            cout << "New Author: ";
            getline(cin, author);
        }
        else if (choice == 3)
        {
            cout << "New Category: ";
            getline(cin, category);
        }
        else if (choice == 4)
        {
            cout << "New Publication: ";
            getline(cin, publication);
        }
        else if (choice == 5)
        {
            cout << "New Edition: ";
            cin >> edition;
        }
        else if (choice == 6)
        {
            cout << "New Quantity: ";
            int newQty;
            cin >> newQty;
            // Keep "available" in sync when quantity changes
            available += (newQty - quantity);
            if (available < 0)
                available = 0;
            quantity = newQty;
        }
        else if (choice == 7)
        {
            addBook();
        }
        else
        {
            cout << "Invalid choice!\n";
        }
    }

    void display() const
    {
        cout << "ID: " << bookID
             << " | Title: " << title
             << " | Author: " << author
             << " | Category: " << category
             << " | Pub: " << publication
             << " | Ed: " << edition
             << " | Qty: " << quantity
             << " | Available: " << available << endl;
    }

    // ---- Getters / Setters ----
    int getBookID() const { return bookID; }
    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    string getCategory() const { return category; }
    int getQuantity() const { return quantity; }
    int getAvailable() const { return available; }
    void setAvailable(int a) { available = a; }
    void setQuantity(int q) { quantity = q; }

    // ---- File I/O ----
    // Writes one book as one line of text, fields separated by '|'
    void saveToFile(ofstream &out) const
    {
        out << bookID << "|" << title << "|" << author << "|"
            << category << "|" << publication << "|" << edition << "|"
            << quantity << "|" << available << "\n";
    }

    // Reads one line and fills this Book's fields.
    // Returns true if a line was successfully read, false if we hit
    // the end of the file (this is the fix for your compile error).
    bool loadFromFile(ifstream &in)
    {
        string line;
        if (!getline(in, line))
        {
            return false; // end of file, nothing more to read
        }
        if (line.empty())
        {
            return false;
        }

        stringstream ss(line);
        string token;

        getline(ss, token, '|');
        bookID = stoi(token);
        getline(ss, title, '|');
        getline(ss, author, '|');
        getline(ss, category, '|');
        getline(ss, publication, '|');
        getline(ss, token, '|');
        edition = stoi(token);
        getline(ss, token, '|');
        quantity = stoi(token);
        getline(ss, token, '|');
        available = stoi(token);

        return true;
    }
};

// ================================================================
// Student Class (inherits from Person)
// ================================================================
class Student : public Person
{
private:
    string department;
    int semester;
    vector<int> borrowedBooks;

public:
    Student() : Person(), semester(1) {}

    Student(int id, string name, string phone, string email,
            string dept, int sem)
        : Person(id, name, phone, email), department(dept), semester(sem) {}

    void addStudent()
    {
        cout << "\n--- Register Student ---\n";
        cout << "Student ID: ";
        cin >> id;
        cin.ignore();
        cout << "Name: ";
        getline(cin, name);
        cout << "Phone: ";
        getline(cin, phone);
        cout << "Email: ";
        getline(cin, email);
        cout << "Department: ";
        getline(cin, department);
        cout << "Semester: ";
        cin >> semester;
    }

    void updateStudent()
    {
        int choice;
        cout << "\n--- Update Student ---\n";
        cout << "1. Name\n2. Phone\n3. Email\n4. Department\n";
        cout << "5. Semester\n6. All\nEnter choice: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1)
        {
            cout << "New Name: ";
            getline(cin, name);
        }
        else if (choice == 2)
        {
            cout << "New Phone: ";
            getline(cin, phone);
        }
        else if (choice == 3)
        {
            cout << "New Email: ";
            getline(cin, email);
        }
        else if (choice == 4)
        {
            cout << "New Department: ";
            getline(cin, department);
        }
        else if (choice == 5)
        {
            cout << "New Semester: ";
            cin >> semester;
        }
        else if (choice == 6)
        {
            addStudent();
        }
        else
        {
            cout << "Invalid choice!\n";
        }
    }

    void display() override
    {
        cout << "ID: " << id
             << " | Name: " << name
             << " | Dept: " << department
             << " | Sem: " << semester
             << " | Phone: " << phone
             << " | Email: " << email
             << " | Books Borrowed: " << borrowedBooks.size() << endl;
    }

    // ---- Getters / Setters ----
    string getDepartment() const { return department; }
    int getSemester() const { return semester; }
    vector<int> getBorrowedBooks() const { return borrowedBooks; }

    void addBorrowedBook(int bookID)
    {
        borrowedBooks.push_back(bookID);
    }

    void removeBorrowedBook(int bookID)
    {
        for (size_t i = 0; i < borrowedBooks.size(); i++)
        {
            if (borrowedBooks[i] == bookID)
            {
                borrowedBooks.erase(borrowedBooks.begin() + i);
                break;
            }
        }
    }

    // ---- File I/O ----
    void saveToFile(ofstream &out) const
    {
        out << id << "|" << name << "|" << phone << "|" << email << "|"
            << department << "|" << semester << "|";
        for (size_t i = 0; i < borrowedBooks.size(); i++)
        {
            out << borrowedBooks[i];
            if (i < borrowedBooks.size() - 1)
                out << ",";
        }
        out << "\n";
    }

    // Returns true if a student record was read, false at end of file.
    bool loadFromFile(ifstream &in)
    {
        string line;
        if (!getline(in, line))
        {
            return false;
        }
        if (line.empty())
        {
            return false;
        }

        stringstream ss(line);
        string token;

        getline(ss, token, '|');
        id = stoi(token);
        getline(ss, name, '|');
        getline(ss, phone, '|');
        getline(ss, email, '|');
        getline(ss, department, '|');
        getline(ss, token, '|');
        semester = stoi(token);
        getline(ss, token, '|'); // the borrowed-books list (comma separated)

        borrowedBooks.clear();
        stringstream bookStream(token);
        string bookToken;
        while (getline(bookStream, bookToken, ','))
        {
            if (!bookToken.empty())
            {
                borrowedBooks.push_back(stoi(bookToken));
            }
        }

        return true;
    }
};

// ================================================================
// Issue Class (represents one "book borrowed by a student" record)
// ================================================================
class Issue
{
private:
    int issueID;
    int studentID;
    int bookID;
    string issueDate;
    string dueDate;
    string returnDate;
    bool isReturned;
    int fine;

public:
    Issue() : issueID(0), studentID(0), bookID(0),
              isReturned(false), fine(0) {}

    Issue(int id, int sID, int bID)
        : issueID(id), studentID(sID), bookID(bID),
          issueDate(getCurrentDate()), dueDate(::getDueDate(7)),
          returnDate(""), isReturned(false), fine(0) {}

    void issueBook()
    {
        cout << "\n--- Issue Book ---\n";
        cout << "Student ID: ";
        cin >> studentID;
        cout << "Book ID: ";
        cin >> bookID;
        issueID = studentID * 100 + bookID;
        issueDate = getCurrentDate();
        // Use ::getDueDate to call the global function, not the
        // member function below that shares the same name.
        dueDate = ::getDueDate(7);
        isReturned = false;
        fine = 0;
        returnDate = "";
    }

    void returnBook()
    {
        returnDate = getCurrentDate();
        isReturned = true;

        int daysLate = daysBetween(dueDate, returnDate);
        if (daysLate > 0)
        {
            fine = daysLate * FINE_PER_DAY;
        }
        else
        {
            fine = 0;
        }

        cout << "\nBook Returned Successfully!\n";
        if (fine > 0)
        {
            cout << "Late by " << daysLate << " days. Fine: Rs. " << fine << endl;
        }
    }

    void display() const
    {
        cout << "Issue ID: " << issueID
             << " | Student ID: " << studentID
             << " | Book ID: " << bookID
             << " | Issued: " << issueDate
             << " | Due: " << dueDate;
        if (isReturned)
        {
            cout << " | Returned: " << returnDate
                 << " | Fine: Rs." << fine;
        }
        else
        {
            cout << " | Status: Not Returned";
        }
        cout << endl;
    }

    // ---- Getters ----
    int getIssueID() const { return issueID; }
    int getStudentID() const { return studentID; }
    int getBookID() const { return bookID; }
    string getIssueDate() const { return issueDate; }
    string getDueDate() const { return dueDate; }
    string getReturnDate() const { return returnDate; }
    bool getIsReturned() const { return isReturned; }
    int getFine() const { return fine; }

    // ---- File I/O ----
    void saveToFile(ofstream &out) const
    {
        out << issueID << "|" << studentID << "|" << bookID << "|"
            << issueDate << "|" << dueDate << "|" << returnDate << "|"
            << isReturned << "|" << fine << "\n";
    }

    // Returns true if an issue record was read, false at end of file.
    bool loadFromFile(ifstream &in)
    {
        string line;
        if (!getline(in, line))
        {
            return false;
        }
        if (line.empty())
        {
            return false;
        }

        stringstream ss(line);
        string token;

        getline(ss, token, '|');
        issueID = stoi(token);
        getline(ss, token, '|');
        studentID = stoi(token);
        getline(ss, token, '|');
        bookID = stoi(token);
        getline(ss, issueDate, '|');
        getline(ss, dueDate, '|');
        getline(ss, returnDate, '|');
        getline(ss, token, '|');
        isReturned = (token == "1");
        getline(ss, token, '|');
        fine = stoi(token);

        return true;
    }
};

// ================================================================
// Library System Class - ties everything together
// ================================================================
class LibrarySystem
{
private:
    vector<Book> books;
    vector<Student> students;
    vector<Issue> issues;

    // ---------- Simple search helpers (replace find_if + lambda) ----------

    // Returns a pointer to the book with this ID, or nullptr if not found
    Book *findBookByID(int id)
    {
        for (size_t i = 0; i < books.size(); i++)
        {
            if (books[i].getBookID() == id)
            {
                return &books[i];
            }
        }
        return nullptr;
    }

    // Returns a pointer to the student with this ID, or nullptr if not found
    Student *findStudentByID(int id)
    {
        for (size_t i = 0; i < students.size(); i++)
        {
            if (students[i].getId() == id)
            {
                return &students[i];
            }
        }
        return nullptr;
    }

    // Returns a pointer to the active (not yet returned) issue that matches
    // this student ID and book ID, or nullptr if there isn't one
    Issue *findActiveIssue(int studentID, int bookID)
    {
        for (size_t i = 0; i < issues.size(); i++)
        {
            if (issues[i].getStudentID() == studentID &&
                issues[i].getBookID() == bookID &&
                !issues[i].getIsReturned())
            {
                return &issues[i];
            }
        }
        return nullptr;
    }

public:
    LibrarySystem()
    {
        loadData();
    }

    ~LibrarySystem()
    {
        saveData();
    }

    void loadData()
    {
        loadBooks();
        loadStudents();
        loadIssues();
    }

    void saveData()
    {
        saveBooks();
        saveStudents();
        saveIssues();
    }

    // ---------- Load / Save each file ----------

    void loadBooks()
    {
        books.clear();
        ifstream file(BOOKS_FILE);
        if (file.is_open())
        {
            Book book;
            while (book.loadFromFile(file))
            {
                books.push_back(book);
            }
            file.close();
        }
    }

    void saveBooks()
    {
        ofstream file(BOOKS_FILE);
        if (file.is_open())
        {
            for (size_t i = 0; i < books.size(); i++)
            {
                books[i].saveToFile(file);
            }
            file.close();
        }
    }

    void loadStudents()
    {
        students.clear();
        ifstream file(STUDENTS_FILE);
        if (file.is_open())
        {
            Student student;
            while (student.loadFromFile(file))
            {
                students.push_back(student);
            }
            file.close();
        }
    }

    void saveStudents()
    {
        ofstream file(STUDENTS_FILE);
        if (file.is_open())
        {
            for (size_t i = 0; i < students.size(); i++)
            {
                students[i].saveToFile(file);
            }
            file.close();
        }
    }

    void loadIssues()
    {
        issues.clear();
        ifstream file(ISSUED_FILE);
        if (file.is_open())
        {
            Issue issue;
            while (issue.loadFromFile(file))
            {
                issues.push_back(issue);
            }
            file.close();
        }
    }

    void saveIssues()
    {
        ofstream file(ISSUED_FILE);
        if (file.is_open())
        {
            for (size_t i = 0; i < issues.size(); i++)
            {
                issues[i].saveToFile(file);
            }
            file.close();
        }
    }

    // ---------- Admin ----------

    bool adminLogin()
    {
        string username, password;
        cout << "\n--- Admin Login ---\n";
        cout << "Username: ";
        cin >> username;
        cout << "Password: ";
        cin >> password;

        ifstream file(ADMIN_FILE);
        if (file.is_open())
        {
            string storedUser, storedPass;
            getline(file, storedUser);
            getline(file, storedPass);
            file.close();
            return (username == storedUser && password == storedPass);
        }

        // Default login if admin.txt does not exist yet
        return (username == "admin" && password == "admin123");
    }

    // ---------- Book management ----------

    void addBook()
    {
        Book book;
        book.addBook();

        if (findBookByID(book.getBookID()) != nullptr)
        {
            cout << "Book with ID " << book.getBookID() << " already exists!\n";
            return;
        }

        books.push_back(book);
        saveData();
        cout << "Book added successfully!\n";
    }

    void viewBooks()
    {
        if (books.empty())
        {
            cout << "No books available.\n";
            return;
        }
        cout << "\n--- All Books ---\n";
        for (size_t i = 0; i < books.size(); i++)
        {
            books[i].display();
        }
    }

    void searchBook()
    {
        if (books.empty())
        {
            cout << "No books available.\n";
            return;
        }

        int choice;
        cout << "\n--- Search Book ---\n";
        cout << "1. By ID\n2. By Title\n3. By Author\n4. By Category\n";
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore();

        string keyword;
        int id;
        bool found = false;

        if (choice == 1)
        {
            cout << "Enter Book ID: ";
            cin >> id;
            Book *b = findBookByID(id);
            if (b != nullptr)
            {
                b->display();
                found = true;
            }
        }
        else if (choice == 2)
        {
            cout << "Enter Title (partial match allowed): ";
            getline(cin, keyword);
            for (size_t i = 0; i < books.size(); i++)
            {
                if (books[i].getTitle().find(keyword) != string::npos)
                {
                    books[i].display();
                    found = true;
                }
            }
        }
        else if (choice == 3)
        {
            cout << "Enter Author (partial match allowed): ";
            getline(cin, keyword);
            for (size_t i = 0; i < books.size(); i++)
            {
                if (books[i].getAuthor().find(keyword) != string::npos)
                {
                    books[i].display();
                    found = true;
                }
            }
        }
        else if (choice == 4)
        {
            cout << "Enter Category: ";
            getline(cin, keyword);
            for (size_t i = 0; i < books.size(); i++)
            {
                if (books[i].getCategory().find(keyword) != string::npos)
                {
                    books[i].display();
                    found = true;
                }
            }
        }
        else
        {
            cout << "Invalid choice!\n";
            return;
        }

        if (!found)
        {
            cout << "No books found matching the criteria.\n";
        }
    }

    void updateBook()
    {
        int id;
        cout << "\n--- Update Book ---\n";
        cout << "Enter Book ID to update: ";
        cin >> id;

        Book *b = findBookByID(id);
        if (b == nullptr)
        {
            cout << "Book not found!\n";
            return;
        }

        b->updateBook();
        saveData();
        cout << "Book updated successfully!\n";
    }

    void deleteBook()
    {
        int id;
        cout << "\n--- Delete Book ---\n";
        cout << "Enter Book ID to delete: ";
        cin >> id;

        for (size_t i = 0; i < books.size(); i++)
        {
            if (books[i].getBookID() == id)
            {
                if (books[i].getAvailable() < books[i].getQuantity())
                {
                    cout << "Cannot delete book. Some copies are currently issued.\n";
                    return;
                }
                books.erase(books.begin() + i);
                saveData();
                cout << "Book deleted successfully!\n";
                return;
            }
        }

        cout << "Book not found!\n";
    }

    // ---------- Student management ----------

    void registerStudent()
    {
        Student student;
        student.addStudent();

        if (findStudentByID(student.getId()) != nullptr)
        {
            cout << "Student with ID " << student.getId() << " already exists!\n";
            return;
        }

        students.push_back(student);
        saveData();
        cout << "Student registered successfully!\n";
    }

    void viewStudents()
    {
        if (students.empty())
        {
            cout << "No students registered.\n";
            return;
        }
        cout << "\n--- All Students ---\n";
        for (size_t i = 0; i < students.size(); i++)
        {
            students[i].display();
        }
    }

    void searchStudent()
    {
        if (students.empty())
        {
            cout << "No students registered.\n";
            return;
        }

        int choice;
        cout << "\n--- Search Student ---\n";
        cout << "1. By ID\n2. By Name\n3. By Department\n";
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore();

        string keyword;
        int id;
        bool found = false;

        if (choice == 1)
        {
            cout << "Enter Student ID: ";
            cin >> id;
            Student *s = findStudentByID(id);
            if (s != nullptr)
            {
                s->display();
                found = true;
            }
        }
        else if (choice == 2)
        {
            cout << "Enter Name (partial match allowed): ";
            getline(cin, keyword);
            for (size_t i = 0; i < students.size(); i++)
            {
                if (students[i].getName().find(keyword) != string::npos)
                {
                    students[i].display();
                    found = true;
                }
            }
        }
        else if (choice == 3)
        {
            cout << "Enter Department: ";
            getline(cin, keyword);
            for (size_t i = 0; i < students.size(); i++)
            {
                if (students[i].getDepartment().find(keyword) != string::npos)
                {
                    students[i].display();
                    found = true;
                }
            }
        }
        else
        {
            cout << "Invalid choice!\n";
            return;
        }

        if (!found)
        {
            cout << "No students found matching the criteria.\n";
        }
    }

    void updateStudent()
    {
        int id;
        cout << "\n--- Update Student ---\n";
        cout << "Enter Student ID to update: ";
        cin >> id;

        Student *s = findStudentByID(id);
        if (s == nullptr)
        {
            cout << "Student not found!\n";
            return;
        }

        s->updateStudent();
        saveData();
        cout << "Student updated successfully!\n";
    }

    void deleteStudent()
    {
        int id;
        cout << "\n--- Delete Student ---\n";
        cout << "Enter Student ID to delete: ";
        cin >> id;

        for (size_t i = 0; i < students.size(); i++)
        {
            if (students[i].getId() == id)
            {
                if (!students[i].getBorrowedBooks().empty())
                {
                    cout << "Cannot delete student. They have borrowed books!\n";
                    return;
                }
                students.erase(students.begin() + i);
                saveData();
                cout << "Student deleted successfully!\n";
                return;
            }
        }

        cout << "Student not found!\n";
    }

    // ---------- Issue / Return books ----------

    void issueBook()
    {
        if (books.empty() || students.empty())
        {
            cout << "Books or students not available.\n";
            return;
        }

        Issue issue;
        issue.issueBook();

        Student *student = findStudentByID(issue.getStudentID());
        if (student == nullptr)
        {
            cout << "Student not found!\n";
            return;
        }

        Book *book = findBookByID(issue.getBookID());
        if (book == nullptr)
        {
            cout << "Book not found!\n";
            return;
        }

        if (book->getAvailable() <= 0)
        {
            cout << "No copies available!\n";
            return;
        }

        book->setAvailable(book->getAvailable() - 1);
        student->addBorrowedBook(issue.getBookID());

        issues.push_back(issue);
        saveData();
        cout << "Book issued successfully!\n";
        cout << "Due Date: " << issue.getDueDate() << endl;
    }

    void returnBook()
    {
        if (issues.empty())
        {
            cout << "No issued books.\n";
            return;
        }

        int studentID, bookID;
        cout << "\n--- Return Book ---\n";
        cout << "Student ID: ";
        cin >> studentID;
        cout << "Book ID: ";
        cin >> bookID;

        Issue *issue = findActiveIssue(studentID, bookID);
        if (issue == nullptr)
        {
            cout << "No active issue record found!\n";
            return;
        }

        issue->returnBook();

        Book *book = findBookByID(bookID);
        if (book != nullptr)
        {
            book->setAvailable(book->getAvailable() + 1);
        }

        Student *student = findStudentByID(studentID);
        if (student != nullptr)
        {
            student->removeBorrowedBook(bookID);
        }

        saveData();
    }

    void viewIssuedBooks()
    {
        if (issues.empty())
        {
            cout << "No issue records found.\n";
            return;
        }
        cout << "\n--- All Issue Records ---\n";
        for (size_t i = 0; i < issues.size(); i++)
        {
            issues[i].display();
        }
    }

    void viewOverdueBooks()
    {
        string currentDate = getCurrentDate();
        bool found = false;

        cout << "\n--- Overdue Books ---\n";
        for (size_t i = 0; i < issues.size(); i++)
        {
            if (!issues[i].getIsReturned())
            {
                int days = daysBetween(issues[i].getDueDate(), currentDate);
                if (days > 0)
                {
                    cout << "Student ID: " << issues[i].getStudentID()
                         << " | Book ID: " << issues[i].getBookID()
                         << " | Due Date: " << issues[i].getDueDate()
                         << " | Overdue: " << days << " days\n";
                    found = true;
                }
            }
        }

        if (!found)
        {
            cout << "No overdue books found.\n";
        }
    }

    void reports()
    {
        int choice;
        cout << "\n--- Reports ---\n";
        cout << "1. Total Books\n2. Total Students\n";
        cout << "3. Issued Books\n4. Available Books\n";
        cout << "5. Overdue Books\n6. Total Fine Collected\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1)
        {
            cout << "Total Books: " << books.size() << endl;
        }
        else if (choice == 2)
        {
            cout << "Total Students: " << students.size() << endl;
        }
        else if (choice == 3)
        {
            int issuedCount = 0;
            for (size_t i = 0; i < issues.size(); i++)
            {
                if (!issues[i].getIsReturned())
                    issuedCount++;
            }
            cout << "Issued Books: " << issuedCount << endl;
        }
        else if (choice == 4)
        {
            int availableCount = 0;
            for (size_t i = 0; i < books.size(); i++)
            {
                availableCount += books[i].getAvailable();
            }
            cout << "Available Books: " << availableCount << endl;
        }
        else if (choice == 5)
        {
            viewOverdueBooks();
        }
        else if (choice == 6)
        {
            int totalFine = 0;
            for (size_t i = 0; i < issues.size(); i++)
            {
                totalFine += issues[i].getFine();
            }
            cout << "Total Fine Collected: Rs. " << totalFine << endl;
        }
        else
        {
            cout << "Invalid choice!\n";
        }
    }

    // ---------- Student-side functions ----------

    void viewAvailableBooks()
    {
        bool found = false;
        cout << "\n--- Available Books ---\n";
        for (size_t i = 0; i < books.size(); i++)
        {
            if (books[i].getAvailable() > 0)
            {
                books[i].display();
                found = true;
            }
        }
        if (!found)
        {
            cout << "No books available.\n";
        }
    }

    void viewStudentBorrowedBooks(int studentID)
    {
        Student *student = findStudentByID(studentID);
        if (student == nullptr)
        {
            cout << "Student not found!\n";
            return;
        }

        vector<int> borrowed = student->getBorrowedBooks();
        if (borrowed.empty())
        {
            cout << "No books borrowed.\n";
            return;
        }

        cout << "\n--- Books Borrowed by " << student->getName() << " ---\n";
        for (size_t i = 0; i < borrowed.size(); i++)
        {
            Book *book = findBookByID(borrowed[i]);
            if (book != nullptr)
            {
                book->display();
            }
        }
    }

    bool studentLogin(int &studentID)
    {
        cout << "\n--- Student Login ---\n";
        cout << "Enter Student ID: ";
        cin >> studentID;

        Student *student = findStudentByID(studentID);
        if (student != nullptr)
        {
            cout << "Welcome, " << student->getName() << "!\n";
            return true;
        }

        cout << "Student not found!\n";
        return false;
    }

    void studentMenu(int studentID)
    {
        int choice;
        do
        {
            cout << "\n========== Student Menu ==========\n";
            cout << "1. Search Book\n";
            cout << "2. View Available Books\n";
            cout << "3. My Borrowed Books\n";
            cout << "4. Logout\n";
            cout << "Enter choice: ";
            cin >> choice;

            if (choice == 1)
            {
                searchBook();
            }
            else if (choice == 2)
            {
                viewAvailableBooks();
            }
            else if (choice == 3)
            {
                viewStudentBorrowedBooks(studentID);
            }
            else if (choice == 4)
            {
                cout << "Logging out...\n";
            }
            else
            {
                cout << "Invalid choice!\n";
            }
        } while (choice != 4);
    }

    // ---------- Admin menu ----------

    void adminMenu()
    {
        int choice;
        do
        {
            cout << "\n========== Admin Menu ==========\n";
            cout << "1. Add Book\n";
            cout << "2. View Books\n";
            cout << "3. Search Book\n";
            cout << "4. Update Book\n";
            cout << "5. Delete Book\n";
            cout << "6. Register Student\n";
            cout << "7. View Students\n";
            cout << "8. Search Student\n";
            cout << "9. Update Student\n";
            cout << "10. Delete Student\n";
            cout << "11. Issue Book\n";
            cout << "12. Return Book\n";
            cout << "13. View Issued Books\n";
            cout << "14. Overdue Books\n";
            cout << "15. Reports\n";
            cout << "16. Logout\n";
            cout << "Enter choice: ";
            cin >> choice;

            if (choice == 1)
                addBook();
            else if (choice == 2)
                viewBooks();
            else if (choice == 3)
                searchBook();
            else if (choice == 4)
                updateBook();
            else if (choice == 5)
                deleteBook();
            else if (choice == 6)
                registerStudent();
            else if (choice == 7)
                viewStudents();
            else if (choice == 8)
                searchStudent();
            else if (choice == 9)
                updateStudent();
            else if (choice == 10)
                deleteStudent();
            else if (choice == 11)
                issueBook();
            else if (choice == 12)
                returnBook();
            else if (choice == 13)
                viewIssuedBooks();
            else if (choice == 14)
                viewOverdueBooks();
            else if (choice == 15)
                reports();
            else if (choice == 16)
                cout << "Logging out...\n";
            else
                cout << "Invalid choice!\n";

        } while (choice != 16);
    }

    // ---------- Main menu loop ----------

    void run()
    {
        int choice, studentID;

        do
        {
            cout << "\n==================================================\n";
            cout << "    LIBRARY MANAGEMENT SYSTEM\n";
            cout << "==================================================\n";
            cout << "1. Admin Login\n";
            cout << "2. Student Login\n";
            cout << "3. Exit\n";
            cout << "==================================================\n";
            cout << "Enter choice: ";
            cin >> choice;

            if (choice == 1)
            {
                if (adminLogin())
                {
                    cout << "Admin logged in successfully!\n";
                    adminMenu();
                }
                else
                {
                    cout << "Invalid credentials!\n";
                }
            }
            else if (choice == 2)
            {
                if (studentLogin(studentID))
                {
                    studentMenu(studentID);
                }
            }
            else if (choice == 3)
            {
                cout << "Thank you for using Ammaz's Library Management System!\n";
                saveData();
            }
            else
            {
                cout << "Invalid choice!\n";
            }
        } while (choice != 3);
    }
};

// ================================================================
// Main Function
// ================================================================
int main()
{
    try
    {
        LibrarySystem library;
        library.run();
    }
    catch (const exception &e)
    {
        cout << "Error: " << e.what() << endl;
    }
    catch (...)
    {
        cout << "Unknown error occurred!\n";
    }

    return 0;
}
