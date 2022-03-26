with import <nixpkgs> {};
mkShell.override { stdenv = llvmPackages_14.stdenv; } {
    buildInputs = [
        glibcLocales
        postgresql_14
        shellcheck
    ];
    shellHook = ''
        export PATH_LIBPG=${postgresql_14.lib}
        export PATH_PG=${postgresql_14}
        . .env
        . .shellhook
    '';
    # NOTE: See `https://nixos.wiki/wiki/C`.
    # NOTE: See `https://nixos.org/manual/nixpkgs/stable/#sec-hardening-in-nixpkgs`.
    hardeningDisable = [ "all" ];
}
