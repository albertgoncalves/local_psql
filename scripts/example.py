#!/usr/bin/env python3

from os import environ

from pandas import read_sql
from sqlalchemy import create_engine


def url(kwargs):
    return "postgresql://{user}:{password}@{host}:{port}/{db}".format(**kwargs)


def connect(kwargs):
    return create_engine(url(kwargs)).connect()


def ops(con):
    sql = "SELECT * FROM t1;"
    print(read_sql(sql, con))


def main():
    creds = {
        ("user", "PGUSER"),
        ("host", "PGHOST"),
        ("port", "PGPORT"),
        ("db", "PGDB"),
        ("password", "PGPASS"),
    }
    con = connect({k: environ[v] for (k, v) in creds})
    try:
        ops(con)
    finally:
        con.close()


if __name__ == "__main__":
    main()
