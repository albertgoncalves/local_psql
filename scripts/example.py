#!/usr/bin/env python3

from os import environ

from pandas import read_sql
from sqlalchemy import create_engine


def url(kwargs):
    return "postgresql://{user}:{password}@{host}:{port}/{db}".format(**kwargs)


def connect(kwargs):
    return create_engine(url(kwargs)).connect()


def try_con(credentials):
    try:
        (con, error) = \
            (connect({k: environ[v] for (k, v) in credentials}), None)
        print("ESTABLISHED CONNECTION")
    except Exception as e:
        (con, error) = (None, e)
    return (con, error)


def try_effects(f, con):
    try:
        f(con)
        error = None
    except Exception as e:
        error = e
    finally:
        con.close()
        print("\nCLOSED CONNECTION")
        return error


def pipeline(f, credentials):
    (con, error) = try_con(credentials)
    if not error:
        error = try_effects(f, con)
    if error:
        print("\n{}".format(error))


def main():
    def effects(con):
        sql = "SELECT * FROM t1;"
        data = read_sql(sql, con)
        print("COMPLETED QUERY\n")
        print(data)

    credentials = {
        ("user", "PGUSER"),
        ("host", "PGHOST"),
        ("port", "PGPORT"),
        ("db", "PGDB"),
        ("password", "PGPASS"),
    }
    pipeline(effects, credentials)


if __name__ == "__main__":
    main()
