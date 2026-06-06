#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sqlite3.h>

#define DB_PATH "/tmp/crash_monitor.db"
#define LOG_PATH "/tmp/crash_debug.log"

void init_database() {
    sqlite3 *db;
    char *err_msg = 0;
    
    int rc = sqlite3_open(DB_PATH, &db);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        exit(1);
    }
    
    char *sql = "CREATE TABLE IF NOT EXISTS crashes ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "timestamp TEXT, "
                "pid INTEGER, "
                "uid INTEGER, "
                "signal INTEGER, "
                "exe_name TEXT);";
                
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        sqlite3_free(err_msg);
    }
    
    sqlite3_close(db);
}

int main(int argc, char *argv[]) {
    FILE *log = fopen(LOG_PATH, "a");
    if (log) {
        fprintf(log, "Daemon triggered! Args count: %d\n", argc);
        for (int i = 0; i < argc; i++) {
            fprintf(log, "  arg[%d]: %s\n", i, argv[i]);
        }
        fclose(log);
    }

    if (argc < 5) {
        return 1;
    }

    init_database();

    int pid = atoi(argv[1]);
    int uid = atoi(argv[2]);
    int sig = atoi(argv[3]);
    char *exe_name = argv[4];

    time_t rawtime;
    struct tm *timeinfo;
    char time_buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    sqlite3 *db;
    sqlite3_stmt *res;
    
    if (sqlite3_open(DB_PATH, &db) == SQLITE_OK) {
        char *sql = "INSERT INTO crashes (timestamp, pid, uid, signal, exe_name) VALUES (?, ?, ?, ?, ?);";
        
        if (sqlite3_prepare_v2(db, sql, -1, &res, 0) == SQLITE_OK) {
            sqlite3_bind_text(res, 1, time_buffer, -1, SQLITE_STATIC);
            sqlite3_bind_int(res, 2, pid);
            sqlite3_bind_int(res, 3, uid);
            sqlite3_bind_int(res, 4, sig);
            sqlite3_bind_text(res, 5, exe_name, -1, SQLITE_STATIC);
            
            sqlite3_step(res);
        }
        sqlite3_finalize(res);
        sqlite3_close(db);
    }

    return 0;
}
