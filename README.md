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
If `$ ./server start` yields the following error (found in check `path/to/local_psql/logfile`):
```
FATAL:  could not create lock file "/run/postgresql/.s.PGSQL.5432.lock": Permission denied
LOG:  database system is shut down
```
```
$ sudo mkdir /var/run/postgresql
$ sudo chmod a+w /var/run/postgresql
```
