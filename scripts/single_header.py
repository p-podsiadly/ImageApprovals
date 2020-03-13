import re
import toposort
import os
from abc import ABC, abstractmethod
from pathlib import Path

def preprocess_file(file_name, src, ignored_abs_includes):

    out_src = ""
    rel_includes = []

    src_lines = src.strip().split("\n")

    if len(src_lines) < 3:
        return ("", [])

    ifndef_re = re.compile("#ifndef ([a-zA-Z0-9_]*)")
    match = ifndef_re.match(src)
    if match is not None:
        
        inc_guard = match.group(1)

        line_2nd = src_lines[1].strip()
        line_last = src_lines[-1].strip()

        if line_2nd == "#define " + inc_guard and line_last.startswith("#endif"):
            del src_lines[0:2]
            del src_lines[-1]

    rel_include_re = re.compile("#include \"([a-zA-Z0-9_.\\/]*)\"")

    def rel_include_filter(src_line):
        match = rel_include_re.match(src_line)
        if match is not None:
            rel_includes.append(Path(match.group(1)))
            return False
        return True
    
    src_lines = filter(rel_include_filter, src_lines)
    
    if len(ignored_abs_includes) > 0:

        ignored_abs_includes = [re.compile(inc) for inc in ignored_abs_includes]

        abs_include_re = re.compile("#include <([a-zA-Z0-9_.\\/]*)>")
        
        def abs_include_filter(src_line):
            match = abs_include_re.match(src_line)
            if match is not None:
                inc_file = match.group(1)

                for inc_re in ignored_abs_includes:
                    if inc_re.match(inc_file) is not None:
                        return False
            
            return True
        
        src_lines = filter(abs_include_filter, src_lines)
        
    out_src = "\n".join(src_lines).strip()
    if len(out_src) == 0:
        out_src = "// {}\n".format(Path(file_name).as_posix())
    else:
        out_src = "// {}\n\n{}\n".format(Path(file_name).as_posix(), out_src)

    return (out_src, rel_includes)

# "Scope" of a source file (see SourceFileAccessor.list_files)
class Scope:

    # The file is a part of the public API
    PUBLIC = 0

    # The file is a part of the implementation
    PRIVATE = 1

class SourceFileAccessor(ABC):

    # Returns list of (dir_path, file_name, scope) tuples.
    # scope is either Scope.PUBLIC or Scope.PRIVATE.
    @abstractmethod
    def list_files(self):
        pass

    # Returns contents of the file
    @abstractmethod
    def read_file(self, dir_path, file_name):
        pass

class SingleHeaderGen:

    def __init__(self, inc_guard, impl_define):
        self._include_guard = inc_guard
        self._impl_define = impl_define
        self._public_sources = dict()
        self._private_sources = dict()

    def add_public_src(self, file_name, src, rel_includes):
        self._public_sources[Path(file_name)] = (src, rel_includes)

    def add_private_src(self, file_name, src, rel_includes):
        self._private_sources[Path(file_name)] = (src, list(filter(lambda inc : inc not in self._public_sources, rel_includes)))

    @staticmethod
    def _generate_part(src_dict):

        src_deps = dict()
        for key, val in src_dict.items():
            src_deps[str(key)] = set([str(p) for p in val[1]])
        
        ordered_sources = toposort.toposort_flatten(src_deps)

        out_src = ""

        for src_file_name in ordered_sources:
            src = src_dict[Path(src_file_name)][0]

            out_src = out_src + src + "\n"

        return out_src

    def generate(self):
        
        out_src = (
            "#ifndef " + self._include_guard + "\n"
            "#define " + self._include_guard + "\n\n"
        )

        out_src = out_src + SingleHeaderGen._generate_part(self._public_sources)

        out_src = out_src + "#ifdef " + self._impl_define + "\n\n"

        out_src = out_src + SingleHeaderGen._generate_part(self._private_sources)

        out_src = out_src + "#endif // " + self._impl_define + "\n\n"
        out_src = out_src + "#endif // " + self._include_guard + "\n"

        return out_src

    def add_source_files(self, src_file_accessor, ignored_abs_includes = []):

        src_files = src_file_accessor.list_files()

        for dir_path, file_name, scope in src_files:

            file_path = os.path.join(dir_path, file_name)

            in_src = src_file_accessor.read_file(dir_path, file_name)

            src, rel_includes = preprocess_file(file_path, in_src, ignored_abs_includes)

            rel_includes = [os.path.join(dir_path, rel_inc) for rel_inc in rel_includes]

            if scope == Scope.PUBLIC:
                self.add_public_src(file_path, src, rel_includes)
            else:
                self.add_private_src(file_path, src, rel_includes)

def is_cpp_file(file_name):
    _, ext = os.path.splitext(file_name)

    return ext in {".hpp", ".cpp"}

class SourceFileAccessorImpl(SourceFileAccessor):

    def __init__(self, root_dir):
        self._root_dir = root_dir

    def _list_cpp_files_from_dir(self, subdir_path, scope):

        result = []

        for full_dir_path, _, files in os.walk(os.path.join(self._root_dir, subdir_path)):

            dir_path = os.path.relpath(full_dir_path, self._root_dir)

            if dir_path == ".":
                dir_path = ""

            for file_name in filter(is_cpp_file, files):
                result.append((dir_path, file_name, scope))

        return result

    def list_files(self):

        inc_dir = os.path.join(self._root_dir, "include")
        src_dir = os.path.join(self._root_dir, "src")

        result = []

        result.extend(self._list_cpp_files_from_dir(inc_dir, Scope.PUBLIC))
        result.extend(self._list_cpp_files_from_dir(src_dir, Scope.PRIVATE))
                
        return result

    def read_file(self, dir_path, file_name):
        full_path = os.path.join(self._root_dir, dir_path, file_name)

        with open(full_path, "r") as f:
            return f.read()

if __name__ == "__main__":

    scripts_dir = Path(os.path.realpath(__file__)).parent
    lib_dir = scripts_dir / "../ImageApprovals/"
    single_header_path = scripts_dir / "../single_header/ImageApprovals.hpp"

    gen = SingleHeaderGen("IMAGEAPPROVALS_HPP_INCLUDED", "ImageApprovals_IMPLEMENT")
    gen.add_source_files(SourceFileAccessorImpl(lib_dir), ["ImageApprovals/"])
    single_header_src = gen.generate()

    print("Writing to {}...".format(single_header_path))
    with open(single_header_path, "w", newline="\n") as f:
        f.write(single_header_src)
