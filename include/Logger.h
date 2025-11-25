#pragma once
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>

class Logger {
private:
    std::ofstream logFile;
    int currentTick;
    
public:
    Logger(const std::string& filename) : currentTick(0) {
        logFile.open(filename, std::ios::out | std::ios::trunc);
        if (logFile.is_open()) {
            logFile << "=== AI BATTLE DETAILED LOG ===" << std::endl;
            logFile << "Generated: " << __DATE__ << " " << __TIME__ << std::endl;
            logFile << "==============================" << std::endl << std::endl;
        }
    }
    
    ~Logger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }
    
    void setTick(int tick) {
        currentTick = tick;
    }
    
    void log(const std::string& category, const std::string& message) {
        if (logFile.is_open()) {
            logFile << "[TICK " << std::setw(5) << currentTick << "] "
                   << "[" << std::setw(10) << category << "] " 
                   << message << std::endl;
        }
    }
    
    void logAgent(const std::string& team, const std::string& type, int id, 
                  int x, int y, int hp, int ammo, int grenades, const std::string& state) {
        std::stringstream ss;
        ss << team << " " << type << "#" << id 
           << " pos(" << x << "," << y << ")"
           << " HP=" << hp
           << " Ammo=" << ammo  
           << " Grenades=" << grenades
           << " State: " << state;
        log("AGENT", ss.str());
    }
    
    void logMovement(const std::string& team, const std::string& type, int id,
                     int fromX, int fromY, int toX, int toY, const std::string& reason) {
        std::stringstream ss;
        ss << team << " " << type << "#" << id
           << " moved (" << fromX << "," << fromY << ") -> (" << toX << "," << toY << ")"
           << " Reason: " << reason;
        log("MOVE", ss.str());
    }
    
    void logCombat(const std::string& attacker, int attackerId, 
                   const std::string& target, int targetId,
                   const std::string& weapon, int damage, int newHP) {
        std::stringstream ss;
        ss << attacker << "#" << attackerId << " " << weapon << " -> "
           << target << "#" << targetId 
           << " Damage=" << damage << " NewHP=" << newHP;
        log("COMBAT", ss.str());
    }
    
    void logDecision(const std::string& agent, const std::string& decision, const std::string& reason) {
        std::stringstream ss;
        ss << agent << " decided: " << decision << " | " << reason;
        log("DECISION", ss.str());
    }
    
    void logError(const std::string& error) {
        log("ERROR", error);
    }
    
    void logGameState(int blueLiving, int orangeLiving, const std::string& state) {
        std::stringstream ss;
        ss << "Blue=" << blueLiving << " Orange=" << orangeLiving << " | " << state;
        log("GAMESTATE", ss.str());
    }
    
    void flush() {
        if (logFile.is_open()) {
            logFile.flush();
        }
    }
};

// Global logger instance
extern Logger* g_logger;
