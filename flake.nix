{
  description = "a little cli todo thingy";

  inputs = {
    self.submodules = true;
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
      buildInputs = with pkgs; [gcc json_c clang-tools valgrind];
    };

    packages.x86_64-linux.default = pkgs.stdenv.mkDerivation rec {
      pname = "daisy-chain";
      version = "0.1.0";

      src = ./.; # whole project directory
      buildInputs = [pkgs.json_c];

      # NOTE: write a makefile bcs this will get tedious (it actually did not get tedious)
      extraCFlags = "-I${src}/includes -I${src}";

      buildPhase = ''
        $CC -Wall -O2 -c src/main_cli.c
        $CC -Wall -O2 -c src/task.c
        $CC -Wall -O2 -c src/chain.c
        $CC -Wall -O2 -c src/files.c
        $CC -Wall -O2 -c src/cmd_parser.c
        $CC -o daisy-chain task.o chain.o files.o cmd_parser.o main_cli.o -ljson-c
      '';

      doCheck = true;
      checkPhase = ''
        # compile munit.c together with the test source
        $CC -Wall -O2 -c ${src}/includes/munit/munit.c ${extraCFlags}
        $CC -Wall -O2 -c src/test.c ${extraCFlags}
        $CC -Wall -O2 -c src/task.c ${extraCFlags}
        $CC -Wall -O2 -c src/chain.c ${extraCFlags}
        $CC -Wall -O2 -c src/files.c ${extraCFlags}
        $CC -Wall -O2 -c src/cmd_parser.c ${extraCFlags}

        $CC -o test-daisy-chain munit.o task.o chain.o files.o cmd_parser.o test.o -ljson-c
      '';

      installPhase = ''
        mkdir -p $out/bin
        cp daisy-chain $out/bin/
        cp test-daisy-chain $out/bin/
      '';
    };
  };
}
