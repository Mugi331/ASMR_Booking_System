#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <limits>
#include <ctime>
#include <cctype>
#include <cstdlib> 
#include <cstdio>
#include <cmath>
#include <chrono>
#include <thread>
#include <windows.h> //used for logo diplay in ASCII code
#ifdef max
#undef max
#endif

using namespace std;

// ================= Constants =================
const int MAX_USERS = 100;
const int MAX_BOOKINGS = 250;
const int NUM_SERVICES = 3;
const int NUM_PACKAGES = 2;
const int TOTAL_OPTIONS = NUM_SERVICES + NUM_PACKAGES;
const int NUM_EXPERTS = 3;
const int BK_CAP = 500;
const int WEEKS_IN_DECEMBER = 4;
const int DAYS_PER_WEEK = 5; // Monday to Friday


// ================= Data Structures =================
struct Expert {
    int id;
    string username;
    string password;
    string name;
};

struct Service {
    int id;
    string name;
    string type;
    int durationMin;
    double price;
};

struct Appointment {
    string customerName;
    string serviceName;
    string expertName;
    string date;
    double startTime;
    double price;
    bool isPackage;
    int durationMin;
};

struct Booking {
    int bookingId;
    int expertId;
    int serviceId;
    string customer;
    string date;
    double startTime;
    int startHour;
    int durationMin;
    double price;
    bool hasConsultation;
    string serviceName; 
    string expertName;
};

struct Config {
    int dayLimitMin;
    int weekLimitMin;
    int openHour;
    int closeHour;
};

// ================== Function Prototypes ==================
string to_string_with_precision(double value, int precision);
bool isValidTimeFormat(double time);
bool isValidDate(const string& date);
bool isNumeric(const string& s);
bool isAlphaNumeric(const string& s);
double getValidTimeInput(const string& prompt, int openHour, int closeHour);
bool isDecember(const string& date);
bool isWeekend(const string& date);
string getValidDateInput();
int getValidNumericInput(const string& prompt, int minVal, int maxVal);
char getYesNoInput(const string& prompt);
int toInt(const string& s, int start, int len);
int parseDate(const string& date, int& y, int& m, int& d);
int dayOfWeek(int y, int m, int d);
string addDays(const string& date, int add);
string mondayOfWeek(const string& date);
bool isTimeOverlap(double start1, int duration1, double start2, int duration2);
bool isValidBookingTime(double startTime, int durationMin, int openHour, int closeHour);
int isDecemberWeekday2025(const string& date);
string getExpertAssignedService(int expertId, const string& date, Service services[], int svcN);
bool isPackage(const string& serviceName);
string getMainComponent(const string& packageName);
bool doesExpertSpecializeIn(int expertId, const string& service, const string& date);
int getExpertsForService(const string& service, const string& date, Expert experts[], int expertN, Service services[], int svcN, int availableExperts[], int maxExperts);

string getString(const string& prompt, bool allowEmpty);

void saveCustomers(const string usernames[], const string passwords[], int userCount);
int loadCustomer(string usernames[], string passwords[], int& userCount);
void saveBooking(const Booking bookings[], int bkCount);
int loadBooking(Booking bookings[], int& bkCount, Expert experts[], int expertN);
void saveExperts(const Expert experts[], int expertCount);
int loadExperts(Expert experts[], int& expertCount);
int customerMenu();
void viewBeautyCentreInfo();
int registerUser(string usernames[], string passwords[], int& userCount);
int loginUser(const string usernames[], const string passwords[], int userCount);
int checkAppointmentAvailability(const string& dateInput, Booking bookings[], int bkCount, const Expert experts[], int expertN, const Config& cfg, Service services[], int svcN);
bool bookAppointment(const string& customer, const string serviceNames[], const string packageNames[], const string expertNames[], const double servicePrices[][2], const double packagePrices[], 
    Booking bookings[], int& bkCount, Expert experts[], int expertN, Service services[], int svcN, const Config& cfg);
int viewMyBookings(Booking bookings[], int bkCount, const string& customer);
bool isValidPhone(const string& phone);
int findServiceIdxByName(Service services[], int svcN, const string& name);
int parseAppointmentService(const string& svcStr, string& baseService, bool& consultationRequested);
int buildDailyOccupancy(const Config& cfg, int expertId, const string& date,
    Booking bookings[], int bkCount, int occupancy[], int occLen);
int weeklyMinutes(const Config& cfg, int expertId, const string& monday,
    Booking bookings[], int bkCount);
int checkBookingFit(const Config& cfg, int expertId, const string& date, int startHour, int durationMin,
    Booking bookings[], int bkCount, int& outDayBookedMin, int& outWeekMin);
int serviceIndexForExpertOnDay(int expertIndex, int dayOfMonth);
int viewIndividualSchedule(const Config& cfg, int expertId, Expert experts[], int expertN,
    Service services[], int svcN, Booking bookings[], int bkCount, const string& anyDate);
int viewAssignedCustomerList(const Config& cfg, int expertId, Booking bookings[], int bkCount,
    Service services[], int svcN, const string& anyDate, Expert experts[]);
double viewEarningsBonus(const Config& cfg, int expertId, Booking bookings[], int bkCount,
    Service services[], int svcN, Expert experts[]);
int viewServicesAndExperts(const string serviceNames[], const string packageNames[],
    const double servicePrices[][2], const double packagePrices[],
    const Expert experts[], int expertN, Service services[], int svcN);
int expertLogin(Expert experts[], int expertN);
int expertMenu();
Booking* bubbleSortBookingsByDate(Booking bookings[], int count, Service services[], int svcN);
string getSortedBookingsByDateDisplay(Booking bookings[], int count, Service services[], int svcN);


bool adminLogin();
int adminMenu();

void viewCustomerList(Booking bookings[], int bkCount, Expert experts[], int expertN, Service services[], int svcN);
int adminViewExpertSchedule(const Config& cfg, int expertId, Expert experts[], int expertN, Service services[], int svcN, Booking bookings[], int bkCount);
double adminGenerateSalesReport(Booking bookings[], int bkCount);
double adminViewExpertBonus(Booking bookings[], int bkCount, Expert experts[], int expertN, int expertId);
double adminMonthlySummary(Booking bookings[], int bkCount);

void clearInputBuffer();
void displayLogo();
void goToPage();
void pauseForMenu();

// ---------------------------------------------------
//                    Functions
// ---------------------------------------------------

// ================ Input Validation =================
// - Checks if a string is numeric
bool isNumeric(const std::string& str) {
    if (str.empty()) 
        return false;
    for (char c : str) {
        if (!isdigit(c)) 
            return false;
    }
    return true;
}


string getString(const string& prompt, bool allowEmpty) {
    string value;
    bool valid = false;

    do {
        cout << prompt;
        getline(cin, value);

        // Check if the string is empty and not allowed
        if (value.empty() && !allowEmpty) {
            cout << "Invalid input! Please enter a value.\n";
            continue;
        }
        else {
            valid = true;
        }
    } while (!valid);

    return value;
}

int promptNumericInput(const string& prompt) {
    string input;
    while (true) {
        cout << prompt;
        getline(cin, input);
        if (!isNumeric(input)) {
            cout << "Invalid input! Please enter a numeric value.\n";
            continue;
        }
        return stoi(input);
    }
}

// - Checks if a string contains only alphabetic characters
bool isAlphabet(const std::string& str) {
    if (str.empty()) 
        return false;
    for (char c : str) {
        if (!isalpha(c)) 
            return false;
    }
    return true;
}

string promptAlphabetInput(const string& prompt) {
    string input;
    while (true) {
        cout << prompt;
        getline(cin, input);
        if (!isAlphabet(input)) {
            cout << "Invalid input! Must be alphabet.\n";
            continue;
        }
        return input;
    }
}

bool isAlphaNumeric(const string& str) {
    for (char c : str) {
        if (!isalnum(c)) return false;
    }
    return true;
}

int getValidNumericInput(const string& prompt, int minVal, int maxVal) {
    int choice;
    while (true) {
        cout << prompt;
        if (!(cin >> choice)) {
            clearInputBuffer();
            cout << "Invalid input! Please enter a numeric value between " << minVal << " and " << maxVal << ".\n";
            continue;
        }

        if (choice < minVal || choice > maxVal) {
            cout << "Invalid choice! Please enter a number between " << minVal << " and " << maxVal << ".\n";
            continue;
        }

        break;
    }
    return choice;
}

char getYesNoInput(const string& prompt) {
    string input;
    char choice;
    while (true) {
        cout << prompt;
        cin >> input;

        if (input.length() == 1) {
            choice = tolower(input[0]);
            if (choice == 'y' || choice == 'n') {
                break;
            }
        }

        cout << "Invalid input! Please enter only Y or N.\n";
        clearInputBuffer();
    }
    return choice;
}

bool isValidTimeFormat(double time) {
    // Check if it's a whole number (no fractional part)
    return (time == static_cast<int>(time));
}

bool isValidDate(const string& date) {
    if (date.size() != 10 || date[4] != '-' || date[7] != '-')
        return false;

    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) 
            continue;
        if (!isdigit(date[i])) 
            return false;
    }

    return true;
}

double getValidTimeInput(const string& prompt, int openHour, int closeHour) {
    double time;
    while (true) {
        cout << prompt;
        if (!(cin >> time)) {
            clearInputBuffer();
            cout << "Invalid input! Please enter a valid time (e.g., 14.00 for 2PM).\n";
            continue;
        }

        if (!isValidTimeFormat(time)) {
            cout << "Invalid time format! Please enter time in .00 or .30 format (e.g., 14.00 or 14.30).\n";
            continue;
        }

        if (time < openHour || time >= closeHour) {
            cout << "Invalid time! Our operating hours are " << openHour << ":00 - " << closeHour << ":00.\n";
            continue;
        }

        break;
    }
    return time;
}

bool isDecember(const string& date);
bool isWeekend(const string& date);
string getValidDateInput() {
    string date;
    while (true) {
        cout << "Enter appointment date (YYYY-MM-DD): ";
        cin >> date;

        if (date.size() != 10 || date[4] != '-' || date[7] != '-') {
            cout << "Invalid date format! Use YYYY-MM-DD.\n";
            continue;
        }

        for (int i = 0; i < 10; i++) {
            if (i == 4 || i == 7) continue;
            if (!isdigit(date[i])) {
                cout << "Invalid date format! Use YYYY-MM-DD.\n";
                continue;
            }
        }

        if (!isDecember(date)) {
            cout << "Invalid date! Only December is allowed.\n";
            continue;
        }
        if (date < "2025-12-01" || date > "2025-12-31") {
            cout << "Invalid date! Only dates between 2025-12-01 and 2025-12-31 are allowed.\n";
            continue;
        }
        if (isWeekend(date)) {
            cout << "Sorry, we are closed on weekends. Please choose another date.\n";
            continue;
        }
        break;
    }
    return date;
}

// Helper to check if a date is within the week starting from weekStartDate (Monday)
bool isWithinWeek(const string& date, const string& weekStartDate) {
    int y1, m1, d1, y2, m2, d2;
    if (!parseDate(date, y1, m1, d1) || !parseDate(weekStartDate, y2, m2, d2))
        return false;
    string monday = mondayOfWeek(date);
    return monday == weekStartDate;
}

// Overload for viewIndividualSchedule to match the call in adminViewOverallSchedule
void viewIndividualScheduleApp(const Config& cfg, Booking bookings[], int bkCount, const Expert& expert, const string& weekStartDate) {
    int occLen = cfg.closeHour - cfg.openHour;
    cout << left << setw(12) << "Date";
    for (int h = cfg.openHour; h < cfg.closeHour; ++h) {
        cout << setw(8) << (to_string(h) + ":00");
    }
    cout << setw(20) << "Assigned Service" << setw(15) << "Day Total" << "\n";
    cout << string(12 + occLen * 8 + 35, '-') << "\n";

    int weeklyBookedMin = 0;
    for (int d = 0; d < 5; ++d) {
        string day = addDays(weekStartDate, d);
        int yy, mm, dd;
        parseDate(day, yy, mm, dd);

        // Assigned service logic (rotating every 3 days)
        int expertIndex = expert.id;
        int svcIndex = serviceIndexForExpertOnDay(expertIndex, dd);
        string assignedService;
        switch (svcIndex) {
            case 0: assignedService = "Foot Massage (Reflexology)"; break;
            case 1: assignedService = "Full Body Massage"; break;
            case 2: assignedService = "Hot Stone Massage"; break;
            default: assignedService = "N/A";
        }
        string displayName = assignedService;
        size_t pos = displayName.find(" (");
        if (pos != string::npos) displayName = displayName.substr(0, pos);

        int occ[24] = { 0 };
        int dayBookedMin = 0;
        for (int b = 0; b < bkCount; ++b) {
            if (bookings[b].expertId == expert.id && bookings[b].date == day) {
                int st = static_cast<int>(bookings[b].startTime);
                int durH = (bookings[b].durationMin + 59) / 60;
                for (int h = 0; h < durH; ++h) {
                    int idx = st + h - cfg.openHour;
                    if (idx >= 0 && idx < occLen) occ[idx] = 1;
                }
                dayBookedMin += bookings[b].durationMin;
            }
        }

        cout << left << setw(12) << day;
        for (int i = 0; i < occLen; ++i) {
            string label = occ[i] ? "Booked" : "Free  ";
            cout << setw(8) << label;
        }
        cout << setw(20) << displayName << fixed << setprecision(2)
            << (dayBookedMin / 60.0) << "h / " << (cfg.dayLimitMin / 60.0) << "h\n";
        cout << string(12 + occLen * 8 + 35, '-') << "\n";
        weeklyBookedMin += dayBookedMin;
    }
}

// Admin overall schedule function
int adminViewOverallSchedule(const Config& cfg, Booking bookings[], int bkCount, Expert experts[], int expertN, const string& weekStartDate) {
    if (bkCount == 0) {
        cout << "\nNo bookings found.\n";
        return 0;
    }

    cout << "\n";
    cout << u8"╔════════════════════════════════════════════════════════════╗\n";
    cout << u8"║               Overall Schedule (Weekly View)               ║\n";
    cout << u8"╚════════════════════════════════════════════════════════════╝\n\n";

    // 1. Print each expert's weekly timetable
    for (int e = 0; e < expertN; e++) {
        cout << "\n====================================================\n";
        cout << "Expert: " << experts[e].name << "\n";
        cout << "====================================================\n";
        viewIndividualScheduleApp(cfg, bookings, bkCount, experts[e], weekStartDate);
    }

    // 2. Summary table
    cout << "\n";
    cout << u8"╔════════════════════════════════════════════════════════════╗\n";
    cout << u8"║              Weekly Summary of Expert Workload             ║\n";
    cout << u8"╚════════════════════════════════════════════════════════════╝\n\n";

    cout << left << setw(15) << "Expert"
        << setw(10) << "Bookings"
        << setw(15) << "Service Hrs"
        << setw(18) << "Consultation Hrs"
        << setw(15) << "Total Hrs" << "\n";

    cout << string(73, '-') << "\n";

    for (int e = 0; e < expertN; e++) {
        int bookingCount = 0;
        double serviceHours = 0.0;
        double consultHours = 0.0;

        for (int b = 0; b < bkCount; b++) {
            if (bookings[b].expertId == experts[e].id &&
                isWithinWeek(bookings[b].date, weekStartDate)) {

                bookingCount++;
                serviceHours += (bookings[b].durationMin / 60.0);

                if (bookings[b].hasConsultation) {
                    consultHours += 1.0; // assuming consultation = 1h
                }
            }
        }

        double totalHours = serviceHours + consultHours;

        cout << left << setw(15) << experts[e].name
            << setw(10) << bookingCount
            << setw(15) << fixed << setprecision(1) << serviceHours
            << setw(18) << consultHours
            << setw(15) << totalHours << "\n";
    }

    cout << string(73, '=') << "\n";

    return 1;
}

// ================ Helper Functions =================
string to_string_with_precision(double value, int precision) {
    ostringstream out;
    out << fixed << setprecision(precision) << value;
    return out.str();
}

int toInt(const string& s, int start, int len) {
    int v = 0;
    for (int i = 0; i < len; ++i) v = v * 10 + (s[start + i] - '0');
    return v;
}

int parseDate(const string& date, int& y, int& m, int& d) {
    if (date.size() != 10 || date[4] != '-' || date[7] != '-') return 0;
    y = toInt(date, 0, 4);
    m = toInt(date, 5, 2);
    d = toInt(date, 8, 2);
    return 1;
}

int dayOfWeek(int y, int m, int d) {
    static int t[] = { 0,3,2,5,0,3,5,1,4,6,2,4 };
    if (m < 3) y -= 1;
    return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
}

string addDays(const string& date, int add) {
    int y, m, d; if (!parseDate(date, y, m, d)) return date;
    auto dim = [&](int yy, int mm) {
        int md[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
        int v = md[mm - 1];
        if (mm == 2) { bool lp = ((yy % 4 == 0 && yy % 100 != 0) || yy % 400 == 0); if (lp) v = 29; }
        return v;
        };
    d += add;
    while (d > dim(y, m)) { d -= dim(y, m); m++; if (m > 12) { m = 1; y++; } }
    while (d <= 0) { m--; if (m < 1) { m = 12; y--; } d += dim(y, m); }

    // Replace sprintf_s with stringstream
    stringstream ss;
    ss << setw(4) << setfill('0') << y << "-"
        << setw(2) << setfill('0') << m << "-"
        << setw(2) << setfill('0') << d;
    return ss.str();
}

string mondayOfWeek(const string& date) {
    int y, m, d; if (!parseDate(date, y, m, d)) return date;
    int w = dayOfWeek(y, m, d); int delta = (w == 0 ? -6 : 1 - w);
    return addDays(date, delta);
}

bool isTimeOverlap(double start1, int duration1, double start2, int duration2) {
    double end1 = start1 + (duration1 / 60.0);
    double end2 = start2 + (duration2 / 60.0);

    // Check if time intervals overlap
    return (start1 < end2) && (end1 > start2);
}

bool isValidBookingTime(double startTime, int durationMin, int openHour, int closeHour) {
    double endTime = startTime + (durationMin / 60.0);
    return (startTime >= openHour && endTime <= closeHour);
}

int isDecemberWeekday2025(const string& date) {
    int y, m, d; if (!parseDate(date, y, m, d)) return 0;
    if (y != 2025 || m != 12) return 0;
    int w = dayOfWeek(y, m, d);
    return (w >= 1 && w <= 5) ? 1 : 0;
}

string getExpertAssignedService(int expertId, const string& date, Service services[], int svcN) {
    int y, m, d;
    parseDate(date, y, m, d);
    int svcIndex = serviceIndexForExpertOnDay(expertId, d);
    return services[svcIndex % svcN].name;
}

bool isPackage(const string& serviceName) {
    return (serviceName.find("Package") != string::npos);
}

string getMainComponent(const string& packageName) {
    if (packageName.find("Body + Hot") != string::npos)
        return "Full Body Massage";
    if (packageName.find("Foot + Hot") != string::npos)
        return "Foot Massage (Reflexology)";
    return "";
}

bool doesExpertSpecializeIn(int expertId, const string& service,
    const string& date) {
    // Each expert's daily specialties (primary and secondary)
    const char* expertSpecializations[3][2] = {
        {"Foot Massage (Reflexology)", "Full Body Massage"}, // June
        {"Full Body Massage", "Hot Stone Massage"},         // Bryan
        {"Hot Stone Massage", "Foot Massage (Reflexology)"} // Amy
    };

    int day = stoi(date.substr(8, 2)); // Extract day from YYYY-MM-DD
    int primaryIndex = day % 2;

    return (service == expertSpecializations[expertId][primaryIndex] ||
        service == expertSpecializations[expertId][(primaryIndex + 1) % 2]);
}

void clearInputBuffer() {
    cin.clear();
    cin.ignore(5000, '\n');
}

// ===== EXPERT SCHEDULING FUNCTIONS ===== //
int getExpertsForService(const string& service, const string& date, Expert experts[], int expertN, Service services[], int svcN, int availableExperts[], int maxExperts) {
    int count = 0;

    // Handle packages
    if (isPackage(service)) {
        string mainComponent = getMainComponent(service);
        if (!mainComponent.empty()) {
            for (int i = 0; i < expertN && count < maxExperts; i++) {
                if (doesExpertSpecializeIn(experts[i].id, mainComponent, date)) {
                    availableExperts[count++] = i;
                }
            }
        }
        return count;
    }

    // Handle regular services
    for (int i = 0; i < expertN && count < maxExperts; i++) {
        if (doesExpertSpecializeIn(experts[i].id, service, date)) {
            availableExperts[count++] = i;
        }
    }
    return count;
}

// ================== Display Functions ==================
void displayLogo() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    cout << u8R"(

        █████  ███████ ███    ███ ██████  
       ██   ██ ██      ████  ████ ██   ██ 
       ███████ ███████ ██ ████ ██ ██████  
       ██   ██      ██ ██  ██  ██ ██   ██ 
       ██   ██ ███████ ██      ██ ██   ██ 
                                           
  A  S E R E N E   M A S S A G E   R E T R E A T
  )" << "\n";

};

void goToPage() {
    system("cls");          
    displayLogo();
}

void pauseForMenu() {
    cout << "\nPress ENTER to return ...";
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    cin.get();
}

// =================== File Handling ===================
// Function to save customer data to file
void saveCustomers(const string usernames[], const string passwords[], int userCount) {
    ofstream outFile("customers.txt");

    if (!outFile) {
        cout << "ERROR : Unable to open customers.txt for writing.\n";
        return;
    }

    for (int i = 0; i < userCount; i++) {
        outFile << usernames[i] << " " << passwords[i] << "\n";

    }

    outFile.close();

}

// Function to load customer data from file
int loadCustomer(string usernames[], string passwords[], int& userCount) {
    ifstream inFile("customers.txt");

    if (!inFile) {
        cout << "ERROR : Unable to open customers.txt for reading.\n";
        userCount = 0;
        return 0;
    }
	userCount = 0;
    string u, p;

    while (inFile >> u >> p && userCount < MAX_USERS) {
        usernames[userCount] = u;
        passwords[userCount] = p;
        userCount++;
    }

    inFile.close();
    return userCount;

}

// Function to save booking data to file
void saveBooking(const Booking bookings[], int bkCount) {
    ofstream outFile("bookings.txt");
    if (!outFile) {
        cout << "ERROR : Unable to open bookings.txt for writing.\n";
        return;
    }

    for (int i = 0; i < bkCount; i++) {
        outFile << bookings[i].bookingId << "|"
            << bookings[i].expertId << "|"
            << bookings[i].serviceId << "|"
            << bookings[i].customer << "|"
            << bookings[i].date << "|"
            << static_cast<int>(bookings[i].startTime) << "|"
            << bookings[i].durationMin << "|"
            << fixed << setprecision(2) << bookings[i].price << "|"
            << (bookings[i].hasConsultation ? "1" : "0") << "\n";
    }

    outFile.close();
    cout << "Booking data saved successfully! (" << bkCount << " records)\n";
}

// Function to load booking data from file
int loadBooking(Booking bookings[], int& bkCount, Expert experts[], int expertN) {
    ifstream inFile("bookings.txt");
    if (!inFile) {
        cout << "ERROR : Unable to open bookings.txt for reading.\n";
		bkCount = 0;
        return 0;
    }

    bkCount = 0;
    string line;

    while (getline(inFile, line) && bkCount < BK_CAP) {
        if (line.empty()) 
            continue;

        stringstream ss(line);
        string token;

        // Parse the 9 fields from your file
        getline(ss, token, '|'); bookings[bkCount].bookingId = stoi(token);
        getline(ss, token, '|'); bookings[bkCount].expertId = stoi(token);
        getline(ss, token, '|'); bookings[bkCount].serviceId = stoi(token);
        getline(ss, bookings[bkCount].customer, '|');
        getline(ss, bookings[bkCount].date, '|');
        getline(ss, token, '|'); bookings[bkCount].startTime = stod(token);
        getline(ss, token, '|'); bookings[bkCount].durationMin = stoi(token);
        getline(ss, token, '|'); bookings[bkCount].price = stod(token);
        getline(ss, token, '|'); bookings[bkCount].hasConsultation = (token == "1");

		// Map serviceId to serviceName
        switch (bookings[bkCount].serviceId) {
        case 100: bookings[bkCount].serviceName = "Foot Massage (Reflexology)"; break;
        case 101: bookings[bkCount].serviceName = "Full Body Massage"; break;
        case 102: bookings[bkCount].serviceName = "Hot Stone Massage"; break;
        case 103: bookings[bkCount].serviceName = "Package A (Body + Hot Stone)"; break;
        case 104: bookings[bkCount].serviceName = "Package B (Foot + Hot Stone)"; break;
        default: bookings[bkCount].serviceName = "Unknown Service";
        }

		// Map expertId to expertName
        bookings[bkCount].expertName = "Unknown";
        for (int i = 0; i < expertN; i++) {
            if (experts[i].id == bookings[bkCount].expertId) {
                bookings[bkCount].expertName = experts[i].name;
                break;
            }
        }

        bkCount++;
    }

    inFile.close();
    return bkCount;
}

// Function to save expert data to file
void saveExperts(const Expert experts[], int expertCount) {
    ofstream outFile("experts.txt");

    if (!outFile) {
        cout << "ERROR : Unable to open experts.txt for writing.\n";
        return;
    }

    for (int i = 0; i < expertCount; i++) {
        outFile << experts[i].id << "|"
            << experts[i].username << "|"
            << experts[i].password << "|"
            << experts[i].name << "\n";
    }

    outFile.close();
}

// Function to load expert data from file
int loadExperts(Expert experts[], int& expertCount) {
    ifstream inFile("experts.txt");

    if (!inFile) {
        cout << "ERROR : Unable to open experts.txt for reading.\n";
        expertCount = 0;
        return 0; // File doesn't exist yet
    }

    expertCount = 0;
    string line;

    while (getline(inFile, line) && expertCount < NUM_EXPERTS) {
        if (line.empty())
            continue;

        stringstream ss(line);
        string token;

        getline(ss, token, '|');
        experts[expertCount].id = stoi(token);
        getline(ss, experts[expertCount].username, '|');
        getline(ss, experts[expertCount].password, '|');
        getline(ss, experts[expertCount].name, '|');

        expertCount++;
    }

    inFile.close();
    return expertCount;

}

// =================================================================================================
//                                  CUSTOMER FUNCTION
// =================================================================================================
int customerMenu() {
    goToPage();
    cout << u8"╔════════════════════════════════════════╗\n";
    cout << u8"║              Customer Menu             ║\n";
    cout << u8"╠════════════════════════════════════════╣\n";
    cout << u8"║ 1. View Beauty Centre Info             ║\n";
    cout << u8"║ 2. View Services & Experts             ║\n";
    cout << u8"║ 3. Check Appointment Availability      ║\n";
    cout << u8"║ 4. Book Appointment                    ║\n";
    cout << u8"║ 5. View My Bookings                    ║\n";
    cout << u8"║ 6. Logout                              ║\n";
    cout << u8"╚════════════════════════════════════════╝\n";

    int custMenuChoice = getValidNumericInput("\nEnter your choice: ", 1, 6);
    return custMenuChoice;
}

void viewBeautyCentreInfo() {
	system("cls");
    cout << u8"\n════════════════════════════════════════════════════════════════════════════\n";
	displayLogo();
    cout << "\n" << right << setw(40) << "Welcome to A.S.M.R !\n";
    cout << "Enjoy our specialised premium massage and wellness experiences :3\n";
    cout << left << setw(19) << "\nLocation" << " : 13, Jalan Genting Klang, 53300, Kuala Lumpur\n";
    cout << left << setw(18) << "Operating Hours" << " : Monday to Friday, 12 PM - 9 PM\n";
    cout << left << setw(18) << "Contact" << " : 012-5897846 | asmr.relax@gmail.com\n";
    cout << left << setw(18) << "Available Services" << " : Foot (Reflexology) | Full body | Hot Stone\n";
    cout << u8"\n════════════════════════════════════════════════════════════════════════════\n";


}

int registerUser(string usernames[], string passwords[], int& userCount) {
    if (userCount >= MAX_USERS) {
        cout << "User limit reached. Cannot register more users.\n";
        return -1;
    }

    string username, password;
    cout << "\n";
    cout << u8"╔═════════════════════════════════════════════╗\n";
    cout << u8"║            Customer Registration            ║\n";
    cout << u8"╚═════════════════════════════════════════════╝";
    cout << "\nEnter new username: ";
    cin >> username;

    for (int i = 0; i < userCount; i++) {
        if (usernames[i] == username) {
            cout << "Username already exists. Try again.\n";
            return -1;
        }
    }

    while (true) {
        cout << "Enter new password (6 digits): ";
        cin >> password;

        if (password.length() == 6) {
            bool allDigits = true;
            for (char c : password) {
                if (!isdigit(c)) {
                    allDigits = false;
                    break;
                }
            }

            if (allDigits)
                break;

        }
        cout << "\nInvalid password! Please enter exactly 6 digits.\n";
    }

    usernames[userCount] = username;
    passwords[userCount] = password;
    userCount++;

    saveCustomers(usernames, passwords, userCount);

    cout << "\nRegistration successful. You can now login.\n";
    return userCount - 1;
}

int loginUser(const string usernames[], const string passwords[], int userCount) {
    string username, password;
    cout << "\n";
    cout << u8"╔═════════════════════════════════════════════╗\n";
    cout << u8"║                Customer Login               ║\n";
    cout << u8"╚═════════════════════════════════════════════╝";

    cout << left << setw(27) << "\nUsername (No empty spaces)" << " : ";
    cin >> username;

    // Validate username format
    if (!isAlphaNumeric(username)) {
        cout << "Invalid username format! Only letters and numbers allowed.\n";
        return -1;
    }

    cout << left << setw(26) << "Password (6 digit)" << " : ";
    cin >> password;

    // Validate password format
    if (!isNumeric(password) || password.length() != 6) {
        cout << "Invalid password format! Must be exactly 6 digits.\n";
        return -1;
    }

    //validate user from file
    ifstream inFile("customers.txt");
    if (!inFile) {
        cout << "User not found. Please register first.\n";
        return -1;
    }

    string line;
    int index = 0;
    while (getline(inFile, line)) {
        size_t position = line.find(' ');
        if (position != string::npos) {
            string fileUsername = line.substr(0, position);
            string filePassword = line.substr(position + 1);

            if (fileUsername == username && filePassword == password) {
                cout << "Login successful. Welcome, " << username << "!" << endl;
                inFile.close();
                return index;
            }
        }

        index++;

    }

    inFile.close();
    cout << "Invalid username or password. " << endl;
    return -1;

}

int viewServicesAndExperts(const string serviceNames[], const string packageNames[],
    const double servicePrices[][2], const double packagePrices[],
    const Expert experts[], int expertN, Service services[], int svcN) {

	cout << "\n";   
    cout << u8"╔═══════════════════════════════════════════════════════╗\n";
    cout << u8"║" <<setw(35) << right << "AVAILABLE SERVICES" << setw(24) << right << u8"║\n";
    cout << u8"╚═══════════════════════════════════════════════════════╝\n";

    cout << left << setw(30) << "Service" << setw(12) << "1 Hour" << setw(12) << "2 Hours" << endl;
    cout << string(57, '-') << "\n";

    for (int i = 0; i < NUM_SERVICES; i++) {
        cout << left << setw(30) << serviceNames[i];
        if (servicePrices[i][0] > 0)
            cout << "RM" << setw(10) << fixed << setprecision(2) << servicePrices[i][0];
        else
            cout << setw(12) << "-";
        if (servicePrices[i][1] > 0)
            cout << "RM" << setw(10) << fixed << setprecision(2) << servicePrices[i][1];
        else
            cout << setw(12) << "-";
        cout << endl;
    }

    for (int i = 0; i < NUM_PACKAGES; i++) {
        cout << left << setw(30) << packageNames[i] << setw(12) << "-"
            << "RM" << setw(10) << fixed << setprecision(2) << packagePrices[i] << endl;
    }

    for (int i = 0; i < svcN; i++) {
        if (services[i].name == "Consultation") {
            cout << left << setw(30) << services[i].name
                << "RM" << setw(10) << fixed << setprecision(2) << services[i].price
                << setw(12) << "-" << endl;
        }
    }

	cout << "\n";
    cout << u8"╔══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
    cout << u8"║" << setw(55) << right << "OUR EXPERTS" << right << setw(67) << u8"║\n";
    cout << u8"╚══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝\n";

    cout << left << setw(10) << "Name" << setw(35) << "Specialist" << setw(25) << "Availability" << "Services" << endl;
    cout << string(120, '-') << "\n";

    for (int i = 0; i < expertN; i++) {
        string specialization1, specialization2;
        if (experts[i].id == 0) { specialization1 = "Foot Massage"; specialization2 = "Full Body Massage"; }
        else if (experts[i].id == 1) { specialization1 = "Full Body Massage"; specialization2 = "Hot Stone Massage"; }
        else if (experts[i].id == 2) { specialization1 = "Hot Stone Massage"; specialization2 = "Foot Massage (Reflexology)"; }

        cout << left << setw(10) << experts[i].name
            << setw(35) << specialization1 + " Specialist"
            << setw(25) << "Weekdays 12PM-9PM"
            << specialization1 << ", " << specialization2 << endl;
    }
    return 1;
}

int checkAppointmentAvailability(const string& dateInput, Booking bookings[], int bkCount,
    const Expert experts[], int expertN, const Config& cfg, Service services[], int svcN) {

    string date = dateInput;
    int y, m, d;
    bool validDate = false;

    while (!validDate) {
        if (!parseDate(date, y, m, d)) {
            cout << "Invalid date format. Enter again (YYYY-MM-DD): ";
            cin >> date;
            continue;
        }
        if (y != 2025) {
            cout << "\nOnly dates in the year 2025 are allowed. Enter again: ";
            cin >> date;
            continue;
        }
        if (!isDecember(date)) {
            cout << "\nThis feature is only available for December 2025. Enter again: ";
            cin >> date;
            continue;
        }
        if (!isDecemberWeekday2025(date)) {
            cout << "\nOur massage center is closed on weekends. Enter a weekday: ";
            cin >> date;
            continue;
        }
        validDate = true;
    }

    // Arrays for occupied slots and blocking
    bool occupied[10][24] = { false };
    bool blocked[10] = { false };

    // Fill booking data per expert
    for (int i = 0; i < expertN; i++) {
        int dailyMinutes = 0;
        for (int j = 0; j < bkCount; j++) {
            if (bookings[j].expertId == experts[i].id && bookings[j].date == date) {
                int start = static_cast<int>(bookings[j].startTime);
                int durMin = bookings[j].durationMin;
                dailyMinutes += durMin;
                int durHour = (durMin + 59) / 60;
                for (int h = 0; h < durHour; h++) {
                    int idx = start + h;
                    if (idx >= cfg.openHour && idx < cfg.closeHour) {
                        occupied[i][idx - cfg.openHour] = true;
                    }
                }
            }
        }
        if (dailyMinutes >= 360) {
            blocked[i] = true;
        }
    }

    cout << u8"\n╔═══════════════════════════════════════════════════════╗\n";
    cout << u8"║ Appointment Availability for " << date
        << setw(25 - date.size()) << " " << u8"║\n";
    cout << u8"╠═══════════════════════════════════════════════════════╣\n";

    cout << u8"║ " << setw(8) << left << "Time" << u8" │ ";

    for (int i = 0; i < expertN; i++) {
        cout << setw(12) << left << experts[i].name;
        if (i != expertN - 1)
            cout << u8" │ ";
        else
            cout << u8" ║";
    }
    cout << "\n";

    cout << u8"╠═══════════════════════════════════════════════════════╣\n";

    for (int h = cfg.openHour; h < cfg.closeHour; h++) {
        cout << u8"║ " << setw(8) << left << (to_string(h) + ":00") << u8" │ ";
        for (int i = 0; i < expertN; i++) {
            string status;
            if (blocked[i]) {
                status = "";
            }
            else {
                status = occupied[i][h - cfg.openHour] ? "[X] Booked" : "[ ] Free";
            }
            cout << setw(12) << left << status;
            if (i != expertN - 1)
                cout << u8" │ ";
            else
                cout << u8" ║";
        }
        cout << "\n";
    }

    cout << u8"╚═══════════════════════════════════════════════════════╝\n";

    return 1;
}

bool bookAppointment(const string& customer, const string serviceNames[], const string packageNames[],
    const string expertNames[], const double servicePrices[][2], const double packagePrices[],
    Booking bookings[], int& bkCount, Expert experts[], int expertN,
    Service services[], int svcN, const Config& cfg) {

    loadBooking(bookings, bkCount, experts, expertN);

    if (bkCount >= MAX_BOOKINGS) {
        cout << "Booking limit reached! Cannot add more appointments.\n";
        return false;
    }

    // --- Variables ---
    string inputDate;
    double bookingTime;
    int durationChoice = 0;
    string bookingService;
    double basePrice = 0.0;
    int durationMin = 0;
    bool isPackage = false;
    bool consultationAdded = false;

    // --- Get valid date ---
    while (true) {
        cout << "\nEnter appointment date (YYYY-MM-DD): ";
        cin >> inputDate;
        if (!isValidDate(inputDate) || !isDecember(inputDate) || isWeekend(inputDate)) {
            cout << "Invalid date! Only valid December 2025 weekdays allowed.\n";
            continue;

        }

        break;

    }

    goToPage();
    // --- Show services/packages menu ---
    cout << "\n============== A.S.M.R - Massage & Package Menu ==============\n\n";
    cout << u8"╔════╦════════════════════════════════╦══════════╦═══════════╗\n";
    cout << u8"║ No ║         Service/Package        ║  1 Hour  ║  2 Hours  ║\n";
    cout << u8"╠════╬════════════════════════════════╬══════════╬═══════════╣\n";

    for (int i = 0; i < NUM_SERVICES; i++) {
        cout << u8"║" << setw(2) << right << i + 1 << u8"  ║ "
            << left << setw(30) << serviceNames[i] << u8" ║ "
            << "RM" << right << setw(6) << fixed << setprecision(2) << servicePrices[i][0] << u8" ║ "
            << "RM" << right << setw(6) << fixed << setprecision(2) << servicePrices[i][1] << u8"  ║\n";
    }
    for (int i = 0; i < NUM_PACKAGES; i++) {
        cout << u8"║" << setw(2) << right << NUM_SERVICES + 1 + i << u8"  ║ "
            << left << setw(30) << packageNames[i] << u8" ║ "
            << setw(8) << right << "-" << u8" ║ "
            << "RM" << right << setw(6) << fixed << setprecision(2) << packagePrices[i] << u8"  ║\n";
    }
    cout << u8"╚════╩════════════════════════════════╩══════════╩═══════════╝\n";
    cout << "Note: Consultation (1 hour, RM50) can be added to any booking\n";

    // Get service choice
    int choice = getValidNumericInput("\nEnter the service you want (1-" + to_string(NUM_SERVICES + NUM_PACKAGES) + "): ", 1, TOTAL_OPTIONS);

    if (choice >= 1 && choice <= NUM_SERVICES) {
        bookingService = serviceNames[choice - 1];
        durationChoice = getValidNumericInput("\nEnter duration (1 for 1 hour, 2 for 2 hours): ", 1, 2);
        basePrice = servicePrices[choice - 1][durationChoice - 1];
        isPackage = false;
        durationMin = durationChoice * 60;
    }
    else {
        bookingService = packageNames[choice - NUM_SERVICES - 1];
        basePrice = packagePrices[choice - NUM_SERVICES - 1];
        isPackage = true;
        durationMin = 120;
    }

    // Consultation
    char consultationChoice = getYesNoInput("\nDo you want a consultation session (1 hour, RM50)? (Y/N): ");
    if (consultationChoice == 'y') {
        consultationAdded = true;
        basePrice += 50.0;
        durationMin += 60;
    }

    // Get time
    bookingTime = getValidTimeInput("\nEnter appointment time (e.g., 14.00 for 2PM): ", cfg.openHour, cfg.closeHour);

    // Expert selection
    bool expertAvailable = false;
    int chosenExpert = -1;

    while (!expertAvailable) {
        const int MAX_EXPERTS = 3;
        int availableExperts[MAX_EXPERTS];
        int availableCount = getExpertsForService(bookingService, inputDate, experts, expertN, services, svcN, availableExperts, MAX_EXPERTS);

        int trulyAvailableExperts[MAX_EXPERTS];
        int trulyAvailableCount = 0;

        for (int i = 0; i < availableCount; i++) {
            bool timeAvailable = true;
            int expertId = experts[availableExperts[i]].id;

            // Check bookings array for conflicts
            for (int j = 0; j < bkCount; j++) {
                if (bookings[j].expertId == expertId && bookings[j].date == inputDate) {
                    double existingStart = bookings[j].startTime;
                    double existingEnd = existingStart + bookings[j].durationMin / 60.0;
                    double newStart = bookingTime;
                    double newEnd = newStart + durationMin / 60.0;

                    if (!(newEnd <= existingStart || newStart >= existingEnd)) {
                        timeAvailable = false;
                        break;
                    }
                }
            }

            // Check 6-hour max daily limit
            if (timeAvailable) {
                int dailyTotal = 0;
                for (int j = 0; j < bkCount; j++) {
                    if (bookings[j].expertId == expertId && bookings[j].date == inputDate)
                        dailyTotal += bookings[j].durationMin;
                }
                if (dailyTotal + durationMin > 360) {
                    timeAvailable = false;
                }
            }

            if (timeAvailable) {
                trulyAvailableExperts[trulyAvailableCount++] = availableExperts[i];
            }
        }

        if (trulyAvailableCount == 0) {
            cout << "No experts available at this time. Please choose another time.\n";
            bookingTime = getValidTimeInput("\nEnter another time: ", cfg.openHour, cfg.closeHour);
            continue;
        }

        // --- Show available experts ---
        cout << "\nAvailable Experts:\n";
        for (int i = 0; i < trulyAvailableCount; i++)
            cout << i + 1 << ". " << experts[trulyAvailableExperts[i]].name << "\n";

        int expertChoice = getValidNumericInput("Select expert (1-" + to_string(trulyAvailableCount) + "): ", 1, trulyAvailableCount);
        chosenExpert = trulyAvailableExperts[expertChoice - 1];
        expertAvailable = true;
    }

    // ===== Payment handling =====
    double serviceCharge = basePrice * 0.16;
    double totalPrice = basePrice + serviceCharge;
    cout << fixed << setprecision(2);
    cout << "\n==== Payment Summary ====\n";
    cout << u8"┌──────────────────────────────┐\n";
    cout << u8"║   Payment Summary║\n";
    cout << u8"╚════════════════════════════════════════  ════╝\n";
    cout << left << setw(20) << "Service/Package:" << "RM " << right << setw(8) << basePrice << "\n";
    cout << left << setw(20) << "16% Service Charge:" << "RM " << right << setw(8) << serviceCharge << "\n";
    cout << left << setw(20) << "Total to pay:" << "RM " << right << setw(8) << totalPrice << "\n";

    int payMethod;
    bool paymentSuccess = false;
    do {
        payMethod = getValidNumericInput("\nSelect payment method:\n1. TNG (Touch N Go)\n2. Bank Transfer\n3. Cancel Payment\n\nEnter choice: ", 1, 3);
        if (payMethod == 1) {
            string tngPhone;
            do {
                cout << "\nEnter your phone number (e.g. 0123456789): ";
                cin >> tngPhone;
                if (!isValidPhone(tngPhone)) {
                    cout << "Invalid phone number! Please enter again.\n";
                }
            } while (!isValidPhone(tngPhone));

            string confirm;
            while (true) {
                cout << "\nConfirm payment of RM" << totalPrice << "? (Y/N): ";
                cin >> confirm;
                if (confirm.length() == 1) {
                    char c = tolower(confirm[0]);
                    if (c == 'y') {
                        cout << "\nPayment successful! Booking completed!\n";
                        paymentSuccess = true;
                        break;
                    }
                    else if (c == 'n') {
                        cout << "\nPayment cancelled. Booking failed.\n";
                        pauseForMenu();
                        return false;
                    }
                }
                cout << "Invalid input! Please enter Y or N.\n";
            }
        }
        else if (payMethod == 2) {
            string bankName, accountNo;
            cout << "\nEnter Bank Name: ";
            clearInputBuffer();
            getline(cin, bankName);

            while (true) {
                cout << "Enter Account Number (12 digit): ";
                getline(cin, accountNo);
                bool valid = (accountNo.length() == 12);
                for (char c : accountNo)
                    if (!isdigit(c))
                        valid = false;

                if (valid)
                    break;

                cout << "Invalid bank account number! Must be exactly 12 digits.\n";
            }

            string confirm;
            while (true) {
                cout << "\nConfirm bank transfer of RM" << totalPrice << "? (Y/N): ";
                cin >> confirm;
                if (confirm.length() == 1) {
                    char c = tolower(confirm[0]);
                    if (c == 'y') {
                        cout << "\nBank transfer received. Booking completed!\n\a";
                        paymentSuccess = true;
                        break;
                    }
                    else if (c == 'n') {
                        cout << "\nBank transfer cancelled. Booking failed.\n";
                        pauseForMenu();
                        return false;
                    }
                }
                cout << "Invalid input! Please enter only Y or N.\n";
            }
        }
        else if (payMethod == 3) {
            cout << "\nPayment cancelled. Booking failed.\n";
            return false;
        }

    } while (!paymentSuccess);

    int expertId = experts[chosenExpert].id;

    // Create booking record
    Booking newBooking;
    newBooking.bookingId = bkCount + 1;
    newBooking.expertId = expertId;
    newBooking.expertName = experts[chosenExpert].name;
    newBooking.customer = customer;
    newBooking.date = inputDate;
    newBooking.startTime = bookingTime;
    newBooking.durationMin = durationMin;
    newBooking.price = totalPrice;
    newBooking.hasConsultation = consultationAdded;

    // Assign service ID
    string baseService = bookingService;
    for (int i = 0; i < svcN; i++) {
        if (services[i].name == baseService) {
            newBooking.serviceId = services[i].id;
            newBooking.serviceName = services[i].name;
            break;
        }
    }

    // Add consultation to service name if needed
    if (consultationAdded) {
        newBooking.serviceName += " + Consultation";
    }

    bookings[bkCount++] = newBooking;
    saveBooking(bookings, bkCount);

    cout << "\nBooking confirmed for " << newBooking.expertName << " on "
        << newBooking.date << " at " << newBooking.startTime << " for "
        << newBooking.durationMin / 60 << " hours.\n";

    return true;
}

int viewMyBookings(Booking bookings[], int bkCount, const string& customer) {
    cout << "\n=== My Bookings ===\n";

    if (bkCount <= 0) {
        cout << "No bookings found.\n";
        return 0;
    }

    bool found = false;
    for (int i = 0; i < bkCount; i++) {
        if (bookings[i].customer == customer) {
            cout << "Booking #" << bookings[i].bookingId << ":\n"
                << "Service: " << bookings[i].serviceName << "\n"
                << "Expert: " << bookings[i].expertName << "\n"
                << "Date: " << bookings[i].date << "\n"
                << "Time: " << fixed << setprecision(2) << bookings[i].startTime << "\n"
                << "Price: RM" << bookings[i].price << "\n"
                << "----------------------------\n";
            found = true;
        }
    }

    if (!found) {
        cout << "No bookings found for customer: " << customer << "\n";
    }

    return 0;
}

bool isDecember(const string& date) {
    if (date.size() != 10 || date[4] != '-' || date[7] != '-')
        return false;
    int month = (date[5] - '0') * 10 + (date[6] - '0');
    return (month == 12);
}

bool isWeekend(const string& date) {
    // First validate date format
    if (date.size() != 10 || date[4] != '-' || date[7] != '-') {
        return false;
    }

    // Extract date components
    int year = stoi(date.substr(0, 4));
    int month = stoi(date.substr(5, 2));
    int day = stoi(date.substr(8, 2));

    // Validate date components
    if (month < 1 || month > 12 || day < 1 || day > 31) {
        return false;
    }

    // Initialize tm struct - must set ALL fields
    struct tm time_in = { 0 };  // Proper struct declaration and initialization
    time_in.tm_year = year - 1900;
    time_in.tm_mon = month - 1;
    time_in.tm_mday = day;
    time_in.tm_hour = 12;    // Set to noon to avoid DST issues
    time_in.tm_min = 0;      // Initialize minutes
    time_in.tm_sec = 0;      // Initialize seconds
    time_in.tm_isdst = -1;   // Let mktime determine daylight saving time

    // Normalize the time structure
    if (mktime(&time_in) == -1) {
        return false; // Invalid date
    }

    // Check if it's Saturday (6) or Sunday (0)
    return (time_in.tm_wday == 0 || time_in.tm_wday == 6);
}

bool isValidPhone(const string& phone) {
    if (phone.length() != 10 && phone.length() != 11)
        return false;
    if (phone[0] != '0' || phone[1] != '1')
        return false;
    for (char c : phone) {
        if (!isdigit(c))
            return false;
    }
    return true;
}

// =================================================================================================
//                                  EXPERT FUNCTION
// =================================================================================================
int findServiceIdxByName(const Service services[], int svcN, const string& name) {
    for (int i = 0; i < svcN; ++i)
        if (services[i].name == name)
            return i;

    return -1;
}

int parseAppointmentService(const string& svcStr, string& baseService, bool& consultationRequested) {
    baseService = svcStr;
    consultationRequested = false;
    size_t pos = svcStr.find("+ Consultation");
    if (pos != string::npos) {
        baseService = svcStr.substr(0, pos);
        while (!baseService.empty() && baseService.back() == ' ') baseService.pop_back();
        consultationRequested = true;
        return 1;
    }
    pos = svcStr.find("Consultation");
    if (pos != string::npos) {
        baseService = svcStr.substr(0, pos);
        while (!baseService.empty() && (baseService.back() == ' ' || baseService.back() == '+')) baseService.pop_back();
        consultationRequested = true;
        return 1;
    }
    return 1;
}

int buildDailyOccupancy(const Config& cfg, int expertId, const string& date,
    Booking bookings[], int bkCount, int occupancy[], int occLen) {

    for (int i = 0; i < occLen; ++i) occupancy[i] = 0;
    int bookedMin = 0;

    for (int i = 0; i < bkCount; ++i) {
        if (bookings[i].expertId == expertId && bookings[i].date == date) {
            int start = static_cast<int>(bookings[i].startTime);
            int durH = (bookings[i].durationMin + 59) / 60;

            for (int h = 0; h < durH; ++h) {
                int idx = (start + h) - cfg.openHour;
                if (idx >= 0 && idx < occLen) {
                    occupancy[idx] = 1;
                }
            }

            bookedMin += bookings[i].durationMin;

        }
    }

    return bookedMin;

}

int weeklyMinutes(const Config& cfg, int expertId, const string& monday,
    Booking bookings[], int bkCount) {
    int sum = 0;
    int occLen = cfg.closeHour - cfg.openHour;
    int occ[24];
    for (int d = 0; d < 5; ++d) {
        string day = addDays(monday, d);
        sum += buildDailyOccupancy(cfg, expertId, day, bookings, bkCount, occ, occLen);
    }
    return sum;
}

int checkBookingFit(const Config& cfg, int expertId, const string& date, int startHour, int durationMin,
    Booking bookings[], int bkCount, int& outDayBookedMin, int& outWeekMin) {
    int y, m, d; if (!parseDate(date, y, m, d)) return 4;
    if (!isDecemberWeekday2025(date)) return 4;
    int occLen = cfg.closeHour - cfg.openHour;
    if (startHour < cfg.openHour || startHour + (durationMin / 60) > cfg.closeHour) return 4;
    int occ[24]; int dayBooked = buildDailyOccupancy(cfg, expertId, date, bookings, bkCount, occ, occLen);
    int startIdx = startHour - cfg.openHour;
    int needSlots = durationMin / 60;
    for (int k = 0; k < needSlots; ++k) {
        if (startIdx + k < 0 || startIdx + k >= occLen) return 4;
        if (occ[startIdx + k]) return 1;
    }
    int newDayMin = dayBooked + durationMin;
    if (newDayMin > cfg.dayLimitMin) { outDayBookedMin = dayBooked; return 2; }
    string mon = mondayOfWeek(date);
    int weekMin = weeklyMinutes(cfg, expertId, mon, bookings, bkCount);
    int newWeekMin = weekMin + durationMin;
    if (newWeekMin > cfg.weekLimitMin) { outWeekMin = weekMin; return 3; }
    outDayBookedMin = dayBooked;
    outWeekMin = weekMin;
    return 0;
}

int serviceIndexForExpertOnDay(int expertIndex, int dayOfMonth) {
    // Validate dayOfMonth (assuming month has up to 31 days)
    if (dayOfMonth < 1 || dayOfMonth > 31) {
        return 0; // Default to first service for invalid days
    }

    // Services rotate every 3 days
    int dayGroup = (dayOfMonth - 1) / 3;

    // Each expert has their own rotation pattern
    switch (expertIndex) {
    case 0: // June
        return (0 + dayGroup) % 3; // Foot, Full Body, Hot Stone
    case 1: // Bryan
        return (1 + dayGroup) % 3; // Full Body, Hot Stone, Foot
    case 2: // Amy
        return (2 + dayGroup) % 3; // Hot Stone, Foot, Full Body
    default:
        return 0; // Default to first service for invalid expertIndex
    }
}

int viewIndividualSchedule(const Config& cfg, int expertId, Expert experts[], int expertN,
    Service services[], int svcN, Booking bookings[], int bkCount, const string& anyDate) {
    int y, m, d;
    if (!parseDate(anyDate, y, m, d))
        return 0;

    string monday = mondayOfWeek(anyDate);
    int occLen = cfg.closeHour - cfg.openHour;

    cout << "\nSchedule for " << experts[expertId].name << " (Week of " << monday << ")\n\n";
    cout << left << setw(12) << "Date";
    for (int h = cfg.openHour; h < cfg.closeHour; ++h) {
        cout << setw(8) << (to_string(h) + ":00");
    }
    cout << setw(20) << "Assigned Service" << setw(15) << "Day Total" << "\n";
    cout << string(12 + occLen * 8 + 35, '-') << "\n";

    int weeklyBookedMin = 0;

    for (int d = 0; d < 5; ++d) {
        string day = addDays(monday, d);
        int yy, mm, dd;
        parseDate(day, yy, mm, dd);

        int svcIndex = serviceIndexForExpertOnDay(expertId, dd);
        string assignedService = (svcIndex >= 0 && svcIndex < svcN) ? services[svcIndex].name : "N/A";
        string displayName = assignedService;
        size_t pos = displayName.find(" (");
        if (pos != string::npos) displayName = displayName.substr(0, pos);

        int occ[24] = { 0 };
        int dayBookedMin = 0;

        for (int b = 0; b < bkCount; ++b) {
            if (bookings[b].expertId == expertId && bookings[b].date == day) {
                int st = static_cast<int>(bookings[b].startTime);
                int durH = (bookings[b].durationMin + 59) / 60;
                for (int h = 0; h < durH; ++h) {
                    int idx = st + h - cfg.openHour;
                    if (idx >= 0 && idx < occLen) occ[idx] = 1;
                }
                dayBookedMin += bookings[b].durationMin;
            }
        }

        cout << left << setw(12) << day;
        for (int i = 0; i < occLen; ++i) {
            string label = occ[i] ? "Booked" : "Free  ";
            cout << setw(8) << label;
        }

        cout << setw(20) << displayName << fixed << setprecision(2)
            << (dayBookedMin / 60.0) << "h / " << (cfg.dayLimitMin / 60.0) << "h\n";
        cout << string(12 + occLen * 8 + 35, '-') << "\n";
        weeklyBookedMin += dayBookedMin;
    }

    cout << "\nWeekly total: " << fixed << setprecision(2)
        << (weeklyBookedMin / 60.0) << "h / " << (cfg.weekLimitMin / 60.0) << "h\n";
    return 5;
}

int viewAssignedCustomerList(const Config& cfg, int expertId, Booking bookings[], int bkCount,
    Service services[], int svcN, const string& anyDate, Expert experts[]) {

    string monday = mondayOfWeek(anyDate);
    int realExpertId = experts[expertId].id;

    cout << "\n" << string(75, '=') << "\n";
    cout << "ASSIGNED CUSTOMERS - " << experts[expertId].name << " (Week of " << monday << ")\n";
    cout << string(75, '=') << "\n";

    // Header with proper column widths
    cout << left
        << setw(12) << "DATE" << " | "
        << setw(6) << "TIME" << " | "
        << setw(12) << "CUSTOMER" << " | "
        << setw(25) << "SERVICE" << " | "
        << setw(8) << "DURATION" << "\n";

    cout << string(12, '-') << "-+-"
        << string(6, '-') << "-+-"
        << string(12, '-') << "-+-"
        << string(25, '-') << "-+-"
        << string(8, '-') << "\n";

    int count = 0;
    double totalHours = 0.0;

    for (int d = 0; d < 5; ++d) {
        string day = addDays(monday, d);
        for (int b = 0; b < bkCount; ++b) {
            if (bookings[b].expertId == realExpertId && bookings[b].date == day) {
                // Format time
                string timeStr = to_string(static_cast<int>(bookings[b].startTime)) + ":00";
                if (timeStr.length() == 4) timeStr = "0" + timeStr;

                // Shorten customer name if too long
                string customerDisplay = bookings[b].customer;
                if (customerDisplay.length() > 11) {
                    customerDisplay = customerDisplay.substr(0, 9) + "..";
                }

                // Shorten service name if too long
                string serviceDisplay = bookings[b].serviceName;
                if (serviceDisplay.length() > 24) {
                    serviceDisplay = serviceDisplay.substr(0, 22) + "..";
                }

                // Format duration
                string durationStr = to_string(bookings[b].durationMin / 60) + "h";

                cout << left
                    << setw(12) << bookings[b].date << " | "
                    << setw(6) << timeStr << " | "
                    << setw(12) << customerDisplay << " | "
                    << setw(25) << serviceDisplay << " | "
                    << setw(8) << durationStr << "\n";

                count++;
                totalHours += bookings[b].durationMin / 60.0;
            }
        }
    }

    if (count == 0) {
        cout << "No customers assigned this week.\n";
    }

    cout << string(75, '=') << "\n";
    return count;

}

double viewEarningsBonus(const Config& cfg, int expertId, Booking bookings[], int bkCount,
    Service services[], int svcN, Expert experts[]) {

    int treatMin = 0;
    int consultMin = 0;
    double totalRM = 0.0;

    for (int b = 0; b < bkCount; ++b) {
        if (bookings[b].expertId != expertId)
            continue;

        if (!isDecemberWeekday2025(bookings[b].date))
            continue;

        if (bookings[b].hasConsultation) {
            consultMin += 60;
            totalRM += 50.0;
            int treatmentDuration = bookings[b].durationMin - 60;
            double treatmentPrice = bookings[b].price - 50.0;
            treatMin += treatmentDuration;
            totalRM += treatmentPrice;
        }
        else {
            treatMin += bookings[b].durationMin;
            totalRM += bookings[b].price;
        }
    }

    double totalH = (treatMin + consultMin) / 60.0;
    double rate = 0.0;

    if (totalH >= 30.0)
        rate = 0.50;
    else if (totalH >= 25.0)
        rate = 0.25;

    double bonus = totalRM * rate;

    cout << "\nEarnings & Bonus (December 2025)\n";
    cout << "Total treatment hours    : " << fixed << setprecision(2) << (treatMin / 60.0) << "h\n";
    cout << "Total consultation hours : " << fixed << setprecision(2) << (consultMin / 60.0) << "h\n";
    cout << "Total combined hours     : " << fixed << setprecision(2) << totalH << "h\n";
    cout << "Total service value      : RM" << fixed << setprecision(2) << totalRM << "\n";
    cout << "Bonus percentage         : " << (rate * 100.0) << "%\n";
    cout << "Bonus amount earned      : RM" << fixed << setprecision(2) << bonus << "\n";

    return bonus;
}

int expertLogin(Expert experts[], int expertN) {
    string u, p;
    cout << "Expert login\nUsername: ";
    cin >> u;

    if (!isAlphaNumeric(u)) {
        cout << "Invalid username format!\n";
        return -1;
    }

    cout << "Password: ";
    cin >> p;

    if (!isAlphaNumeric(p)) {
        cout << "Invalid password format!\n";
        return -1;
    }

    for (int i = 0; i < expertN; ++i) {
        if (experts[i].username == u && experts[i].password == p) return i;
    }
    return -1;
}

int expertMenu() {
    goToPage();
    cout << u8"╔════════════════════════════════════════╗\n";
    cout << u8"║              Expert Menu               ║\n";
    cout << u8"╠════════════════════════════════════════╣\n";
    cout << u8"║ 1. View Individual Schedule            ║\n";
    cout << u8"║ 2. View Assigned Customer List         ║\n";
    cout << u8"║ 3. View Earnings & Bonus Entitlement   ║\n";
    cout << u8"║ 4. Sort My Bookings by Date            ║\n";
    cout << u8"║ 5. Exit                                ║\n";
    cout << u8"╚════════════════════════════════════════╝\n";   

    return getValidNumericInput("\nSelect: ", 1, 5);
}

// ===== BUBBLE SORT IMPLEMENTATION (EXPERT ONLY - DATE ONLY) ===== //
Booking* bubbleSortBookingsByDate(Booking bookings[], int count, Service services[], int svcN) {
    if (count <= 0) return nullptr;

    Booking* sortedBookings = new Booking[count];
    for (int i = 0; i < count; i++) {
        sortedBookings[i] = bookings[i];
    }

    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (sortedBookings[j].date > sortedBookings[j + 1].date) {
                Booking temp = sortedBookings[j];
                sortedBookings[j] = sortedBookings[j + 1];
                sortedBookings[j + 1] = temp;
            }
        }
    }

    return sortedBookings;
}

string getSortedBookingsByDateDisplay(Booking bookings[], int count, Service services[], int svcN) {
    if (count <= 0) {
        return "No bookings found to sort.\n";
    }
    stringstream ss;
    ss << "\n=== My Bookings Sorted by Date ===\n";
    ss << left << setw(12) << "Date" << setw(8) << "Time" << setw(20) << "Customer"
        << setw(30) << "Service" << setw(10) << "Duration" << "Price" << "\n";
    ss << string(95, '-') << "\n";

    for (int i = 0; i < count; i++) {
        string timeStr = to_string(static_cast<int>(bookings[i].startTime)) + ":00";

        if (timeStr.length() == 4) timeStr = "0" + timeStr;
        string priceStr = "RM" + to_string_with_precision(bookings[i].price, 2);

        ss << left << setw(12) << bookings[i].date
            << setw(8) << timeStr
            << setw(20) << bookings[i].customer.substr(0, 19)
            << setw(30) << bookings[i].serviceName.substr(0, 29)
            << right << setw(8) << (bookings[i].durationMin / 60)
            << setw(10) << priceStr << "\n";
    }
    ss << string(95, '-') << "\n";
    ss << "Total bookings: " << count << "\n";
    return ss.str();
}

// =================================================================================================
//                                  ADMIN FUNCTION
// =================================================================================================
bool adminLogin() {
    string aUsername, aPass; int attempts = 0;
    while (attempts < 3) {
        cout << "\n";
        cout << u8"╔═════════════════════════════════════════════╗\n";
        cout << u8"║                  Admin Login                ║\n";
        cout << u8"╚═════════════════════════════════════════════╝";
        do {
            cout << "\nUsername: ";
            clearInputBuffer();
            getline(cin, aUsername);
            if (!isAlphaNumeric(aUsername)) {
                cout << "Invalid username! Only letters and numbers allowed.\n";
            }
        } while (!isAlphaNumeric(aUsername));

        cout << "Password: ";
        cin >> aPass;

        if (aUsername == "admin123" && aPass == "abc123") {
            cout << "\nLogin successful. Welcome back, " << aUsername << " !\n";
            string prompt = "Press ENTER to continue...";
            bool empty = true;
			getline(cin, aPass); // Clear newline from previous input
			getString(prompt, empty);
            return true; // Login successful, exit the loop
        }
        else {
            cout << "\nOpps! Invalid username or password. ";
            attempts++;
            if (attempts < 3) {
                cout << (3 - attempts) << " attempts left, please try again...\n";
                continue; // Continue to next iteration for another attempt
            }
            else {
                cout << "\nAccess denied! You have reached the attempt limits." << endl;
                return false; // Login failed after 3 attempts, exit the loop
            }
        }
    }
}

int adminMenu() {
    goToPage();
    cout << u8"╔════════════════════════════════════════╗\n";
    cout << u8"║              Admin Menu                ║\n";
    cout << u8"╠════════════════════════════════════════╣\n";
    cout << u8"║ 1. View Individual Expert Schedule     ║\n";
    cout << u8"║ 2. View Overall Schedule               ║\n";
    cout << u8"║ 3. View Customer List                  ║\n";
    cout << u8"║ 4. Generate Sales Report               ║\n";
    cout << u8"║ 5. View Expert Bonus Entitlement       ║\n";
    cout << u8"║ 6. Monthly Summary Report              ║\n";
    cout << u8"║ 7. Logout                              ║\n";
    cout << u8"╚════════════════════════════════════════╝\n";

    return getValidNumericInput("\nSelect: ", 1, 7);
    pauseForMenu();
}

// 1. View Individual Expert Schedule
int adminViewExpertSchedule(const Config& cfg, int expertId, Expert experts[], int expertN, Service services[], int svcN, Booking bookings[], int bkCount) {
    string date = getValidDateInput(); // Use your input validation for date
    if (!isDecember(date)) {
        cout << "Invalid date! Only December is allowed.\n";
        return 0;
    }
    if (!isDecemberWeekday2025(date)) {
        cout << "Sorry, only weekdays in December 2025 are allowed.\n";
        return 0;
    }
    loadBooking(bookings, bkCount, experts, expertN); // Ensure bookings are up to date
    viewIndividualSchedule(cfg, expertId, experts, expertN, services, svcN, bookings, bkCount, date);
    return 1;
}

// 2. View Overall Schedule


// 3. View Customer List
void viewCustomerList(Booking bookings[], int bkCount, Expert experts[], int expertN, Service services[], int svcN) {
    if (bkCount == 0) {
        cout << "\nNo customer bookings found.\n";
        return;
    }

	cout << "\n";
    cout << u8"╔══════════════════════════════════════════════════════════════╗\n";
    cout << u8"║                     Customer Booking List                    ║\n";
    cout << u8"╚══════════════════════════════════════════════════════════════╝\n\n";

    // Collect unique customers
    string customers[100]; 
    int custCount = 0;

    for (int i = 0; i < bkCount; i++) {
        string c = bookings[i].customer;
        bool exists = false;
        for (int j = 0; j < custCount; j++) {
            if (customers[j] == c) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            customers[custCount++] = c;
        }
    }

    // Display each customer's bookings
    for (int i = 0; i < custCount; i++) {
            cout << u8"╔═════════════════════════════════════════════════════╗\n";
            cout << u8"║ Customer : " << setw(41) << left << customers[i] << u8"║\n";
            cout << u8"╟─────────────────────────────────────────────────────╢\n";

            double totalSpent = 0.0;

            for (int b = 0; b < bkCount; b++) {
                if (bookings[b].customer == customers[i]) {
                    string timeStr = to_string(static_cast<int>(bookings[b].startTime)) + ":00";
                    if (timeStr.length() == 4) timeStr = "0" + timeStr;

                    cout << u8"║ Date     : " << setw(41) << left << bookings[b].date  << u8"║\n";
                    cout << u8"║ Time     : " << setw(41) << left << timeStr <<  u8"║\n";
                    cout << u8"║ Service  : " << setw(41) << left << bookings[b].serviceName << u8"║\n";
                    cout << u8"║ Expert   : " << setw(41) << left << bookings[b].expertName << u8"║\n";
                    cout << u8"║ Price    : RM" << fixed << setprecision(2) << setw(39) << left << bookings[b].price << u8"║\n";
                    cout << u8"╠═════════════════════════════════════════════════════╣\n";

                    totalSpent += bookings[b].price;
                }
            }

            // Per-customer total
            cout << u8"║ Total Spent : RM " << setw(35) << left << setprecision(2) << totalSpent << u8"║\n";
            cout << u8"╚═════════════════════════════════════════════════════╝\n\n";
    }
}
// 4. Generate Sales Report
double adminGenerateSalesReport(Booking bookings[], int bkCount) {
    double total = 0;
    for (int i = 0; i < bkCount; ++i) {
        total += bookings[i].price;
    }
    return total;
}

// 5. View Expert Bonus
double adminViewExpertBonus(Booking bookings[], int bkCount, Expert experts[], int expertN, int expertId) {
    double totalH = 0, totalRM = 0;
    for (int i = 0; i < bkCount; ++i) {
        if (bookings[i].expertId == experts[expertId].id) {
            totalH += bookings[i].durationMin / 60.0;
            totalRM += bookings[i].price;
        }
    }
    double rate = (totalH >= 30 ? 0.5 : (totalH >= 25 ? 0.25 : 0));
    double bonus = totalRM * rate;
    return bonus;
}

// 6. Monthly Summary Report
double adminMonthlySummary(Booking bookings[], int bkCount) {
    return adminGenerateSalesReport(bookings, bkCount);
}

// =================================================================================================
//                                  MAIN FUNCTION
// =================================================================================================

int main() {
    // Set console to UTF-8 for proper Unicode display
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // Initialize data
    int userCount = 0;
    string usernames[MAX_USERS];
    string passwords[MAX_USERS];

    string expertNames[NUM_EXPERTS] = { "June", "Bryan", "Amy" };
    string serviceNames[NUM_SERVICES] = { "Foot Massage (Reflexology)", "Full Body Massage", "Hot Stone Massage" };
    string packageNames[NUM_PACKAGES] = { "Package A (Body + Hot Stone)", "Package B (Foot + Hot Stone)" };
    double servicePrices[NUM_SERVICES][2] = { {80.00, 158.00}, {90.00, 178.00}, {140.00, 278.00} };
    double packagePrices[NUM_PACKAGES] = { 218.00,  208.00 };

    // Expert system initialization
    Config cfg;
    cfg.dayLimitMin = 360;
    cfg.weekLimitMin = 1800;
    cfg.openHour = 12;
    cfg.closeHour = 21;

    Expert experts[NUM_EXPERTS];
    int expertN = 0;
    loadExperts(experts, expertN);

    Service services[] = {
        {100, "Foot Massage (Reflexology)", "Treatment", 60, 80.0},
        {101, "Full Body Massage", "Treatment", 60, 90.0},
        {102, "Hot Stone Massage", "Treatment", 60, 140.0},
        {103, "Package A (Body + Hot Stone)", "Package", 120, 218.0},
        {104, "Package B (Foot + Hot Stone)", "Package", 120, 208.0},
        {200, "Consultation", "Consultation", 60, 50.0}
    };
    int svcN = sizeof(services) / sizeof(services[0]);

    Booking bookings[BK_CAP];
    int bkCount = 0;

    loadCustomer(usernames, passwords, userCount);
    loadBooking(bookings, bkCount, experts, expertN);

    // Main menu
    int roleChoice;
    do {
		goToPage();
        cout << "1. Customer" << endl;
        cout << "2. Expert" << endl;
        cout << "3. Admin" << endl;
        cout << "4. Exit" << endl;

        roleChoice = getValidNumericInput("\nSelect role: ", 1, 4);

        switch (roleChoice) {
        case 1: {
            int userIndex = -1;
            int custOption = 0;
            do {
                cout << "\n===== Customer REGISTER / LOGIN =====";
                cout << "\n1. Register" << endl;
                cout << "2. Login" << endl;
                cout << "3. Back" << endl;

                int custOption = getValidNumericInput("Select: ", 1, 3);

                if (custOption == 1) {
					goToPage();
                    registerUser(usernames, passwords, userCount);
                }
                else if (custOption == 2) {
                    goToPage();
                    userIndex = loginUser(usernames, passwords, userCount);
                    if (userIndex != -1)
                        break; // successful login
                }
                else if (custOption == 3) {
                    cout << "\nReturning to main menu...\n" << endl;
                    this_thread::sleep_for(chrono::seconds(1)); // Pause for 1 sec before returning to main menu
                    break; // go back to role menu
                }
            } while (custOption != 3);

            if (userIndex != -1) {
                int custMenuChoice = 0;
                do {
                    custMenuChoice = customerMenu();

                    switch (custMenuChoice) {
                    case 1:
                        viewBeautyCentreInfo();
                        pauseForMenu();
                        break;
                    case 2:
                        viewServicesAndExperts(serviceNames, packageNames, servicePrices, packagePrices,
                            experts, expertN, services, svcN);
                        pauseForMenu();
                        break;
                    case 3:
                    {
                        string checkDate;
                        cout << "Enter a December 2025 date to check availability (YYYY-MM-DD): ";
                        cin >> checkDate;

                        // Validate date
                        if (!isValidDate(checkDate) || !isDecember(checkDate)) {
                            cout << "Invalid date! Please enter a valid December 2025 date.\n";
                        }
                        else {
                            checkAppointmentAvailability(checkDate, bookings, bkCount, experts, expertN, cfg, services, svcN);
                        }
                    }
                    pauseForMenu();
                    break;
                    case 4: // Book Appointment
                        bookAppointment(usernames[userIndex], serviceNames, packageNames, expertNames,
                            servicePrices, packagePrices, bookings, bkCount, experts, expertN, services,
                            svcN, cfg);
                        pauseForMenu();
                        break;
                    case 5: // View My Bookings
                        viewMyBookings(bookings, bkCount, usernames[userIndex]);
                        pauseForMenu();
                        break;
                    case 6: // Logout
                        cout << "Logging out..." << endl;
                        this_thread::sleep_for(chrono::seconds(1));
                        break;
                    }
                } while (custMenuChoice != 6);
            }
            break;
        }

        case 2: {
            int idx = expertLogin(experts, expertN);
            if (idx < 0) {
                cout << "Invalid login.\n";
                break;
            }

            int expertMenuChoice = -1;
            do {
                goToPage();
                cout << "\nWelcome, " << experts[idx].name << "!\n";
                expertMenuChoice = expertMenu();

                switch (expertMenuChoice) {
                case 1: {
                    string date;
                    cout << "\nEnter any December 2025 date (YYYY-MM-DD) in the week you want to view: ";
                    cin >> date;

                    if (!isValidDate(date) || !isDecember(date)) {
                        cout << "Invalid date! Please enter a valid December 2025 date in YYYY-MM-DD format.\n";
                        break;
                    }
                    if (!isDecemberWeekday2025(date)) {
                        int y, m, d;
                        if (!parseDate(date, y, m, d) || y != 2025 || m != 12) {
                            cout << "Please enter a date in Dec 2025.\n";
                            break;
                        }
                    }
                    loadBooking(bookings, bkCount, experts, expertN);
                    viewIndividualSchedule(cfg, idx, experts, expertN, services, svcN, bookings, bkCount, date);
                    pauseForMenu();
                    break;
                }
                case 2: {
                    loadBooking(bookings, bkCount, experts, expertN);

                    string date;
                    cout << "\nEnter any December 2025 date (YYYY-MM-DD) in the week you want to view: ";
                    cin >> date;

                    if (!isValidDate(date) || !isDecember(date)) {
                        cout << "Invalid date! Please enter a valid December 2025 date in YYYY-MM-DD format.\n";
                        break;
                    }
                    if (!isDecemberWeekday2025(date)) {
                        int y, m, d;
                        if (!parseDate(date, y, m, d) || y != 2025 || m != 12) {
                            cout << "Please enter a date in Dec 2025.\n";
                            break;
                        }
                    }
                    viewAssignedCustomerList(cfg, idx, bookings, bkCount, services, svcN, date, experts);
                    pauseForMenu();
                    break;
                }
                case 3:
                    loadBooking(bookings, bkCount, experts, expertN);
                    viewEarningsBonus(cfg, idx, bookings, bkCount, services, svcN, experts);
                    pauseForMenu();
                    break;
                case 4: {
                    int expertBookingsCount = 0;
                    Booking expertBookings[BK_CAP];

                    // Collect bookings for this expert
                    for (int i = 0; i < bkCount; i++) {
                        if (bookings[i].expertId == experts[idx].id) {
                            expertBookings[expertBookingsCount++] = bookings[i];
                        }
                    }

                    if (expertBookingsCount == 0) {
                        cout << "No bookings found to sort.\n";
                        break;
                    }

                    // Sort and display
                    Booking* sortedBookings = bubbleSortBookingsByDate(expertBookings, expertBookingsCount, services, svcN);
                    if (sortedBookings != nullptr) {
                        string displayResult = getSortedBookingsByDateDisplay(sortedBookings, expertBookingsCount, services, svcN);
                        cout << displayResult;
                        delete[] sortedBookings;
                    }
                    pauseForMenu();
                    break;
                }
                case 5:
                    cout << "Goodbye!\n";
                    break;
                }
            } while (expertMenuChoice != 6);
            break;
        }

        case 3: {
            if (adminLogin()) {
                int adminMenuChoice;
                do {
                    adminMenuChoice = adminMenu(); // show menu, get input
                    switch (adminMenuChoice) {
                    case 1: {
                        goToPage();
                        cout << u8"╔═══════════════════════════════════════════════════╗\n";
                        cout << u8"║          View Individual Expert Schedule          ║\n";
                        cout << u8"╚═══════════════════════════════════════════════════╝\n";

                        int expID = getValidNumericInput("\nEnter Expert ID (0: June, 1: Bryan, 2: Amy): ", 0, expertN - 1);

                       // adminViewExpertSchedule(cfg, expID, experts, expertN, services, svcN, bookings, bkCount);
                        pauseForMenu();
                        break;
                    }
                    case 2: {
                        string weekDate;
                        while (true) {
                            cout << "Enter any December 2025 date (YYYY-MM-DD) for the week to view: ";
                            cin >> weekDate;
                            // Use robust validation
                            if (isValidDate(weekDate) && isDecember(weekDate)) {
                                int y, m, d;
                                if (parseDate(weekDate, y, m, d) && y == 2025 && m == 12 && d >= 1 && d <= 31) {
                                    break;
                                }
                            }
                            cout << "Invalid date! Please enter a date in the format 2025-12-DD.\n";
                        }
                        string monday = mondayOfWeek(weekDate);
                        adminViewOverallSchedule(cfg, bookings, bkCount, experts, expertN, monday);
                        pauseForMenu();
                        break;
                    }
                    case 3:
                        viewCustomerList(bookings, bkCount, experts, expertN, services, svcN);
                        pauseForMenu();
                        break;
                    case 4:
                        cout << "\n=== Sales Report ===\n";
                        cout << "Total Sales: RM" << fixed << setprecision(2) << adminGenerateSalesReport(bookings, bkCount) << "\n";
                        pauseForMenu();
                        break;
                    case 5: {
                        int id;
                        cout << "Enter Expert ID (0: June, 1: Bryan, 2: Amy): ";
                        cin >> id;
                        cout << "Expert Bonus: RM" << fixed << setprecision(2) << adminViewExpertBonus(bookings, bkCount, experts, expertN, id) << "\n";
                        pauseForMenu();
                        break;
                    }
                    case 6:
                        cout << "\n=== Monthly Summary Report ===\n";
                        cout << "Total Sales: RM" << fixed << setprecision(2) << adminMonthlySummary(bookings, bkCount) << "\n";
                        pauseForMenu();
                        break;
                    case 7:
                        cout << "\nLogging out admin system..." << endl;
                        pauseForMenu();
                        break;
                    }
                } while (adminMenuChoice != 7);
            }
            else {
                cout << "\nReturning to main menu...\n" << endl;
				this_thread::sleep_for(chrono::seconds(1)); // Pause for 2 seconds before returning to main menu
            }
            break;
        }

        case 4: {
            cout << "\nThanks for using our system! Exiting ...\n";
            break;
        }
        }
    } while (roleChoice != 4);

    // Save all data before exiting
    saveCustomers(usernames, passwords, userCount);
    saveBooking(bookings, bkCount);
    saveExperts(experts, expertN);

    return 0;
}

