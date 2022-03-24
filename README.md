# local_psql

Needed things
---
  * [Nix](https://nixos.org/nix/)
  * `path/to/local_psql/.env` with the following:

```
export PGUSER="USERNAME"  # $ whoami
export PGHOST="localhost"
export PGPORT="5432"
export PGDATABASE="postgres"
export PGPASSWORD=""
```

Quick start
---
```
$ nix-shell
[nix-shell:path/to/local_psql]$ ./server create
[nix-shell:path/to/local_psql]$ ./server start
[nix-shell:path/to/local_psql]$ ./server status
[nix-shell:path/to/local_psql]$ ./server list
[nix-shell:path/to/local_psql]$ ./server stop
```

Note
---
If `$ ./server start` yields the following error (found in `path/to/local_psql/logfile`)
```
FATAL:  could not create lock file "/run/postgresql/.s.PGSQL.5432.lock": Permission denied
LOG:  database system is shut down
```
run these two commands
```
$ sudo mkdir /var/run/postgresql
$ sudo chmod a+w /var/run/postgresql
```
---
To server on a LAN, adjust `path/to/local_psql/db/postgresql.conf` (see [https://www.postgresql.org/docs/current/runtime-config-connection.html#GUC-LISTEN-ADDRESSES]().)
```
listen_addresses = '*'
```
then you'll need add an entry to `path/to/local_psql/db/pg_hba.conf` (see [https://stackoverflow.com/a/22083740]())
```
# TYPE  DATABASE        USER            ADDRESS                 METHOD
host    all             all             192.168.1.0/24          trust
```
