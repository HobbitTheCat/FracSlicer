{ pkgs, lib, config, inputs, ... }:

let
  gpuLibs = with pkgs; [
    cudaPackages.cudatoolkit
    linuxPackages.nvidia_x11
    libGL
    libGLU
    freeglut
    glew
    glfw
    stdenv.cc.cc.lib

    openblas
  ];
in {
  packages = with pkgs; [
    nixd
    gcc
    gnumake
    cmake
    gdb
    glm
    tbb

    pkg-config
    llvmPackages.clang-tools
    compiledb

    cudaPackages.cudatoolkit
    glfw
    glew
    libGL
    freeglut
    libGLU

    xorg.libX11
    xorg.libXrandr
    xorg.libXinerama
    xorg.libXcursor
    xorg.libXi

    fmt
    spdlog
    armadillo

    openblas
    lapack

    lua5_4
  ];


  env = {
    CMAKE_PREFIX_PATH = lib.makeSearchPath "lib/cmake" config.packages;
    PKG_CONFIG_PATH = lib.makeSearchPath "lib/pkgconfig" config.packages;

    CUDA_PATH = "${pkgs.cudaPackages.cudatoolkit}";
    LD_LIBRARY_PATH = lib.makeLibraryPath gpuLibs;
    # LD_LIBRARY_PATH = lib.makeLibraryPath config.packages;
    CPATH = lib.makeSearchPath "include" config.packages;
    PKG_CONFIG_PATH_FOR_CMAKE = lib.makeSearchPath "share/pkgconfig" config.packages;
  };
}
