from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.files import copy, get
from conan.tools.build import check_min_cppstd
from conan.tools.scm import Version
import os

class tcpcatRecipe(ConanFile):
    name = "tcpcat"
    version = "1.0.1"
    package_type = "library"

    # Optional metadata
    license = "MIT"
    author = "ydemir9191@gmail.com"
    homepage = "https://github.com/ydcpp/tcpcat"
    description = "Simple C++ TCP Server and Client library."
    topics = ("network", "tcp", "tcp server", "tcp client")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*", "include/*"

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def validate(self):
        if self.settings.compiler.get_safe("cppstd"):
            check_min_cppstd(self, 17)

    def requirements(self):
        self.requires("asio/[^1.30]", transitive_headers = True)

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        copy(self, "*", src=os.path.join(self.source_folder, "include"), dst=os.path.join(self.package_folder, "include"))
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["tcpcat"]
        self.cpp_info.set_property("cmake_target_name", "tcpcat")

