{ pkgs ? import <nixpkgs> {} }:
with pkgs; mkShell {
    name = "Psql";
    buildInputs = [
        (python37.withPackages(ps: with ps; [
            flake8
            pandas
            psycopg2
            sqlalchemy
        ]))
        postgresql
        shellcheck
    ];
    shellHook = ''
        . .env
        . .shellhook
    '';
}
