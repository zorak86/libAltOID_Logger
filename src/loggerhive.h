#ifndef LOGGERHIVE_H
#define LOGGERHIVE_H

#include <string>
#include <list>

#include <sqlite3.h>

#include <alt_mutex/mutex_instance.h>

enum LogLevel
{
    LOG_X_ALL = 0x0,
    LOG_X_INFO = 0x1,
    LOG_X_WARN = 0x2,
    LOG_X_CRITICAL = 0x3,
    LOG_X_ERR = 0x4,
    LOG_X_DEBUG = 0x5,
    LOG_X_DEBUG1 = 0x6
};

struct LogElement {
    int id,severity;
    std::string date,module,user,ip,message;
};

#define LOG_M_SYSLOG 0x1
#define LOG_M_STD_OUTPUT 0x2
#define LOG_M_SQLITE 0x4
#define LOG_M_WIN_EVENTLOG 0x8

/**
 * Log Hive Class
 */
class LoggerHive
{
public:
    /**
     * Class constructor.
     * @param _logName Log Name
     * @param _appName Application Name
     * @param _logMode Log mode (LOG_M_SYSLOG, LOG_M_STD_OUTPUT, LOG_M_SQLITE)
     */
    LoggerHive(const std::string & _appName, const std::string & _logName, unsigned int _logMode = LOG_M_SQLITE);
    /**
     * Class destructor.
     */
    virtual ~LoggerHive();
    /**
     * Log some event.
     * @param logSeverity Log severity (ALL,INFO,WARN,CRITICAL,ERR,DEBUG,DEBUG1).
     * @param module Internal application module
     * @param user User triggering the log
     * @param ip IP triggering the log
     * @param fmtLog Log details
     */
    void LogEvent(LogLevel logSeverity, const std::string & module, const std::string & user,const std::string & ip, const char* fmtLog, ...);

    /**
     * @brief setDebug Set application in debug mode.
     * @param value true for debugging the application
     */
    void setDebug(bool value);

    /////////////////////////////////////////////////////////
    // Sqlite3 log manipulation:

    /**
     * Drop log events from SQLITE.
     */
    void DropLog();

    /**
     * Get the last log ID for sqlite3 logs
     * @return return the last log id
     */
    unsigned int GetLogLastID();

    /**
     * Get the list of log events from id_from to id_to...
     * @param id_from First ID to search (inclusive).
     * @param id_to Last ID to include (inclusive).
     * @param filter Filter TODO (not working yet)
     * @param logLevelFilter get only specific log level
     * @return list of log elements
     */
    std::list<LogElement>  GetLogView(unsigned int id_from, unsigned int id_to,const std::string & filter, LogLevel logLevelFilter = LOG_X_ALL);

private:
    bool CheckIfSQLITETableExist(const std::string &table);
    bool ExecSQLITEQuery(const std::string &query);
    bool ExecSQLITEQueryVA(const std::string& query, int _va_size, ...);
    bool IsWindowsEventLog();
    bool IsSysLog();
    bool IsSTDLog();
    bool IsSQLITELog();

    // Print functions:
    void PrintDate(FILE *fp);
    void PrintBold(FILE *fp, const char * str);
    void PrintBlue(FILE *fp, const char * str);
    void PrintRed(FILE *fp, const char * str);
    void PrintPurple(FILE *fp, const char * str);
    void PrintColorWin32(FILE *fp, unsigned short color, const char * str);

    void InitLog();

    unsigned int logMode;

    std::string appName;
    std::string logName;

    // Internal:
    std::string appLogDir;
    std::string appLogFile;

#ifndef NOSQLITE
    // DB
    sqlite3 *ppDb;
#endif
    Mutex_Instance mt;
    bool debug;
};



#endif // LOGGERHIVE_H
