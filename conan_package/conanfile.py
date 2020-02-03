from conans import ConanFile, CMake, tools
import os

class ImageApprovalsConan(ConanFile):
    name = "imageapprovals"
    version = "master"
    description = "Image support for ApprovalTests.cpp library"
    homepage = "https://github.com/p-podsiadly/ImageApprovals"
    url = "https://github.com/p-podsiadly/ImageApprovals.git"
    license = "Apache-2.0"
    topics = ("testing", "unit-testing", "approval-tests", "graphics", "image")

    settings = "os", "compiler", "build_type", "arch"

    requires = [
        "approvaltests.cpp/8.1.0",
        "libpng/1.6.37",
        "openexr/2.4.0",
        "zlib/1.2.11"
    ]

    generators = ["cmake_find_package"]

    _source_subfolder = "source_subfolder"

    def source(self):
        tools.get(**self.conan_data["sources"][self.version])
        os.rename("ImageApprovals-{}".format(self.version), self._source_subfolder)
    
    @property
    def _configured_cmake(self):
        cmake = CMake(self)

        cmake.definitions["ImageApprovals_ENABLE_TEST"] = False

        cmake.configure(source_folder=self._source_subfolder)

        return cmake

    def build(self):
        self._configured_cmake.build()

    def package(self):
        self._configured_cmake.install()
    
    def package_info(self):
        self.cpp_info.names["cmake_find_package"] = "ImageApprovals"
        self.cpp_info.names["cmake_find_package_multi"] = "ImageApprovals"

        self.cpp_info.libs = ["ImageApprovals"]
