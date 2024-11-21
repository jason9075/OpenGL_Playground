{
  description = "flake.nix";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-24.05";
    nixgl.url = "github:nix-community/nixGL";
  };

  outputs = { nixpkgs, nixgl, ... }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs {
        inherit system;
        overlays = [ nixgl.overlay ];
      };
    in {
      devShells.x86_64-linux.default = pkgs.mkShell {
        nativeBuildInputs = with pkgs; [
          gcc
          cmake
          SDL2
          glew
          glm
          nlohmann_json
          entr
        ];

        shellHook = ''
          export LD_LIBRARY_PATH="/run/opengl-driver/lib:/run/opengl-driver-32/lib:$LD_LIBRARY_PATH"
          echo "Nix env activated."
        '';
      };
    };
}
