#include <libpq-fe.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef uint32_t u32;
typedef int32_t  i32;
typedef float    f32;
typedef double   f64;

#define OK    0
#define ERROR 1

#define null nullptr

#define STATIC_ASSERT(condition) static_assert(condition, "!(" #condition ")")

#define EXIT(conn)                                   \
    {                                                \
        fflush(stdout);                              \
        fprintf(stderr, "%s", PQerrorMessage(conn)); \
        PQfinish(conn);                              \
        _exit(ERROR);                                \
    }

#define CHECK_CONN(conn)                   \
    if (PQstatus(conn) != CONNECTION_OK) { \
        EXIT(conn);                        \
    }

#define CHECK_RES(conn, res, expected)     \
    if (PQresultStatus(res) != expected) { \
        PQclear(res);                      \
        EXIT(conn);                        \
    }

#define CHECK_COMMAND(conn, command)            \
    {                                           \
        PGresult* res = PQexec(conn, command);  \
        CHECK_RES(conn, res, PGRES_COMMAND_OK); \
        PQclear(res);                           \
    }

union Value32 {
    u32  as_u32;
    i32  as_i32;
    f32  as_f32;
    char as_chars[4];
};

STATIC_ASSERT(sizeof(Value32) == sizeof(u32));

static Value32 read_value32(PGresult* res, i32 i, const char* column) {
    Value32 value;
    memcpy(&value.as_u32,
           PQgetvalue(res, i, PQfnumber(res, column)),
           sizeof(Value32));
    value.as_u32 = ntohl(value.as_u32);
    return value;
}

i32 main() {
    // NOTE: See `https://www.postgresql.org/docs/9.1/libpq-example.html`.
    PGconn* conn = PQconnectdb("");
    CHECK_CONN(conn);
    CHECK_COMMAND(conn,
                  "CREATE TABLE IF NOT EXISTS t "
                  "(id SERIAL PRIMARY KEY, x REAL NOT NULL);");
    CHECK_COMMAND(conn, "BEGIN;");
    {
        // NOTE: See `https://www.postgresql.org/docs/current/libpq-exec.html`.
        // NOTE: See `https://gist.github.com/ictlyh/12fe787ec265b33fd7e4b0bd08bc27cb`.
        Value32 x = {.as_f32 = 0.01f};
        Value32 y = {.as_f32 = -9.2f};
        x.as_u32 = htonl(x.as_u32);
        y.as_u32 = htonl(y.as_u32);
        const char* values[] = {
            reinterpret_cast<const char*>(&x),
            reinterpret_cast<const char*>(&y),
        };
        // NOTE: See `SELECT oid, typname FROM pg_type;`.
        Oid       types[] = {700, 700};
        i32       lengths[] = {sizeof(Value32), sizeof(Value32)};
        i32       formats[] = {1, 1};
        PGresult* res = PQexecParams(conn,
                                     "INSERT INTO t (x) VALUES ($1), ($2);",
                                     2,
                                     types,
                                     values,
                                     lengths,
                                     formats,
                                     0);
        CHECK_RES(conn, res, PGRES_COMMAND_OK);
        PQclear(res);
    }
    {
        PGresult* res = PQexecParams(conn,
                                     "SELECT id, x FROM t;",
                                     0,
                                     null,
                                     null,
                                     null,
                                     null,
                                     1);
        CHECK_RES(conn, res, PGRES_TUPLES_OK);
        printf("%5s | %5s\n", "id", "x");
        printf("------+------\n");
        for (i32 i = 0; i < PQntuples(res); ++i) {
            Value32 id = read_value32(res, i, "id");
            Value32 x = read_value32(res, i, "x");
            printf("%5d | %5.2f\n", id.as_i32, static_cast<f64>(x.as_f32));
        }
        PQclear(res);
    }
    CHECK_COMMAND(conn, "ROLLBACK;");
    CHECK_COMMAND(conn, "DROP TABLE IF EXISTS t;");
    PQfinish(conn);
    return OK;
}
