with import <nixpkgs> {};
mkShell.override { stdenv = llvmPackages_14.stdenv; } {
    buildInputs = [
        glibcLocales
        mold
        postgresql_14
        shellcheck
    ];
    shellHook = ''
        export PATH_PG=${postgresql_14}
        export PATH_LIBPG=${postgresql_14.lib}
        . .env
        . .shellhook
    '';
}
