# local_psql

Needed things
---
  * [Nix](https://nixos.org/nix/)
  * `path/to/local_psql/.env` with the following:

```
export PGUSER="USERNAME"
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
