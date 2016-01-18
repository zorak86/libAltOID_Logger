#include "loggerhive.h"
#include "alt_mutex/locker_mutex.h"

#ifdef _WIN32
#include <Shlobj.h>
#else
#include <pwd.h>
#include <syslog.h>
#endif

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

using namespace std;

LoggerHive::LoggerHive(const std::string & _appName, const std::string & _logName, unsigned int _logMode)
{
    appName = _appName;
    logName = _logName;
    logMode = _logMode;

#ifdef _WIN32
    TCHAR szPath[MAX_PATH];
    if(SUCCEEDED(SHGetFolderPath(NULL,
                                 CSIDL_LOCAL_APPDATA|CSIDL_FLAG_CREATE,
                                 NULL,
                                 0,
                                 szPath)))
    {
        // TODO: support wstring
        wstring szPath1(&szPath[0]);
        string szPath2(szPath1.begin(),szPath1.end());
        appLogDir = szPath2 + "\\" + _appName;
    }
    else
    {
        appLogDir = "c:\\" + _appName;
    }
    appLogFile = appLogDir + "/" + logName;

#else
    if (getuid()==0)
    {
        appLogDir = "/var/log/" + _appName;
    }
    else
    {
        const char *homedir;
        homedir = getpwuid(getuid())->pw_dir;
        appLogDir = string(homedir) + string("/.") +  _appName;
        if (access(appLogDir.c_str(),R_OK)) mkdir(appLogDir.c_str(),0700);
        appLogDir = appLogDir + string("/log");
    }
    appLogFile = appLogDir + "/" + logName;
#endif

    ppDb = NULL;
    debug = false;

    InitLog();
}

LoggerHive::~LoggerHive()
{
    mt.Lock();
    if (ppDb)
        sqlite3_close(ppDb);
    if (IsSysLog())
    {
#ifndef _WIN32
        closelog();
#endif
    }
}

void LoggerHive::LogEvent(LogLevel logSeverity, const std::string & module, const std::string & user,const std::string & ip, const char* fmtLog, ...)
{
    Locker_Mutex rlock(&mt);
    char buffer[8192];
    buffer[8191] = 0;

    std::list<LogElement> r;

    // take arguments...
    va_list args;
    va_start(args, fmtLog);
    vsnprintf(buffer, 8191, fmtLog, args);

    string firstSep = " - ";
    if (user != "")
    {
        firstSep += user + " - ";
    }
    if (ip != "")
    {
        firstSep += ip + " - ";
    }

    if (IsSysLog())
    {
#ifndef _WIN32
        if (logSeverity == LOG_X_INFO)
            syslog( LOG_INFO, buffer);
        else if (logSeverity == LOG_X_WARN)
            syslog( LOG_WARNING, buffer);
        else if (logSeverity == LOG_X_CRITICAL)
            syslog( LOG_CRIT, buffer);
        else if (logSeverity == LOG_X_ERR)
            syslog( LOG_ERR, buffer);
#endif
    }

    if (IsWindowsEventLog())
    {
        //TODO:
    }

    if (IsSTDLog())
    {
        char xdate[64]="";
        time_t x = time(NULL);
        struct tm *tmp = localtime(&x);
        strftime(xdate, 64, "%Y-%m-%dT%H:%M:%S%z", tmp);

        if (logSeverity == LOG_X_INFO)
        {
            fprintf(stdout, "[%s] - \033[1mINFO\033[0m%s%s\n", xdate, firstSep.c_str(),  buffer);
            fflush(stdout);
        }
        else if (logSeverity == LOG_X_WARN)
        {
            fprintf(stdout, "[%s] - \033[1;34mWARN\033[0m%s%s\n", xdate, firstSep.c_str(), buffer);
            fflush(stdout);
        }
        else if ((logSeverity == LOG_X_DEBUG || logSeverity == LOG_X_DEBUG1) && debug)
        {
            fprintf(stdout, "[%s] - \033[1;34mDEBUG\033[0m%s%s\n", xdate, firstSep.c_str(), buffer);
            fflush(stdout);
        }
        else if (logSeverity == LOG_X_CRITICAL)
        {
            fprintf(stdout, "[%s] - \033[1;31mCRIT\033[0m%s%s\n", xdate, firstSep.c_str(), buffer);
            fflush(stdout);
        }
        else if (logSeverity == LOG_X_ERR)
        {
            fprintf(stderr, "[%s] - \033[1;35mERR\033[0m%s%s\n", xdate,  firstSep.c_str(), buffer);
            fflush(stderr);
        }
    }
    if (IsSQLITELog() && ppDb)
    {
        unsigned int log_severity = (unsigned int) logSeverity;
        std::string severity = to_string(log_severity);

        ExecSQLITEQueryVA("INSERT INTO logs_v1 (date,severity,module,user,ip,message) VALUES(DateTime('now'),?,?,?,?,?);", 5, severity.c_str(), module.c_str(), user.c_str(), ip.c_str(), buffer);
    }
    va_end(args);
}

bool LoggerHive::CheckIfSQLITETableExist(const std::string &table)
{
    bool ret;
    string xsql = "select sql from sqlite_master where tbl_name=?;";
    sqlite3_stmt * stmt;
    sqlite3_prepare_v2(ppDb, xsql.c_str(), xsql.size() + 1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, table.c_str(), table.size(), 0);
    int s = sqlite3_step(stmt);
    ret = (s == SQLITE_ROW ? true : false);
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    return ret;
}

bool LoggerHive::IsSysLog()
{
    return (logMode & LOG_M_SYSLOG) == LOG_M_SYSLOG;
}

bool LoggerHive::IsSTDLog()
{
    return (logMode & LOG_M_STD_OUTPUT) == LOG_M_STD_OUTPUT;
}

bool LoggerHive::IsSQLITELog()
{
    return (logMode & LOG_M_SQLITE) == LOG_M_SQLITE;
}

bool LoggerHive::ExecSQLITEQuery(const std::string& query)
{
    const char *tail;
    sqlite3_stmt *stmt = 0;
    sqlite3_prepare_v2(ppDb, query.c_str(), query.length(), &stmt, &tail);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(ppDb));
        return false;
    }
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);

    return true;
}

bool LoggerHive::ExecSQLITEQueryVA(const std::string& query, int _va_size, ...)
{
    const char *tail;
    sqlite3_stmt *stmt = 0;
    sqlite3_prepare_v2(ppDb, query.c_str(), query.length(), &stmt, &tail);

    va_list args;
    va_start(args, _va_size);

    for (int i = 0; i < _va_size; i++)
    {
        const char *val_to_bind = va_arg(args, const char *);
        sqlite3_bind_text(stmt, i + 1, val_to_bind, strlen(val_to_bind) + 1, SQLITE_TRANSIENT);
    }

    va_end(args);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(ppDb));
        return false;
    }
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);

    return true;
}

bool LoggerHive::IsWindowsEventLog()
{
    return (logMode & LOG_M_WIN_EVENTLOG) == LOG_M_WIN_EVENTLOG;
}

void LoggerHive::DropLog()
{
    ExecSQLITEQuery("DELETE FROM logs_v1 WHERE 1=1;");
}

void LoggerHive::InitLog()
{
    if (IsSysLog())
    {
#ifndef _WIN32
        openlog( NULL, LOG_PID, LOG_LOCAL5);
#else
        fprintf(stderr,"SysLog Not implemented on WIN32, don't use.");
#endif
    }
    if (IsSTDLog())
    {
        // do nothing...
    }
    if (IsWindowsEventLog())
    {
        //TODO: future work.
    }
    if (IsSQLITELog())
    {
        if (access(appLogDir.c_str(), R_OK))
        {
#ifdef _WIN32
            if (mkdir(appLogDir.c_str()) == -1)
#else
            if (mkdir(appLogDir.c_str(), 0700) == -1)
#endif
            {
                fprintf(stderr, " [+] ERROR (@std_logger)> Unable to create log dir (%s)\n", appLogDir.c_str());
            }
        }
        if (!access(appLogDir.c_str(), W_OK))
        {
            int rc;
            rc = sqlite3_open(appLogFile.c_str(), &ppDb);
            if (rc)
            {
                fprintf(stderr, " [+] ERROR (@std_logger)> Unable to create/open log file (%s) - %s\n", appLogFile.c_str(), sqlite3_errmsg(ppDb));
            }
            else
            {
                if (!CheckIfSQLITETableExist("logs_v1"))
                {
                    ExecSQLITEQuery("CREATE TABLE \"logs_v1\" (\"id\" INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL , \"date\" DATETIME NOT NULL , \"severity\" INTEGER NOT NULL , \"module\" VARCHAR(256) NOT NULL, \"user\" VARCHAR(256), \"ip\" VARCHAR(46), \"message\" TEXT NOT NULL );");
                    ExecSQLITEQuery("CREATE UNIQUE INDEX \"idx_logs_v1_id\" ON \"logs_v1\" (\"id\" ASC);");
                    ExecSQLITEQuery("CREATE INDEX \"idx_logs_v1_date\" ON \"logs_v1\" (\"date\" ASC);");
                    ExecSQLITEQuery("CREATE INDEX \"idx_logs_v1_module\" ON \"logs_v1\" (\"module\" ASC);");
                    ExecSQLITEQuery("CREATE INDEX \"idx_logs_v1_user\" ON \"logs_v1\" (\"user\" ASC);");
                    ExecSQLITEQuery("CREATE INDEX \"idx_logs_v1_ip\" ON \"logs_v1\" (\"ip\" ASC);");
                    ExecSQLITEQuery("CREATE INDEX \"idx_logs_v1_severity\" ON \"logs_v1\" (\"severity\" ASC);");

                    // now we have the tables ;)
                }
            }
        }
    }
}

void LoggerHive::setDebug(bool value)
{
    Locker_Mutex rlock(&mt);
    debug = value;
}

unsigned int LoggerHive::GetLogLastID()
{
    Locker_Mutex rlock(&mt);

    unsigned int x = 0;
    string xsql = "SELECT id FROM logs_v1 ORDER BY id DESC LIMIT 1";

    sqlite3_stmt * stmt;
    sqlite3_prepare_v2(ppDb, xsql.c_str(), xsql.size() + 1, &stmt, NULL);

    int s = sqlite3_step(stmt);
    if (s == SQLITE_ROW)
    {
        x = sqlite3_column_int(stmt, 0);
    }
    else
    {
        fprintf(stderr, "Get IDS failed (using 0).\n");
        exit(1);
    }
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    return x;
}

std::list<LogElement> LoggerHive::GetLogView(unsigned int id_from, unsigned int id_to, const std::string& filter, LogLevel logLevelFilter)
{
    Locker_Mutex rlock(&mt);

    std::list<LogElement> r;
    string xsql;

    if (logLevelFilter == LOG_X_ALL)
        xsql = "SELECT id,date as TEXT,severity,module,user,ip,message AS TEXT FROM logs_v1 WHERE id >= '" + to_string(id_from) + "' and id <= '" + to_string(id_to) + "';";
    else
        xsql = "SELECT id,date as TEXT,severity,module,user,ip,message AS TEXT FROM logs_v1 WHERE id >= '" + to_string(id_from) + "' AND id <= '" + to_string(id_to) + "' and severity = '" + to_string((unsigned int) logLevelFilter) + "';";

    sqlite3_stmt * stmt;
    sqlite3_prepare_v2(ppDb, xsql.c_str(), xsql.size() + 1, &stmt, NULL);

    for (;;)
    {
        int s = sqlite3_step(stmt);
        if (s == SQLITE_ROW)
        {
            LogElement rx;
            rx.id = sqlite3_column_int(stmt, 0);
            rx.date = (char *)sqlite3_column_text(stmt, 1);
            rx.severity = sqlite3_column_int(stmt, 2);
            rx.module = (char *)sqlite3_column_text(stmt, 3);
            rx.user = (char *)sqlite3_column_text(stmt, 4);
            rx.ip = (char *)sqlite3_column_text(stmt, 5);
            rx.message = (char *)sqlite3_column_text(stmt, 6);

            r.push_back(rx);
        }
        else if (s == SQLITE_DONE)
        {
            break;
        }
        else
        {
            fprintf(stderr, "Get OIDS failed.\n");
            exit(1);
        }
    }
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);

    return r;
}

