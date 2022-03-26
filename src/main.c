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

#define OID_F32 700

#define TEXT   0
#define BINARY 1

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

static u32 read_u32(PGresult* res, i32 i, const char* column) {
    u32 value;
    memcpy(&value, PQgetvalue(res, i, PQfnumber(res, column)), sizeof(u32));
    return ntohl(value);
}

static i32 read_i32(PGresult* res, i32 i, const char* column) {
    return (i32)read_u32(res, i, column);
}

static f32 read_f32(PGresult* res, i32 i, const char* column) {
    u32 value = read_u32(res, i, column);
    return *(f32*)&value;
}

static u32 htonf(f32 x) {
    u32 bytes = *(u32*)&x;
    return htonl(bytes);
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
        u32 x = htonf(0.01f);
        u32 y = htonf(-1.23f);
        // NOTE: See `SELECT oid, typname FROM pg_type;`.
        Oid         types[] = {OID_F32, OID_F32};
        const char* values[] = {(const char*)(&x), (const char*)(&y)};
        i32         lengths[] = {sizeof(f32), sizeof(f32)};
        i32         formats[] = {BINARY, BINARY};
        PGresult*   res = PQexecParams(conn,
                                     "INSERT INTO t (x) VALUES ($1), ($2);",
                                     2,
                                     types,
                                     values,
                                     lengths,
                                     formats,
                                     TEXT);
        CHECK_RES(conn, res, PGRES_COMMAND_OK);
        PQclear(res);
    }
    {
        PGresult* res = PQexecParams(conn,
                                     "SELECT id, x FROM t;",
                                     0,
                                     NULL,
                                     NULL,
                                     NULL,
                                     NULL,
                                     BINARY);
        CHECK_RES(conn, res, PGRES_TUPLES_OK);
        printf("%5s | %5s\n", "id", "x");
        printf("------+------\n");
        for (i32 i = 0; i < PQntuples(res); ++i) {
            i32 id = read_i32(res, i, "id");
            f32 x = read_f32(res, i, "x");
            printf("%5d | %5.2f\n", id, (f64)x);
        }
        PQclear(res);
    }
    CHECK_COMMAND(conn, "ROLLBACK;");
    CHECK_COMMAND(conn, "DROP TABLE IF EXISTS t;");
    PQfinish(conn);
    return OK;
}
