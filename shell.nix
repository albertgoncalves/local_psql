with import <nixpkgs> {};
mkShell {
    buildInputs = [
        (python3.withPackages(ps: with ps; [
            flake8
            pandas
            psycopg2
            sqlalchemy
        ]))
        glibcLocales
        postgresql_14
        shellcheck
    ];
    shellHook = ''
        . .env
        . .shellhook
    '';
}
