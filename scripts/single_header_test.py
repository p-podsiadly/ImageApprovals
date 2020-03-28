from single_header import preprocess_file, SingleHeaderGen, SourceFileAccessor, Scope
import unittest
from pathlib import Path
import re
import os

class TestPreprocessFile(unittest.TestCase):

    def test_removing_include_guards(self):

        in_src = (
            "#ifndef INCLUDE_GUARD\n"
            "#define INCLUDE_GUARD\n"
            "\n"
            "void someFunction();\n"
            "\n"
            "#endif // INCLUDE_GUARD\n"
        )

        expected_out_src = (
            "// Test.hpp\n"
            "\n"
            "void someFunction();\n"
        )

        out_src, rel_includes = preprocess_file("Test.hpp", in_src, [])

        self.assertEqual(out_src, expected_out_src)
        self.assertEqual(rel_includes, [])

    def test_removing_rel_includes(self):

        in_src = (
            "#include \"Header1.hpp\"\n"
            "#include <cstdint>\n"
            "\n"
            "void a();\n"
            "#include \"Header2.hpp\""
        )

        expected_out_src = (
            "// Test.hpp\n"
            "\n"
            "#include <cstdint>\n"
            "\n"
            "void a();\n"
        )

        out_src, rel_includes = preprocess_file("Test.hpp", in_src, [])

        self.assertEqual(out_src, expected_out_src)
        self.assertEqual(rel_includes, [Path("Header1.hpp"), Path("Header2.hpp")])

    def test_ignored_abs_includes(self):

        in_src = (
            "#include <ImageApprovals/ColorSpace.hpp>\n"
            "#include <cstdint>\n"
            "void f();\n"
        )

        out_src, rel_includes = preprocess_file("Test.hpp", in_src, ["ImageApprovals/"])

        expected_out_src = (
            "// Test.hpp\n"
            "\n"
            "#include <cstdint>\n"
            "void f();\n"
        )

        self.assertEqual(out_src, expected_out_src)
        self.assertEqual(rel_includes, [])

class TestSingleHeaderGen(unittest.TestCase):

    in_header_1 = {
        "file_name": "Test1.hpp",
        "src": (
            "// Test1.hpp\n"
            "\n"
            "void f1();\n"
        ),
        "rel_includes": []
    }

    in_header_2 = {
        "file_name": "Test2.hpp",
        "src": (
            "// Test2.hpp\n"
            "\n"
            "void f2();\n"
        ),
        "rel_includes": ["Test1.hpp"]
    }

    def test_one_header(self):

        gen = SingleHeaderGen("SINGLE_HPP", "Single_IMPL")

        gen.add_public_src(**self.in_header_1)

        result = gen.generate()

        expected_result = (
            "#ifndef SINGLE_HPP\n"
            "#define SINGLE_HPP\n"
            "\n"
            "// Test1.hpp\n"
            "\n"
            "void f1();\n"
            "\n"
            "#ifdef Single_IMPL\n"
            "\n"
            "#endif // Single_IMPL\n"
            "\n"
            "#endif // SINGLE_HPP\n"
        )

        self.assertEqual(result, expected_result)

    def test_two_headers(self):
        
        gen = SingleHeaderGen("SINGLE_HPP", "Single_IMPL")

        gen.add_public_src(**self.in_header_2)
        gen.add_public_src(**self.in_header_1)

        result = gen.generate()

        expected_result = (
            "#ifndef SINGLE_HPP\n"
            "#define SINGLE_HPP\n"
            "\n"
            "// Test1.hpp\n"
            "\n"
            "void f1();\n"
            "\n"
            "// Test2.hpp\n"
            "\n"
            "void f2();\n"
            "\n"
            "#ifdef Single_IMPL\n"
            "\n"
            "#endif // Single_IMPL\n"
            "\n"
            "#endif // SINGLE_HPP\n"
        )

        self.assertEqual(result, expected_result)
    
    def test_additional_header(self):

        gen = SingleHeaderGen("SINGLE_HPP", "Single_IMPL")

        gen.additional_header = (
            "/*\n"
            " * This is a comment.\n"
            " */\n"
        )

        gen.add_public_src(**self.in_header_1)

        result = gen.generate()

        expected_result = (
            "#ifndef SINGLE_HPP\n"
            "#define SINGLE_HPP\n"
            "\n"
            "/*\n"
            " * This is a comment.\n"
            " */\n"
            "\n"
            "\n"
            "// Test1.hpp\n"
            "\n"
            "void f1();\n"
            "\n"
            "#ifdef Single_IMPL\n"
            "\n"
            "#endif // Single_IMPL\n"
            "\n"
            "#endif // SINGLE_HPP\n"
        )

        self.assertEqual(result, expected_result)

    def test_add_source_files(self):

        class TestSrcFileAccessor(SourceFileAccessor):

            def __init__(self):
                self.test_files = {
                    ("", "Root.hpp", Scope.PUBLIC): (
                        "#ifndef ROOT_HPP\n"
                        "#define ROOT_HPP\n"
                        "#include \"lib/Header1.hpp\"\n"
                        "#include \"lib/Header2.hpp\"\n"
                        "#endif // ROOT_HPP"
                    ),
                    ("lib", "Header1.hpp", Scope.PUBLIC): (
                        "#ifndef HEADER1_HPP\n"
                        "#define HEADER1_HPP\n"
                        "#include \"Header2.hpp\"\n"
                        "void f1();\n"
                        "#endif"
                    ),
                    ("lib", "Header2.hpp", Scope.PUBLIC): (
                        "#ifndef HEADER2_HPP\n"
                        "#define HEADER2_HPP\n"
                        "void f2();\n"
                        "#endif"
                    ),
                    ("lib", "Source1.cpp", Scope.PRIVATE): (
                        "#include \"Header1.hpp\"\n"
                        "#include \"Header2.hpp\"\n"
                        "void f1() {}\n"
                        "void f2() {}\n"
                    )
                }

            def list_files(self):
                return list(self.test_files.keys())

            def read_file(self, path_dir, file_name):
                for key, val in self.test_files.items():
                    if key[0:2] == (path_dir, file_name):
                        return val
                return None

        gen = SingleHeaderGen("LIB_HPP", "LIB_IMPL")

        gen.add_source_files(TestSrcFileAccessor())

        out_src = gen.generate()
        
        expected_out_src = (
            "#ifndef LIB_HPP\n"
            "#define LIB_HPP\n"
            "\n"
            "// lib/Header2.hpp\n"
            "\n"
            "void f2();\n"
            "\n"
            "// lib/Header1.hpp\n"
            "\n"
            "void f1();\n"
            "\n"
            "// Root.hpp\n"
            "\n"
            "#ifdef LIB_IMPL\n"
            "\n"
            "// lib/Source1.cpp\n"
            "\n"
            "void f1() {}\n"
            "void f2() {}\n"
            "\n"
            "#endif // LIB_IMPL\n"
            "\n"
            "#endif // LIB_HPP\n"
        )

        self.assertEqual(out_src, expected_out_src)
