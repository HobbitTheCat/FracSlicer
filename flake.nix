{
  description = "My custom C++ template for Devenv";

  outputs = {self }: {
    templates.default = {
      path = ./.;
      description = "Default C++ template";
    };
  };
}
