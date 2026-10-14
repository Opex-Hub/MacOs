#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <ncurses.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <thread>

using namespace std;

// Structure to represent a script
struct Script {
    string name;
    string description;
    string code;
};

// Current state
int currentTab = 0; // 0 = Home, 1 = Scripts, 2 = Execute
vector<Script> scripts;
string luaCode = "-- Enter your Lua code here\n";
int cursorPos = 0;
bool insertMode = false;
string searchTerm = "";
int selectedScript = 0;
vector<Script> filteredScripts;

void initScripts() {
    // Sample scripts for demonstration
    scripts.push_back({"Auto Farm", "Universal auto farm script", "while true do\n  wait(1)\n  -- Farming logic here\nend"});
    scripts.push_back({"ESP", "Player ESP display", "for _, player in pairs(game.Players:GetPlayers()) do\n  -- ESP logic\nend"});
    scripts.push_back({"Speed Hack", "Increase walk speed", "game.Players.LocalPlayer.Character.Humanoid.WalkSpeed = 50"});
    scripts.push_back({"Fly Script", "Allows flying", "wait(1)\n-- Fly implementation"});
    scripts.push_back({"Infinite Yield", "Admin commands GUI", "loadstring(game:HttpGet('https://raw.githubusercontent.com/EdgeIY/infiniteyield/master/source'))()"});
    scripts.push_back({"Dex Explorer", "Model explorer", "loadstring(game:HttpGet('https://raw.githubusercontent.com/Babyhamsta/RBLX_Scripts/main/Universal/BypassedDarkDexV3.lua'))()"});
}

void openRobloxFolder() {
    // Command to open Roblox folder on macOS
    system("open ~/Library/Application\\ Support/Roblox");
}

void saveLuaCode(const string& code) {
    ofstream file("saved_code.lua");
    if (file.is_open()) {
        file << code;
        file.close();
    }
}

string loadLuaCode() {
    ifstream file("saved_code.lua");
    string code;
    if (file.is_open()) {
        stringstream buffer;
        buffer << file.rdbuf();
        code = buffer.str();
        file.close();
    }
    return code.empty() ? "-- Enter your Lua code here\n" : code;
}

void drawHomeTab() {
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);
    
    mvprintw(3, 2, "Welcome to Roblox UI Connector!");
    mvprintw(5, 2, "Features:");
    mvprintw(6, 4, "- Press TAB to navigate between tabs");
    mvprintw(7, 4, "- Search scripts by game name");
    mvprintw(8, 4, "- Execute custom Lua code");
    mvprintw(9, 4, "- Full loadstring support");
    
    mvprintw(maxY - 3, 2, "Press 'A' to open Roblox folder");
    mvprintw(maxY - 2, 2, "Press 'Q' to quit");
}

void drawScriptsTab() {
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);
    
    mvprintw(3, 2, "Script Search: %s", searchTerm.c_str());
    
    // Draw search box
    mvprintw(3, 17, "______________________________");
    
    mvprintw(5, 2, "Available Scripts:");
    
    // Filter scripts based on search term
    filteredScripts = filterScripts(searchTerm);
    
    int startY = 7;
    for (size_t i = 0; i < filteredScripts.size() && startY + i < maxY - 3; ++i) {
        if (static_cast<int>(i) == selectedScript) {
            attron(A_REVERSE);
        }
        mvprintw(startY + i, 4, "[Execute]");
        mvprintw(startY + i, 15, "%s", filteredScripts[i].name.c_str());
        mvprintw(startY + i, 35, "%s", filteredScripts[i].description.c_str());
        if (static_cast<int>(i) == selectedScript) {
            attroff(A_REVERSE);
        }
    }
    
    mvprintw(maxY - 2, 2, "Use UP/DOWN arrows to navigate, ENTER to execute, BACKSPACE to edit search");
}

void drawExecuteTab() {
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);
    
    mvprintw(3, 2, "Lua Code Editor:");
    mvprintw(4, 2, "(Press 'I' to toggle insert mode, ESC to exit insert mode)");
    
    // Draw border for code area
    for (int i = 0; i < custom_min(50, maxX - 4); ++i) {
        mvaddch(5, 2 + i, '-');
    }
    
    // Display Lua code lines
    int lineStart = 6;
    stringstream ss(luaCode);
    string line;
    int lineNumber = 0;
    
    while (getline(ss, line) && lineStart + lineNumber < maxY - 4) {
        mvprintw(lineStart + lineNumber, 2, "%s", line.c_str());
        lineNumber++;
    }
    
    // Show cursor position when in insert mode
    if (insertMode) {
        mvprintw(4, 50, "INSERT MODE");
        // Simplified cursor display
        int cursorLine = cursorPos / 30; // Approximate line
        int cursorCol = cursorPos % 30;
        if (lineStart + cursorLine < maxY - 4) {
            mvchgat(lineStart + cursorLine, 2 + cursorCol, 1, A_REVERSE, 0, NULL);
        }
    } else {
        mvprintw(4, 50, "NORMAL MODE");
    }
    
    mvprintw(maxY - 3, 2, "Commands:");
    mvprintw(maxY - 2, 4, "'E' Execute Code | '+' New Script | 'S' Save | 'L' Load");
}

void drawUI() {
    clear();
    box(stdscr, 0, 0);
    
    // Title
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);
    mvprintw(1, 5, "Roblox UI Connector");
    
    // Tab bar
    attron(A_REVERSE);
    mvprintw(1, 25, " HOME ");
    mvprintw(1, 33, " SCRIPTS ");
    mvprintw(1, 43, " EXECUTE ");
    attroff(A_REVERSE);
    
    // Highlight current tab
    switch(currentTab) {
        case 0: mvprintw(1, 25, "[HOME]"); break;
        case 1: mvprintw(1, 33, "[SCRIPTS]"); break;
        case 2: mvprintw(1, 43, "[EXECUTE]"); break;
    }
    
    // "+" button in top right corner
    mvprintw(1, maxX - 5, "[ + ]");
    
    // Render current tab content
    switch(currentTab) {
        case 0: drawHomeTab(); break;
        case 1: drawScriptsTab(); break;
        case 2: drawExecuteTab(); break;
    }
    
    refresh();
}

void simulateExecution(const string& code) {
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);
    
    clear();
    box(stdscr, 0, 0);
    mvprintw(1, 5, "Executing Lua Code...");
    mvprintw(3, 2, "Code Preview:");
    
    // Show first few lines of code
    stringstream ss(code);
    string line;
    int lineNum = 0;
    while (getline(ss, line) && lineNum < 10) {
        mvprintw(5 + lineNum, 4, "%.50s", line.c_str()); // Limit line length
        lineNum++;
    }
    
    mvprintw(maxY - 3, 2, "Simulating code execution...");
    mvprintw(maxY - 2, 2, "(In real implementation, this would inject into Roblox)");
    
    refresh();
    std::this_thread::sleep_for(std::chrono::seconds(3));
}

vector<Script> filterScripts(const string& term) {
    vector<Script> result;
    if (term.empty()) {
        return scripts;
    }
    
    string termLower = term;
    transform(termLower.begin(), termLower.end(), termLower.begin(), ::tolower);
    
    for (const auto& script : scripts) {
        // Simple case-insensitive search
        string name = script.name;
        string desc = script.description;
        
        // Convert to lowercase for comparison
        transform(name.begin(), name.end(), name.begin(), ::tolower);
        transform(desc.begin(), desc.end(), desc.begin(), ::tolower);
        
        if (name.find(termLower) != string::npos || desc.find(termLower) != string::npos) {
            result.push_back(script);
        }
    }
    return result;
}

// Helper functions for min/max (avoiding conflicts with standard library)
template<typename T>
T custom_min(T a, T b) {
    return (a < b) ? a : b;
}

template<typename T>
T custom_max(T a, T b) {
    return (a > b) ? a : b;
}

int main() {
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    
    // Initialize scripts data
    initScripts();
    luaCode = loadLuaCode();
    
    // Draw initial UI
    drawUI();
    
    // Main loop
    int ch;
    while ((ch = getch()) != 'q') {
        if (insertMode) {
            // Handle text input in insert mode
            if (ch == 27) { // ESC key
                insertMode = false;
            } else if (ch == 10 || ch == 13) { // Enter key
                if (currentTab == 1) { // In search field
                    insertMode = false;
                } else {
                    luaCode.insert(cursorPos, "\n");
                    cursorPos += 1;
                }
                                    } else if (ch == KEY_BACKSPACE || ch == 127) { // Backspace
                if (currentTab == 1) { // In search field
                    if (!searchTerm.empty()) {
                        searchTerm.pop_back();
                        selectedScript = 0;
                    }
                } else if (cursorPos > 0) { // In code editor
                    luaCode.erase(cursorPos - 1, 1);
                    cursorPos--;
                }
                        } else if (ch >= 32 && ch <= 126) { // Printable characters
                            if (currentTab == 1) { // In search field
                                searchTerm += static_cast<char>(ch);
                            } else { // In code editor
                                luaCode.insert(cursorPos, 1, static_cast<char>(ch));
                                cursorPos++;
                            }
            } else if (ch == KEY_LEFT && cursorPos > 0) {
                cursorPos--;
            } else if (ch == KEY_RIGHT && cursorPos < static_cast<int>(luaCode.length())) {
                cursorPos++;
            }
        } else {
            // Normal navigation mode
            switch (ch) {
                case '\t': // Tab key
                    currentTab = (currentTab + 1) % 3;
                    searchTerm = ""; // Clear search when leaving Scripts tab
                    selectedScript = 0;
                    break;
                case 'a':
                case 'A':
                    if (currentTab == 0) {
                        openRobloxFolder();
                        mvprintw(10, 2, "Opened Roblox folder! Check your Finder.");
                        refresh();
                        std::this_thread::sleep_for(std::chrono::seconds(2));
                    }
                    break;
                case 'i':
                case 'I':
                    if (currentTab == 2) {
                        insertMode = true;
                    } else if (currentTab == 1) {
                        insertMode = true;
                    }
                    break;
                case 'e':
                case 'E':
                    if (currentTab == 2) {
                        simulateExecution(luaCode);
                        saveLuaCode(luaCode);
                        std::this_thread::sleep_for(std::chrono::seconds(2));
                    }
                    break;
                case KEY_ENTER:
                case 10:
                case 13:
                    if (currentTab == 1) {
                        // Execute selected script
                        if (!filteredScripts.empty()) {
                            simulateExecution(filteredScripts[selectedScript].code);
                            std::this_thread::sleep_for(std::chrono::seconds(2));
                        }
                    }
                    break;
                            } else if (ch == KEY_UP) {
                if (currentTab == 1) {
                    selectedScript = custom_max(0, selectedScript - 1);
                }
                break;
            case KEY_DOWN:
                if (currentTab == 1) {
                    filteredScripts = filterScripts(searchTerm);
                    if (!filteredScripts.empty()) {
                        selectedScript = custom_min(selectedScript + 1, static_cast<int>(filteredScripts.size()) - 1);
                    }
                }
                    break;
                case '+':
                    if (currentTab == 2) {
                        luaCode += "\n-- New section --\n";
                        cursorPos = luaCode.length();
                    }
                    break;
                case 's':
                case 'S':
                    if (currentTab == 2) {
                        saveLuaCode(luaCode);
                        mvprintw(2, 50, "Saved!");
                        refresh();
                        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 0.5 seconds
                    }
                    break;
                case 'l':
                case 'L':
                    if (currentTab == 2) {
                        luaCode = loadLuaCode();
                        cursorPos = luaCode.length();
                        mvprintw(2, 50, "Loaded!");
                        refresh();
                        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 0.5 seconds
                    }
                    break;
                case KEY_RESIZE:
                    break;
            }
        }
        
        drawUI();
        
        // Ensure cursor visibility in insert mode
        if (insertMode) {
            curs_set(1);
        } else {
            curs_set(0);
        }
    }
    
    // Save code on exit
    saveLuaCode(luaCode);
    
    // Clean up ncurses
    endwin();
    return 0;
}
