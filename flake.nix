{
  description = "ncurses app with bundled munit";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = {
    self,
    nixpkgs,
  }: let
    pkgs = import nixpkgs {system = "x86_64-linux";};
  in {
    formatter.x86_64-linux = nixpkgs.legacyPackages.x86_64-linux.alejandra;
    devShells.x86_64-linux.default = pkgs.mkShell {
      buildInputs = with pkgs; [gcc ncurses json_c clang-tools];
    };

    packages.x86_64-linux.default = pkgs.stdenv.mkDerivation rec {
      pname = "daisy-chain";
      version = "0.1.0";

      src = ./.; # whole project directory
      dontCopySrc = true;

      buildInputs = [pkgs.ncurses pkgs.json_c];

      # TODO: write a makefile bcs this will get tedious
      extraCFlags = "-I${src}/includes";

      buildPhase = ''
        $CC -Wall -O2 -c src/main.c
        $CC -o daisy-chain main.o -lncurses -ljson-c
      '';

      doCheck = true;
      checkPhase = ''
        # compile munit.c together with the test source
        $CC -Wall -O2 -c ${src}/includes/munit/munit.c ${extraCFlags}
        $CC -Wall -O2 -c src/test.c

        $CC -o test-daisy-chain munit.o test.o -lncurses -ljson-c
        ./test-daisy-chain
      '';

      installPhase = ''
        mkdir -p $out/bin
        cp daisy-chain $out/bin/
      '';
    };
  };
}
