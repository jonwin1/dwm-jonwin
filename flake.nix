{
  description = "DWM flake by jonwin";

  inputs = {
    nixpkgs.url = "nixpkgs/nixos-unstable";
    utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, utils, ... }@inputs:
    utils.lib.eachSystem ["x86_64-linux"] (system: let
      pkgs = import nixpkgs {
        inherit system;

        overlays = [
          (final: prev: {
            dwm = prev.dwm.overrideAttrs (old: {
              src = ./.;
              buildInputs = with prev; old.buildInputs ++ [xorg.libXext];
            });
          })
        ];
      };
    in {
      packages.default = pkgs.dwm;

      devShells.default = pkgs.mkShell {
        name = "dwm";
        packages = with pkgs; [
        ];
      };
    });
}
