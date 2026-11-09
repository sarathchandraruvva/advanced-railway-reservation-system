#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <map>
#include <ctime>
#include <cctype>
#include <cstdlib>
using namespace std;
const string PF = "passengers.txt", TF = "trains.txt", BF = "bookings.txt";
const string ADMIN_USER = "admin", ADMIN_PASS = "admin123";
string trim(string s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    return s.substr(a, s.find_last_not_of(" \t\r\n") - a + 1);
}
string ask(const string& prompt) {
    cout << prompt;
    string s;
    getline(cin, s);
    return trim(s);
}
int askInt(const string& prompt, int minimum = 0) {
    while (true) {
        cout << prompt;
        int n;
        if (cin >> n && n >= minimum) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return n;
        }
        if (cin.eof()) {
            cout << "\nNo input received. Please run this program in a terminal and enter a menu number.\n";
            exit(0);
        }
        cout << "Invalid number. Please enter a valid number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}
string lower(string s) {
    transform(s.begin(), s.end(), s.begin(),[](unsigned char c) {
        return static_cast<char>(tolower(c));
    }
    );
    return s;
}
string safe(string s) {
    replace(s.begin(), s.end(), '|', '/');
    return s;
}
vector<string> split(const string& line) {
    vector<string> fields;
    string part;
    stringstream ss(line);
    while (getline(ss, part, '|')) fields.push_back(part);
    return fields;
}
class User {
    protected: string name, username, password;
    public: User() = default;
    User(string n, string u, string p) : name(n), username(u), password(p) {
    }
    virtual ~User() = default;
    string getName() const {
        return name;
    }
    string getUsername() const {
        return username;
    }
    bool passwordMatches(const string& p) const {
        return password == p;
    }
    virtual void role() const = 0;
}
;
class Passenger : public User {
    int age = 0;
    public: Passenger() = default;
    Passenger(string n, string u, string p, int a) : User(n, u, p), age(a) {
    }
    int getAge() const {
        return age;
    }
    void role() const override {
        cout << "Passenger\n";
    }
}
;
class Train {
    public: int number = 0;
    string name, source, destination, departure, arrival;
    double distance = 0;
    int sleeper = 0, ac = 0, first = 0;
    int capacity(const string& c) const {
        if (c == "Sleeper") return sleeper;
        if (c == "AC") return ac;
        if (c == "First") return first;
        return 0;
    }
}
;
class Booking {
    public:
    string pnr, passenger, source, destination, date, travelClass, status;
    int trainNo = 0, seatNo = 0;
    double fare = 0;
}
;
class FareCalculator {
    public: virtual ~FareCalculator() = default;
    virtual double calculate(double km) const = 0;
}
;
class SleeperFare : public FareCalculator {
    public:
    double calculate(double km) const override {
        return km;
    }
}
;
class ACFare : public FareCalculator {
    public:
    double calculate(double km) const override {
        return km * 2;
    }
}
;
class FirstFare : public FareCalculator {
    public:
    double calculate(double km) const override {
        return km * 3;
    }
}
;
class Payment {
    public:
    virtual ~Payment() = default;
    virtual bool pay(double amount) const = 0;
}
;
class UPIPayment : public Payment {
    public: bool pay(double amount) const override {
        cout << fixed << setprecision(2) << "Simulated UPI payment: Rs. " << amount << "\n";
        return true;
    }
}
;
class CardPayment : public Payment {
    public: bool pay(double amount) const override {
        cout << fixed << setprecision(2) << "Simulated card payment: Rs. " << amount << "\n";
        return true;
    }
}
;
class RailwaySystem {
    vector<Passenger> passengers;
    vector<Train> trains;
    vector<Booking> bookings;
    map<string, string> passwords;
    Passenger* passengerByUser(const string& u) {
        for (auto& p : passengers) if (p.getUsername() == u) return &p;
        return nullptr;
    }
    Train* trainByNumber(int n) {
        for (auto& t : trains) if (t.number == n) return &t;
        return nullptr;
    }
    Booking* bookingByPNR(const string& pnr) {
        for (auto& b : bookings) if (b.pnr == pnr) return &b;
        return nullptr;
    }
    int confirmedCount(int n, const string& cls) const {
        int count = 0;
        for (const auto& b : bookings)
        if (b.trainNo == n && b.travelClass == cls && b.status == "CONFIRMED") ++count;
        return count;
    }
    string makePNR() const {
        static unsigned long counter = 1000;
        return "PNR" + to_string(time(nullptr)) + to_string(++counter);
    }
    void loadPassengers() {
        ifstream f(PF);
        string line;
        while (getline(f, line)) {
            auto v = split(line);
            if (v.size() != 4) continue;
            try {
                passengers.emplace_back(v[0], v[1], v[2], stoi(v[3]));
                passwords[v[1]] = v[2];
            }
            catch (...) {
            }
        }
    }
    void savePassengers() const {
        ofstream f(PF);
        for (const auto& p : passengers)
        f << safe(p.getName()) << '|' << safe(p.getUsername()) << '|'
        << safe(passwords.at(p.getUsername())) << '|' << p.getAge() << '\n';
    }
    void loadTrains() {
        ifstream f(TF);
        string line;
        while (getline(f, line)) {
            auto v = split(line);
            if (v.size() != 10) continue;
            try {
                Train t;
                t.number = stoi(v[0]);
                t.name = v[1];
                t.source = v[2];
                t.destination = v[3];
                t.distance = stod(v[4]);
                t.departure = v[5];
                t.arrival = v[6];
                t.sleeper = stoi(v[7]);
                t.ac = stoi(v[8]);
                t.first = stoi(v[9]);
                trains.push_back(t);
            }
            catch (...) {
            }
        }
    }
    void saveTrains() const {
        ofstream f(TF);
        for (const auto& t : trains)
        f << t.number << '|' << safe(t.name) << '|' << safe(t.source) << '|'
        << safe(t.destination) << '|' << t.distance << '|' << safe(t.departure)
        << '|' << safe(t.arrival) << '|' << t.sleeper << '|' << t.ac << '|' << t.first << '\n';
    }
    void loadBookings() {
        ifstream f(BF);
        string line;
        while (getline(f, line)) {
            auto v = split(line);
            if (v.size() != 10) continue;
            try {
                Booking b;
                b.pnr = v[0];
                b.passenger = v[1];
                b.trainNo = stoi(v[2]);
                b.source = v[3];
                b.destination = v[4];
                b.date = v[5];
                b.travelClass = v[6];
                b.fare = stod(v[7]);
                b.status = v[8];
                b.seatNo = stoi(v[9]);
                bookings.push_back(b);
            }
            catch (...) {
            }
        }
    }
    void saveBookings() const {
        ofstream f(BF);
        for (const auto& b : bookings)
        f << safe(b.pnr) << '|' << safe(b.passenger) << '|' << b.trainNo << '|'
        << safe(b.source) << '|' << safe(b.destination) << '|' << safe(b.date)
        << '|' << safe(b.travelClass) << '|' << b.fare << '|' << b.status << '|' << b.seatNo << '\n';
    }
    Booking* firstWaiting(int n, const string& cls) {
        for (auto& b : bookings)
        if (b.trainNo == n && b.travelClass == cls && b.status == "WAITING") return &b;
        return nullptr;
    }
    void promoteWaiting(int n, const string& cls) {
        Train* t = trainByNumber(n);
        if (!t || confirmedCount(n, cls) >= t->capacity(cls)) return;
        Booking* b = firstWaiting(n, cls);
        if (!b) return;
        b->status = "CONFIRMED";
        b->seatNo = confirmedCount(n, cls);
        cout << "Waiting-list PNR " << b->pnr << " is now confirmed.\n";
    }
    void showTrain(const Train& t) const {
        cout << "\nTrain: " << t.number << " - " << t.name << '\n'
        << t.source << " to " << t.destination << " | " << t.distance << " km\n"
        << "Departure: " << t.departure << " | Arrival: " << t.arrival << '\n'
        << "Capacity Sleeper/AC/First: " << t.sleeper << '/' << t.ac << '/' << t.first << '\n'
        << "Confirmed Sleeper/AC/First: " << confirmedCount(t.number, "Sleeper") << '/'
        << confirmedCount(t.number, "AC") << '/' << confirmedCount(t.number, "First") << '\n';
    }
    void showBooking(const Booking& b) const {
        cout << "\n--- TICKET DETAILS ---\nPNR: " << b.pnr
        << "\nTrain: " << b.trainNo << "\nRoute: " << b.source << " -> " << b.destination
        << "\nDate: " << b.date << "\nClass: " << b.travelClass << "\nFare: Rs. " << fixed << setprecision(2) << b.fare
        << "\nStatus: " << b.status << '\n';
        if (b.seatNo > 0) cout << "Seat: " << b.seatNo << '\n';
    }
    void searchTrains() const {
        string from = ask("Source station: "), to = ask("Destination station: ");
        bool found = false;
        for (const auto& t : trains)
        if (lower(t.source) == lower(from) && lower(t.destination) == lower(to)) {
            showTrain(t);
            found = true;
        }
        if (!found) cout << "No direct train found.\n";
    }
    void bookTicket(const string& user) {
        string from = ask("Source station: "), to = ask("Destination station: ");
        vector<int> matches;
        for (const auto& t : trains) if (lower(t.source) == lower(from) && lower(t.destination) == lower(to))
        matches.push_back(t.number);
        if (matches.empty()) {
            cout << "No matching train.\n";
            return;
        }
        for (int n : matches) showTrain(*trainByNumber(n));
        int n = askInt("Choose train number: ", 1);
        Train* t = trainByNumber(n);
        if (!t || find(matches.begin(), matches.end(), n) == matches.end()) {
            cout << "Invalid train choice.\n";
            return;
        }
        cout << "1. Sleeper  2. AC  3. First\n";
        int choice = askInt("Choose class: ", 1);
        string cls;
        if (choice == 1) cls = "Sleeper";
        else if (choice == 2) cls = "AC";
        else if (choice == 3) cls = "First";
        else {
            cout << "Invalid class.\n";
            return;
        }
        string date = ask("Journey date (YYYY-MM-DD): ");
        if (date.empty()) date = "Not specified";
        double fare = 0;
        if (cls == "Sleeper") {
            SleeperFare f;
            fare = f.calculate(t->distance);
        }
        else if (cls == "AC") {
            ACFare f;
            fare = f.calculate(t->distance);
        }
        else {
            FirstFare f;
            fare = f.calculate(t->distance);
        }
        Passenger* p = passengerByUser(user);
        if (p && p->getAge() >= 60) fare *= 0.75;
        cout << fixed << setprecision(2) << "Fare: Rs. " << fare << '\n';
        Booking b;
        b.pnr = makePNR();
        b.passenger = user;
        b.trainNo = t->number;
        b.source = t->source;
        b.destination = t->destination;
        b.date = date;
        b.travelClass = cls;
        b.fare = fare;
        if (confirmedCount(t->number, cls) < t->capacity(cls)) {
            cout << "Payment: 1. UPI  2. Card\n";
            int payChoice = askInt("Choose payment: ", 1);
            bool success = false;
            if (payChoice == 1) {
                UPIPayment pay;
                success = pay.pay(fare);
            }
            else if (payChoice == 2) {
                CardPayment pay;
                success = pay.pay(fare);
            }
            else {
                cout << "Invalid payment choice.\n";
                return;
            }
            if (!success) {
                cout << "Payment failed.\n";
                return;
            }
            b.status = "CONFIRMED";
            b.seatNo = confirmedCount(t->number, cls) + 1;
        }
        else {
            int join = askInt("No seats. Join waiting list? (1 yes, 0 no): ", 0);
            if (join != 1) {
                cout << "Booking not created.\n";
                return;
            }
            b.status = "WAITING";
            b.seatNo = 0;
        }
        bookings.push_back(b);
        saveBookings();
        showBooking(b);
    }
    void myBookings(const string& user) const {
        bool found = false;
        for (const auto& b : bookings) if (b.passenger == user) {
            showBooking(b);
            found = true;
        }
        if (!found) cout << "No bookings found.\n";
    }
    void checkPNR(const string& user) const {
        string pnr = ask("Enter PNR: ");
        for (const auto& b : bookings)
        if (b.pnr == pnr && b.passenger == user) {
            showBooking(b);
            return;
        }
        cout << "PNR not found in your account.\n";
    }
    void cancelTicket(const string& user) {
        Booking* b = bookingByPNR(ask("PNR to cancel: "));
        if (!b || b->passenger != user) {
            cout << "Booking not found in your account.\n";
            return;
        }
        if (b->status == "CANCELLED") {
            cout << "Already cancelled.\n";
            return;
        }
        int n = b->trainNo;
        string cls = b->travelClass;
        bool confirmed = b->status == "CONFIRMED";
        b->status = "CANCELLED";
        b->seatNo = 0;
        if (confirmed) promoteWaiting(n, cls);
        saveBookings();
        cout << "Ticket cancelled successfully.\n";
    }
    void passengerMenu(const string& user) {
        while (true) {
            cout << "\nPASSENGER MENU\n1 Search trains\n2 Book ticket\n3 My bookings\n4 Check PNR\n5 Cancel ticket\n0 Logout\n";
            int c = askInt("Choose: ");
            if (c == 0) return;
            if (c == 1) searchTrains();
            else if (c == 2) bookTicket(user);
            else if (c == 3) myBookings(user);
            else if (c == 4) checkPNR(user);
            else if (c == 5) cancelTicket(user);
            else cout << "Invalid choice.\n";
        }
    }
    void addTrain() {
        Train t;
        t.number = askInt("Train number: ", 1);
        if (trainByNumber(t.number)) {
            cout << "Train number already exists.\n";
            return;
        }
        t.name = ask("Train name: ");
        t.source = ask("Source: ");
        t.destination = ask("Destination: ");
        t.distance = askInt("Distance in km: ", 1);
        t.departure = ask("Departure time: ");
        t.arrival = ask("Arrival time: ");
        t.sleeper = askInt("Sleeper capacity: ");
        t.ac = askInt("AC capacity: ");
        t.first = askInt("First-class capacity: ");
        trains.push_back(t);
        saveTrains();
        cout << "Train added.\n";
    }
    void viewTrains() const {
        if (trains.empty()) cout << "No trains.\n";
        for (const auto& t : trains) showTrain(t);
    }
    void removeTrain() {
        int n = askInt("Train number to remove: ", 1);
        if (!trainByNumber(n)) {
            cout << "Train not found.\n";
            return;
        }
        for (const auto& b : bookings)
        if (b.trainNo == n && (b.status == "CONFIRMED" || b.status == "WAITING")) {
            cout << "Cannot remove train with active bookings.\n";
            return;
        }
        trains.erase(remove_if(trains.begin(), trains.end(),
        [n](const Train& t) {
            return t.number == n;
        }
        ), trains.end());
        saveTrains();
        cout << "Train removed.\n";
    }
    void allBookings() const {
        if (bookings.empty()) cout << "No bookings.\n";
        for (const auto& b : bookings) showBooking(b);
    }
    void adminMenu() {
        while (true) {
            cout << "\nADMIN MENU\n1 Add train\n2 View trains\n3 Remove train\n4 View bookings\n0 Logout\n";
            int c = askInt("Choose: ");
            if (c == 0) return;
            if (c == 1) addTrain();
            else if (c == 2) viewTrains();
            else if (c == 3) removeTrain();
            else if (c == 4) allBookings();
            else cout << "Invalid choice.\n";
        }
    }
    void addSampleTrains() {
        const Train samples[] = {
            {12864, "Howrah Chennai Express", "Howrah", "Chennai", "14:00", "10:00", 1660, 10, 8, 4},
            {12002, "Bhopal Shatabdi", "New Delhi", "Bhopal", "06:00", "14:00", 700, 10, 8, 4},
            {12009, "Shatabdi Express", "Mumbai Central", "Ahmedabad", "06:20", "12:45", 490, 10, 8, 4},
            {12627, "Karnataka Express", "Bengaluru", "New Delhi", "19:20", "10:30", 2400, 10, 8, 4},
            {12727, "Godavari Express", "Hyderabad", "Visakhapatnam", "17:30", "06:00", 700, 10, 8, 4},
            {12801, "Purushottam Express", "Puri", "New Delhi", "21:45", "04:00", 1850, 10, 8, 4},
            {12615, "Grand Trunk Express", "Chennai", "New Delhi", "18:40", "06:20", 2180, 10, 8, 4},
            {12951, "Mumbai Rajdhani", "Mumbai Central", "New Delhi", "16:35", "08:30", 1380, 10, 8, 4},
            {12839, "Chennai Mail", "Chennai", "Howrah", "23:45", "04:00", 1660, 10, 8, 4},
            {18477, "Kalinga Utkal Express", "Puri", "Haridwar", "21:55", "04:00", 1900, 10, 8, 4}
        };
        for (const auto& sample : samples) {
            if (!trainByNumber(sample.number)) trains.push_back(sample);
        }
        saveTrains();
    }
    public: RailwaySystem() {
        loadPassengers();
        loadTrains();
        loadBookings();
        addSampleTrains();
    }
    void registerPassenger() {
        string n = ask("Full name: "), u = ask("Choose username: ");
        if (u.empty() || passengerByUser(u)) {
            cout << "Username is empty or already taken.\n";
            return;
        }
        string p = ask("Choose password: ");
        int age = askInt("Age: ", 1);
        passengers.emplace_back(n, u, p, age);
        passwords[u] = p;
        savePassengers();
        cout << "Registration successful.\n";
    }
    void passengerLogin() {
        string u = ask("Username: "), p = ask("Password: ");
        Passenger* passenger = passengerByUser(u);
        if (!passenger || !passenger->passwordMatches(p)) {
            cout << "Invalid username or password.\n";
            return;
        }
        cout << "Welcome, " << passenger->getName() << "!\n";
        passengerMenu(u);
    }
    void adminLogin() {
        string u = ask("Admin username: "), p = ask("Admin password: ");
        if (u != ADMIN_USER || p != ADMIN_PASS) {
            cout << "Invalid admin credentials.\n";
            return;
        }
        cout << "Admin login successful.\n";
        adminMenu();
    }
    void run() {
        while (true) {
            cout << "\n=== ADVANCED RAILWAY RESERVATION SYSTEM ===\n"
            << "1 Passenger registration\n2 Passenger login\n3 Admin login\n0 Exit\n";
            int c = askInt("Choose: ");
            if (c == 0) {
                cout << "Goodbye.\n";
                return;
            }
            if (c == 1) registerPassenger();
            else if (c == 2) passengerLogin();
            else if (c == 3) adminLogin();
            else cout << "Invalid choice.\n";
        }
    }
}
;
int main() {
    RailwaySystem app;
    app.run();
    return 0;
}
