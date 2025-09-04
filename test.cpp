#include <iostream>
#include <vector>
#include <string>
#include <bits/stdc++.h>
#include <chrono>
#include <algorithm> 
using namespace std;

vector<vector<string>> content;
vector<string> toupdate;

// Global constants for file names
const string BOOKS_FILE = "books.csv";
const string USERS_FILE = "users.csv";
const string BORROWED_BOOKS_FILE = "borrowed_books.csv";
const string BORROW_HISTORY_FILE = "borrow_history.csv";
const string FINES_FILE = "fine_entries.csv";

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

void readfile(string fname){ //reference : https://java2blog.com/read-csv-file-in-cpp/
    vector<string> row;
    string line, word;

    fstream file (fname,ios::in);
    if(file.is_open()){
        while(getline(file, line)){
            row.clear();
            stringstream str(line);
            while(getline(str, word, ',')) row.push_back(word);
            content.push_back(row);
        }
    }
    else cout<<"Could not open the file\n";
}
void writefile(vector<vector<string>> par, string fname){  
    fstream fout(fname,ios::out);
    for(auto x:par){
        for(auto y:x){
            fout<<y;
            if(y!=x.back()) fout<<",";
        }
        fout<<"\n";
    }

}
void writefileappend(vector<string> par, string fname){  
    fstream fout(fname,ios::out | ios::app);
    for(auto x:par){
        fout<<x;
        if(x!=par.back()) fout<<",";
    }
    fout<<"\n";
}

// Classes 

class Account;
class Book;
class User; 

enum class Status {Available, Borrowed, Reserved}; 
enum class Role {Student, Faculty, Librarian}; 

class Account {
    int useridie; 
    int role; // Role: 0 = Student, 1 = Faculty, 2 = Librarian
public:
    Account(int id, Role r) : useridie(id), role(static_cast<int>(r)) {}

    int getBorrowedCount() const {
        int count =0;
        content.clear();
        readfile("borrowed_books.csv");
        for(int i=0; i<content.size(); i++) {
             if(content[i][0] == to_string(useridie)) {
                count++;
             } 
        }
        content.clear();
        return count;
         
    } 
    
    //displays currently borrowed books list by a user
    void displayCBB() {
        content.clear();
        readfile(BORROWED_BOOKS_FILE);
        
        for (int i = 0; i < content.size(); i++) {
           
            if (content[i][0] == to_string(useridie)) {
                cout << "Book ID: " << content[i][2] << " | Book Title: " << content[i][1] << endl;
            }
        }
    
        content.clear();
    }
    
    //displays overdue books
    void displayODB() {
        content.clear();
        readfile(BORROWED_BOOKS_FILE);

        for(int i=0; i<content.size(); i++) {
            if(content[i][0] == to_string(useridie)) {
                string bookID = content[i][2];
                if (getOverduePeriod(stoi(bookID)) > 0) { 
                    cout << "Book ID: " << content[i][3] << " | Book Title: " << content[i][4] << " | Overdue Period: " << getOverduePeriod(stoi(bookID)) << " days" << endl; 
                }
            }
        }
        content.clear();
    }

    int getOverduePeriod(int bookID) {
        int overdue = 0;
        int days = 15; 
        content.clear();
        readfile(BORROWED_BOOKS_FILE);

        for(int i=0; i<content.size(); i++) {
            if(content[i][0] == to_string(useridie) && content[i][2] == to_string(bookID)) {
                time_t borrowStamp = stoi(content[i][3]);
                time_t currStamp   = time(0);
                if(role == 1) days = 30;
                if ((currStamp - borrowStamp) / 60 > days) {
                    overdue = (currStamp - borrowStamp) / 60 - days; 
                }
                break; 
            }
        }
        content.clear();
        return overdue;
    } 
};

class Book {
private:
    int bookID;
    string title;
    string author;
    string publisher;
    int year;
    string ISBN;
    Status bookStatus;
    
public:
    Book(int id, string t, string a, string p, int y, string isbn)
        : bookID(id), title(t), author(a), publisher(p), year(y), ISBN(isbn), bookStatus(Status::Available) {}

    // Getters
    int getBookID() const { return bookID; }
    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    string getPublisher() const { return publisher; }
    int getYear() const { return year; }
    string getISBN() const { return ISBN; }
    Status getBookStatus() const { return bookStatus; }

    // Setters
    void setBookID(int id) { bookID = id; }
    void setTitle(const string& t) { title = t; }
    void setAuthor(const string& a) { author = a; }
    void setPublisher(const string& p) { publisher = p; }
    void setYear(int y) { year = y; }
    void setISBN(const string& isbn) { ISBN = isbn; }
    void setBookStatus(Status status) { bookStatus = status; }
};

class User {
protected:
    int userID;
    string name; 
    Role userRole; 
    Account userAccount; 
    
public:

    User(int id, string uname, Role role) 
     : userID(id), name(uname), userRole(role), userAccount(id, role) {}
    
    //Getters
    int getUserID() const { return userID; }
    string getName() const { return name; } 
    Role getUserRole() const { return userRole; }
    Account getUserAccount() const { return userAccount; }

    // Setters
    void setUserID(int id) { userID = id; }
    void setName(const string& n) { name = n; }
    void setUserRole(Role role) { userRole = role; }
    void setUserAccount(const Account& acc) { userAccount = acc; }

    // Non-pure virtual functions 
    virtual void borrowBook(int bookID) {
        cout << name << " is borrowing book with ID " << bookID << endl;
    }
    virtual void returnBook(int bookID) {
        cout << name << " is returning book with ID " << bookID << endl;
    }
    virtual void displayBorrowHistory() const {
        cout << name << "'s borrow history:" << endl;
    }

    virtual void displayMenu(); 

    void displayBookStatus(int bookID) {
        content.clear();
        readfile(BOOKS_FILE);  // Read book records
    
        bool found = false;
        for (int i = 0; i < content.size(); i++) {
            if (stoi(content[i][0]) == bookID) { 
                found = true;
                cout << "---------------------------------------------\n";
                cout << "Book ID: " << content[i][0] << endl;
                cout << "Title: " << content[i][1] << endl;
                cout << "Author: " << content[i][2] << endl;
                cout << "Publisher: " << content[i][3] << endl;
                cout << "Year: " << content[i][4] << endl;
                cout << "ISBN: " << content[i][5] << endl;
                
                // Determine book status
                string status;
                if (content[i][6] == "0") status = "Available";
                else if (content[i][6] == "1") status = "Borrowed";
                else if (content[i][6] == "2") status = "Reserved";
                else status = "Unknown";
    
                cout << "Status: " << status << endl;
    
                // If reserved, display the user who reserved it
                if (content[i][6] == "2") {
                    cout << "Reserved by User ID: " << content[i][7] << endl;
                }
    
                cout << "---------------------------------------------\n";
                break;
            }
        }
    
        if (!found) {
            cout << "Book with ID " << bookID << " not found.\n";
        }
    }

    void viewAllBooks() {
        cout << "Book ID | Title | Author | Publisher | Year | ISBN | Status\n";
        cout << "-------------------------------------------------------------------\n";
        content.clear();
        readfile(BOOKS_FILE);
    
        for (auto row : content) {
            if (row.size() < 7) continue;
            cout << row[0] << " | " << row[1] << " | " << row[2] << " | "
                 << row[3] << " | " << row[4] << " | " << row[5] << " | "
                 << row[6] << endl;
        }

        content.clear();
    }
    

    
    // Virtual destructor (Ensures correct destructor calls) 
    virtual ~User() {}
};

class Student : public User {
    protected:
    int borrowLimit;       
    int borrowPeriod;      
    int fineOverdue;       
        
    public:
    Student(int id, string n) : User(id, n, Role::Student), borrowLimit(3), borrowPeriod(15), fineOverdue(10) {};
        
    // Getters
    int getBorrowLimit() const { return borrowLimit; }
    int getBorrowPeriod() const { return borrowPeriod; }
    int getFineOverdue() const { return fineOverdue; }

    // Setters
    void setBorrowLimit(int limit) { borrowLimit = limit; }
    void setBorrowPeriod(int period) { borrowPeriod = period; }
    void setFineOverdue(int fine) { fineOverdue = fine; }
        
    void borrowBook(int bookID);
    void returnBook(int bookID, bool reserve);
    void displayBorrowHistory() const;
        
    void displayMenu();
    int calculateFine();
    int calculateAndClearFine();
    void clearFine(int fine);
};

class Faculty : public User {
    protected:
    int borrowLimit;       
    int borrowPeriod;      
                        
    public:
    Faculty(int id, string n) : User(id, n, Role::Faculty), borrowLimit(5), borrowPeriod(30) {}; 

    // Getters
    int getBorrowLimit() const { return borrowLimit; }
    int getBorrowPeriod() const { return borrowPeriod; }

    // Setters
    void setBorrowLimit(int limit) { borrowLimit = limit; }
    void setBorrowPeriod(int period) { borrowPeriod = period; }
            
    void borrowBook(int bookID);
    void returnBook(int bookID, bool reserve);
    void displayBorrowHistory() const;
            
    void displayMenu();
};

class Librarian : public User {
    public:
    Librarian(int id, string n) : User(id, n, Role::Librarian) {}; 

    void displayMenu();
        
    void addBook();
    void removeBook(string bookID);
    void updateBook(string bookID);
                
    void addUser();
    void removeUser(string userID);

    void viewAllUsers() {
        content.clear();
        readfile(USERS_FILE);  
    
        cout << "User ID | Name | Role\n";
        cout << "-------------------------\n";
    
        for (auto row : content) {
            if (row.size() < 3) continue; // Skip malformed entries
            cout << row[0] << " | " << row[1] << " | " << row[2] << endl;
        }
        content.clear();
    }

    void viewBorrowedBooks() {
        content.clear();
        readfile(BORROWED_BOOKS_FILE);
    
        cout << "User ID | Book ID | Book Title | Borrowed On\n";
        cout << "---------------------------------------------------\n";
    
        for (auto row : content) {
            if (row.size() < 4) continue;
            cout << row[0] << " | " << row[2] << " | " << row[1] << " | " << row[3] << endl;
        }
        content.clear();
    }
    
    void resetFiles() {
        // Data for users.csv
        const string usersData = 
            "1,Asritha,0\n"
            "2,Nithin,1\n"
            "3,Varsha,2\n"
            "4,Rupesh,0\n"
            "5,Sreya,1\n"
            "6,Sowmya,0\n"
            "7,Adithya,0\n"
            "8,Varsha,0\n"
            "9,Pradeepthi,1\n";
        
        // Writing to users.csv
        ofstream usersFile("users.csv", ios::trunc);
        if (usersFile.is_open()) {
            usersFile << usersData;
            usersFile.close();
        } else {
            cerr << "Error opening users.csv" << endl;
        }
    
        // Data for books.csv
        const string booksData = 
            "1,The Great Gatsby,F. Scott Fitzgerald,Scribner,1925,97807,0\n"
            "2,To Kill a Mockingbird,Harper Lee,J. B. Lippincott & Co.,1960,97800,0\n"
            "3,1984,George Orwell,Secker & Warburg,1949,97804,0\n"
            "4,Heaven Official's Blessing,MXTX,Seven Seas,2021,97801,0\n"
            "5,SAYE,Wu Zhe,JiLiang Literature,2018,97803,0\n"
            "6,The Hobbit,J. R. R. Tolkien,George Allen & Unwin,1937,97805,0\n"
            "7,The Alchemist,Paulo Coelho,HarperOne,1988,97800,0\n"
            "8,The Lord of the Rings,J. R. R. Tolkien,George Allen & Unwin,1954,97805,0\n"
            "9,The Diary of a Young Girl,Anne Frank,Contact Publishing,1947,97805,0\n"
            "10,The Little Prince,Antoine de Saint-Exupéry,Reynal & Hitchcock,1943,97805,0\n";
        
        // Writing to books.csv
        ofstream booksFile("books.csv", ios::trunc);
        if (booksFile.is_open()) {
            booksFile << booksData;
            booksFile.close();
        } else {
            cerr << "Error opening books.csv" << endl;
        }
    
        // Clearing borrow_history.csv
        ofstream borrowHistoryFile("borrow_history.csv", ios::trunc);
        if (borrowHistoryFile.is_open()) {
            borrowHistoryFile.close();  // File will be truncated when opened in ios::trunc mode
        } else {
            cerr << "Error opening borrow_history.csv" << endl;
        }
    
        // Clearing borrowed_books.csv
        ofstream borrowedBooksFile("borrowed_books.csv", ios::trunc);
        if (borrowedBooksFile.is_open()) {
            borrowedBooksFile.close();  // File will be truncated when opened in ios::trunc mode
        } else {
            cerr << "Error opening borrowed_books.csv" << endl;
        }
    
        // Clearing fine_entries.csv
        ofstream fineEntriesFile("fine_entries.csv", ios::trunc);
        if (fineEntriesFile.is_open()) {
            fineEntriesFile.close();  // File will be truncated when opened in ios::trunc mode
        } else {
            cerr << "Error opening fine_entries.csv" << endl;
        }
    }
};    

int Student::calculateFine() {
    int fine = 0;  
    
    content.clear();  
    readfile("borrowed_books.csv");  
        
    for(int i = 0; i < content.size(); i++){
        if(content[i][0] == to_string(userID)){  
            int curtime = time(0);  
            int isstime = stoi(content[i][3]);  
            
            // Simulating time in minutes instead of days
            if((curtime - isstime) / 60 > 15) {
                fine += 10 * ((curtime - isstime) / 86400 - 15);
            }
        }
    }
    return fine; 

}

void Student::borrowBook(int bookID) {
    content.clear();
    int flag = 0; 

    // Check if the student has already borrowed 3 books
    if (userAccount.getBorrowedCount() >= 3) { 
        cout << "Borrow Limit is Reached!" << endl;
        return; 
    }

    if (calculateFine() > 0) {
        cout << "User has unpaid fines! Borrowing is not allowed!" << endl;
    }
    
    readfile(BOOKS_FILE);
    for (int i = 0; i < content.size(); i++) {
        // Check if the book exists and is either available OR reserved by this student
        if ((content[i][0] == to_string(bookID) && content[i][6] == "0" ) || (content[i][0] == to_string(bookID) && content[i][6] == "2" && content[i][7] == to_string(userID))) {  
            flag = 1; 
            
            // If book was reserved by the student, clear the reservation field before borrowing
            if(content[i][6] == "2") content[i][7].clear();

            // Update book status to "borrowed" (status = "1")
            content[i][6] = "1"; 
        
            // Add an entry to the borrowed books 
            toupdate.clear();
            toupdate.push_back(to_string(userID));
            toupdate.push_back(content[i][1]);  //book Title
            toupdate.push_back(to_string(bookID));  //book ID 
            toupdate.push_back(to_string(time(0))); //current timestamp
            writefileappend(toupdate, BORROWED_BOOKS_FILE);

            cout << "Book successfully borrowed!" << endl;
            break; 
        }
    }

    // If book was not found or unavailable, notify the user
    if (flag == 0) cout << "Book not available!" << endl;

    // Write updated book status back to books.csv
    writefile(content, BOOKS_FILE); 
    content.clear(); 
}

void Student::returnBook(int bookID, bool reserve) {
    content.clear();
    int flag = 0; 

    readfile(BORROWED_BOOKS_FILE);
    for (int i = 0; i < content.size(); i++) {
        // Check if the book is borrowed and is borrowed by the user
        if (content[i][2] == to_string(bookID) && content[i][0] == to_string(userID)) {  
            flag = 1; 
            
            // Add an entry to borrow_history.csv
            toupdate.clear();
            toupdate.push_back(to_string(userID));
            toupdate.push_back(name);
            toupdate.push_back("Student"); 
            toupdate.push_back(to_string(bookID));
            toupdate.push_back(content[i][1]);  //book Title
            writefileappend(toupdate, BORROW_HISTORY_FILE);

            // Remove from borrowed_books.csv
            content.erase(content.begin()+i);
            writefile(content, BORROWED_BOOKS_FILE);
            
            cout << "Book successfully returned!" << endl;
            cout << "This book was overdue by " << userAccount.getOverduePeriod(bookID) << " days." << endl;

            break; 
        }
    }

    if (flag == 0) cout << "Book not found in borrowed records" << endl;

    // Update the book's status in books.csv
    readfile(BOOKS_FILE);
    for (int i = 0; i < content.size(); i++) {
        if (content[i][0] == to_string(bookID)) {  
            if (reserve) {
                content[i][6] = "2";   // Set status to "Reserved"
                content[i].push_back(to_string(userID));  
            } else {
                content[i][6] = "0";   // Set status to "Available"
            }
            break;  
        }
    }

    writefile(content, BOOKS_FILE); 
    content.clear();
    
}

void Student::displayBorrowHistory() const {
    cout << "Student Borrow History:\n";
    content.clear();

    readfile(BORROW_HISTORY_FILE);
    for(int i=0; i<content.size(); i++) {
        if(content[i][0] == to_string(userID)) {
            cout << "Book ID: " << content[i][3] << " | Book Title: " << content[i][4] << endl; 
        }
    }
    content.clear();
} 
    
int Student::calculateAndClearFine() {
    int fine = 0;  
    
    content.clear();  
    readfile("borrowed_books.csv");  
        
    for(int i = 0; i < content.size(); i++){
        if(content[i][0] == to_string(userID)){  
            int curtime = time(0);  
            int isstime = stoi(content[i][3]);  
            
            // Simulating time in minutes instead of days
            if((curtime - isstime) / 60 > 15) {
                fine += 10 * ((curtime - isstime) / 86400 - 15);
            }
        }
    }
    cout << "Your fine is : " << fine << "\n";  
    
    if(fine > 0) {
        cout << "Do you want to pay the fine? (yes/no): ";
        string response; 
        cin >> response; 

        if (response == "yes") {
            clearFine(fine);
            cout << "Fine has been paid successfully.\n";
            fine = 0; 
        } else {
            content.clear();
            readfile(FINES_FILE);
            
            bool found = false;
            for (int i = 0; i < content.size(); i++) {
                if (content[i][0] == to_string(userID) && content[i][2] == "0") {  
                    content[i][1] = to_string(fine);  
                    found = true;
                    break;
                }
            }

            if (!found) {
                vector<string> new_entry = {to_string(userID), to_string(fine), "0"}; 
                content.push_back(new_entry);
            }

            writefile(content, FINES_FILE); 

            cout << "Fine is still due.\n";
        }
    }
    return fine; 
}

void Student::clearFine(int fine) {
    content.clear();
    readfile(FINES_FILE);  

    bool found = false;
    for (int i = 0; i < content.size(); i++) {
        if (content[i][0] == to_string(userID) && content[i][2] == "0") {  
            content[i][2] = "1";  
            found = true;
            break;
        }
    }

    if (!found) {
        vector<string> new_entry = {to_string(userID), to_string(fine), "1"};  // New entry with fine set to 0
        content.push_back(new_entry);
    }

    writefile(content, FINES_FILE);  // Update file
    content.clear();
}


void Faculty::borrowBook(int bookID) {
    content.clear();

    // Check if the Faculty has already borrowed 5 books
    if (userAccount.getBorrowedCount() >= borrowLimit) { 
        cout << "Borrow Limit is Reached!" << endl;
        return; 
    }

    // Check if faculty has an overdue book (over 60 days)
    content.clear();
    readfile(BORROWED_BOOKS_FILE);
    for (int i = 0; i < content.size(); i++) {
        if (content[i][0] == to_string(userID)) { 
            if (userAccount.getOverduePeriod(stoi(content[i][2])) > 60) {
                cout << "Overdue Period for a book is over 60 days! Borrowing is not possible" << endl;
                return;
            }
        }
    }

    // Check if the book is available
    content.clear();
    readfile(BOOKS_FILE);
    for (int i = 0; i < content.size(); i++) {
        if (content[i][0] == to_string(bookID)) { 
            if (content[i][6] == "0" || (content[i][6] == "2" && content[i][7] == to_string(userID))) {
                
                // If the book was reserved by the faculty, clear the reservation field
                if (content[i][6] == "2") content[i][7] = "0";  

                // Update book status to "borrowed" (status = "1")
                content[i][6] = "1"; 
        
                // Add an entry to the borrowed books list
                toupdate.clear();
                toupdate.push_back(to_string(userID));
                toupdate.push_back(content[i][1]);  
                toupdate.push_back(to_string(bookID));   
                toupdate.push_back(to_string(time(0))); 
                writefileappend(toupdate, BORROWED_BOOKS_FILE);

                writefile(content, BOOKS_FILE); 

                cout << "Book successfully borrowed!" << endl;
                return;  
            }
        }
    }

    cout << "Book not available!" << endl;
}


void Faculty::returnBook(int bookID, bool reserve) {
    content.clear();
    int flag = 0; 

    readfile(BORROWED_BOOKS_FILE);
    for (int i = 0; i < content.size(); i++) {
        // Check if the book is borrowed and is borrowed by the user
        if (content[i][2] == to_string(bookID) && content[i][0] == to_string(userID)) {  
            flag = 1; 
            
            // Add an entry to borrow_history.csv
            toupdate.clear();
            toupdate.push_back(to_string(userID));
            toupdate.push_back(name);
            toupdate.push_back("Student"); 
            toupdate.push_back(to_string(bookID));
            toupdate.push_back(content[i][1]);  //book Title
            writefileappend(toupdate, BORROW_HISTORY_FILE);

            // Remove from borrowed_books.csv
            content.erase(content.begin()+i,content.begin() + i + 1);
            writefile(content,BORROWED_BOOKS_FILE);
            
            cout << "Book successfully returned!" << endl;
            cout << userAccount.getOverduePeriod(bookID) << endl;

            break; 
        }
    }

    if (flag == 0) cout << "Book not found in borrowed records" << endl;

    // Update the book's status in books.csv
    readfile(BOOKS_FILE);
    for (int i = 0; i < content.size(); i++) {
        if (content[i][0] == to_string(bookID)) {  
            if (reserve) {
                content[i][6] = "2";   // Set status to "Reserved"
                content[i][7] = to_string(userID);  
            } else {
                content[i][6] = "0";   // Set status to "Available"
            }
            break;  
        }
    }

    writefile(content, BOOKS_FILE); 
    content.clear();
    
}

void Faculty::displayBorrowHistory() const {
    cout << "Faculty Borrow History:\n";
    content.clear();

    readfile(BORROW_HISTORY_FILE);
    for(int i=0; i<content.size(); i++) {
        if(content[i][0] == to_string(userID)) {
            cout << "Book ID: " << content[i][3] << " | Book Title: " << content[i][4] << endl; 
        }
    }
    content.clear();
} 
    
void Librarian::addBook() {
    string newAuthor, newTitle, newPublisher, newISBN;
    int newYear, newbookID;

    cout << "Enter Book ID (must be unique): ";  
    cin >> newbookID;
    cin.ignore(); 

    cout << "Enter title: ";
    getline(cin, newTitle);
    
    cout << "Enter author: ";
    getline(cin, newAuthor);
    
    cout << "Enter publisher: ";
    getline(cin, newPublisher);
    
    cout << "Enter ISBN: ";
    getline(cin, newISBN);
    
    cout << "Enter year: ";
    cin >> newYear;
    cin.ignore(); 

    // Open the file in append mode
    fstream fout("books.csv", ios::out | ios::app);
    if (!fout) {
        cerr << "Error: Could not open books.csv for writing!" << endl;
        return;
    }

    // Write book details to CSV
    fout << newbookID << "," << newTitle << "," << newAuthor << "," << newPublisher << "," << newYear << "," << newISBN << ",0\n";  // Status = 0 (Available)

    fout.close();  

    cout << "Book added successfully!" << endl;
}


void Librarian::removeBook(string bookID) {
    content.clear();
    readfile(BOOKS_FILE);

    bool found = false;
    for (int i = 0; i < content.size(); i++) {
        if (content[i][0] == bookID) {
            content.erase(content.begin() + i, content.begin()+i+1) ;
            found = true;
            break; 
        }
    }

    if (found) {
        writefile(content, BOOKS_FILE);
        cout << "Book with ID " << bookID << " has been removed successfully.\n";
    } else {
        cout << "Book with ID " << bookID << " not found.\n";
    }

    content.clear();
}

void Librarian::updateBook(string bookID) {
    string author, title, publisher, isbn, status;
    int year;
    bool found = false;

    cout << "Enter new author: ";
    cin.ignore();
    getline(cin, author); 
    cout << "Enter new title: ";
    getline(cin, title);
    cout << "Enter new publisher: ";
    getline(cin, publisher);
    cout << "Enter new ISBN: ";
    getline(cin, isbn);
    cout << "Enter new status (0: Available, 1: Borrowed, 2: Reserved): ";
    getline(cin, status);
    cout << "Enter new year: ";
    cin >> year; 
    
    content.clear();
    readfile(BOOKS_FILE);

    for (int i = 0; i < content.size(); i++) {
        if (content[i][0] == bookID) {
            content[i][1] = title;
            content[i][2] = author;
            content[i][3] = publisher;
            content[i][4] = to_string(year);
            content[i][5] = isbn;
            content[i][6] = status;
            found = true;
            break; 
        }
    }

    if (found) {
        writefile(content, BOOKS_FILE);
        cout << "Book updated successfully!" << endl;
    } else {
        cout << "Book with ID " << bookID << " not found!" << endl;
    }

    content.clear();
}


void Librarian::addUser() { 
    int newuserID;
    string newName; 
    int newuserRole; 
    
    cout << "Enter User ID (must be unique): " ; 
    cin >> newuserID; 
    cin.ignore(); // Clear newline from buffer
    
    cout << "Enter Name of the User: " ; 
    getline(cin, newName);
    
    cout << "Enter User Role (0: Student, 1: Faculty, 2: Librarian): " ; 
    cin >> newuserRole;
    
    fstream fout("users.csv", ios::out | ios::app);
    fout << newuserID << "," << newName << "," << newuserRole << '\n';

    fout.close();

    cout << "User added successfully!" << endl;
}


void Librarian::removeUser(string userID) {
    content.clear();
    readfile(USERS_FILE);


    bool found = false;
    for (int i = 0; i < content.size(); i++) {
        if (content[i][0] == userID) {
            content.erase(content.begin() + i, content.begin() + i + 1);
            found = true;
            break; 
        }
    }

    if (found) {
        writefile(content, USERS_FILE);
        cout << "User with ID " << userID << " has been removed successfully.\n";
    } else {
        cout << "User with ID " << userID << " not found.\n";
    }

    content.clear();
}


void User::displayMenu() {
    while (true) { // Infinite loop until the user chooses to exit
        char c;
        cout << "---------------------------------------------------------------------\n";
        cout << "\n\nWelcome to the Library Management System!\n\n";
        cout << "1. Press 1 to navigate to respective user menu \n";
        cout << "2. Press 2 to exit\n";
        cout << "---------------------------------------------------------------------\n";
        cin >> c;

        if (c == '1') { 
            int userid; 
            cout << "Enter your user ID: ";
            cin >> userid;

            content.clear();
            readfile(USERS_FILE); 

            string namE;
            string rolE;
            bool found = false;
            
            for (int i = 0; i < content.size(); i++) {
                if (content[i][0] == to_string(userid)) {  
                    rolE = content[i][2];

                    // Clean it
                    rolE.erase(remove_if(rolE.begin(), rolE.end(), [](unsigned char c) { 
                        return isspace(c) || c == '\0'; 
                    }), rolE.end());

                    namE = content[i][1];
                    found = true;
                    break;
                }
            }

            if (!found) {
                cout << "User ID not found. Please try again.\n";
                continue; // Restart menu loop
            }

            // Navigate to respective user menu
            if (rolE == "0") {
                Student student(userid, namE);
                student.displayMenu();
            } 
            else if (rolE == "1") {
                Faculty faculty(userid, namE);
                faculty.displayMenu();
            } 
            else if (rolE == "2") {
                Librarian librarian(userid, namE);
                librarian.displayMenu();
            } 
            else {
                cout << "Invalid role detected. Contact the administrator.\n";
            }
        } 

        else if (c == '2') {
            cout << "Thanks for using the system!\n";
            exit(0);
        } 

        else {
            cout << "Invalid choice! Please enter 1 or 2.\n";
        }
    }
}



void Student::displayMenu() {
    char c;
    
    while (true) {  
        cout << "---------------------------------------------------------------------\n";
        cout << "\nWelcome " << name << "! You are logged in as a Student.\n\n";
        cout << "Press 0 to see all the books\n";
        cout << "Press 1 to check if a book is available for borrowing\n";  
        cout << "Press 2 to borrow a book\n";  
        cout << "Press 3 to return a book\n";  
        cout << "Press 4 to view your borrow history\n"; 
        cout << "Press 5 to view your current borrowed books list\n"; 
        cout << "Press 6 to view your overdue books list\n"; 
        cout << "Press 7 to view the total outstanding fine and to clear it\n"; 
        cout << "Press 8 to logout\n";
        cout << "---------------------------------------------------------------------\n";
        
        cout << "Enter your choice: ";
        cin >> c;

        int bookID; 

        switch (c) {
            case '0':
                viewAllBooks();
                break;

            case '1':
                cout << "Enter the ID of the book to check its status: ";
                cin >> bookID;
                displayBookStatus(bookID); 
                break;

            case '2':
                cout << "Enter the ID of the book you wish to borrow: "; 
                cin >> bookID; 
                borrowBook(bookID); 
                break;

            case '3': {
                cout << "Enter the ID of the book you wish to return: ";
                cin >> bookID;

                char choice;
                cout << "Do you wish to reserve the book? (y/n): ";
                cin >> choice;

                bool response = (choice == 'y' || choice == 'Y');  

                returnBook(bookID, response);
                break;
            }

            case '4':
                displayBorrowHistory(); 
                break;

            case '5':
                userAccount.displayCBB(); 
                break;

            case '6':
                userAccount.displayODB(); 
                break;

            case '7':
                calculateAndClearFine();
                break;

            case '8':
                cout << "Logging out...\n";
                return;  // end the loop
            
            default:
                cout << "Invalid option! Please try again.\n";
                break;
        }

        cout << "\nPress any key to continue...\n";
        cin.ignore();  // Ignore any leftover input
        cin.get();     // Wait for user to press Enter
    }
}


void Faculty :: displayMenu(){
    char c;
    
    while (true) {  
        cout << "---------------------------------------------------------------------\n";
        cout << "\nWelcome " << name << "! You are logged in as a faculty.\n\n";
        cout << "Press 0 to check if a book is available for borrowing\n"; 
        cout << "Press 1 to borrow a book\n";  
        cout << "Press 2 to return a book\n";  
        cout << "Press 3 to view your borrow history\n"; 
        cout << "Press 4 to view your current borrowed books list\n"; 
        cout << "Press 5 to view your overdue books list\n";  
        cout << "Press 6 to see all the books\n";
        cout << "Press 7 to logout\n";
        cout << "---------------------------------------------------------------------\n";
        
        cout << "Enter your choice: ";
        cin >> c;

    
    int bookID; 

    switch (c) {
        case '0':
            cout << "Enter the ID of the book to check its status: ";
            cin >> bookID;
            displayBookStatus(bookID); 
            break;

        case '1':
            cout << "Enter the ID of the book you wish to borrow: "; 
            cin >> bookID; 
            borrowBook(bookID); 
            break;

        case '2': {
            cout << "Enter the ID of the book you wish to return: ";
            cin >> bookID;

            char choice;
            cout << "Do you wish to reserve the book? (y/n): ";
            cin >> choice;

            bool response = (choice == 'y' || choice == 'Y');  

            returnBook(bookID, response);
            break;
        }

        case '3':
            displayBorrowHistory(); 
            break;

        case '4':
            userAccount.displayCBB(); 
            break;

        case '5':
            userAccount.displayODB(); 
            break;

        case '6':
           viewAllBooks();
           break;

        case '7':
            cout << "Logging out...\n";
            return;  // end the loop
        
        default:
            cout << "Invalid option! Please try again.\n";
            break;
    }

    cout << "\nPress any key to continue...\n";
    cin.ignore();  
    cin.get();     // Wait for user to press Enter
}
}

void Librarian::displayMenu() {
    char choice;

    while (true) {
        cout << "---------------------------------------------------------------------\n";
        cout << "\nWelcome " << name << "! You are logged in as a librarian.\n\n";
        cout << "Press 0 to check a book's status\n";
        cout << "Press 1 to add a user\n";
        cout << "Press 2 to remove a user\n";
        cout << "Press 3 to add a book\n";
        cout << "Press 4 to update a book\n";
        cout << "Press 5 to remove a book\n";
        cout << "Press 6 to view all users\n";
        cout << "Press 7 to view all books\n";
        cout << "Press 8 to view all the borrowed books\n";
        cout << "Press 9 to reset the users and books data\n";
        cout << "Press l to logout\n";
        cout << "---------------------------------------------------------------------\n";
        cout << "Enter your choice: ";
        
        cin >> choice;
        cin.ignore(); 

        string userid, bookid;
        int bookID;

        switch (choice) {
            case '0':
                cout << "Enter the ID of the book to check its status: ";
                cin >> bookID;
                displayBookStatus(bookID); 
                break;
            case '1':
                addUser();
                break;
            case '2':
                cout << "Enter the ID of the user you want to remove: ";
                cin >> userid;
                removeUser(userid);
                break;  
            case '3':
                addBook();
                break;
            case '4':
                cout << "Enter the ID of the book you want to update: ";
                cin >> bookid;
                updateBook(bookid);
                break;
            case '5':
                cout << "Enter the ID of the book you want to remove: ";
                cin >> bookid;
                removeBook(bookid);
                break;
            case '6':
                viewAllUsers();
                break;
            case '7':
                viewAllBooks();
                break;
            case '8':
                viewBorrowedBooks();
                break;
            case '9':
                resetFiles();
                break;
            case 'l':
                cout << "Logging out...\n";
                return;  // Exit the function
            default:
            cout << "Invalid option! Please try again.\n";
            break;
        }

        cout << "\nPress any key to continue...\n";
        cin.ignore();  
        cin.get();     // Wait for user to press Enter
    }
}



int main() {
    User temp(-1,"whatever",Role::Student);  
    temp.displayMenu(); 
    return 0;
}
