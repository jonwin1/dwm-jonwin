{
  description = "DWM flake by jonwin";

  inputs = {
    nixpkgs.url = "nixpkgs/nixos-unstable";
    utils.url = "github:numtide/flake-utils";
  };

  outputs = { nixpkgs, utils, ... }@inputs:
    utils.lib.eachDefaultSystem (system: let
      pkgs = import nixpkgs {
        inherit system;

        overlays = [
          (final: prev: {
            dwm = prev.dwm.overrideAttrs (old: {
              src = ./.;
              buildInputs = with prev; old.buildInputs ++ [
                xorg.libXext
              ];
            });
          })
        ];
      };
    in {
      packages.default = pkgs.dwm;
    });
}
