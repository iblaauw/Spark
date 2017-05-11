#!/usr/bin/python3

import os
import sys
import subprocess

PIPE = subprocess.PIPE

def modify_time(path):
    if path is None:
        return -1
    if not os.path.isfile(path):
        return -1
    return os.path.getmtime(path)

build_dir = 'build/'

def replace_build_dir(fname):
    current_dir = sys.path[0]
    current_dir = os.path.abspath(current_dir)

    prefix = os.path.commonprefix([current_dir, fname])
    postfix = fname[len(prefix):]

    if postfix[0] == '/':
        postfix = postfix[1:]

    new_path = os.path.join(build_dir, postfix)
    return os.path.abspath(new_path)

def ensure_dir_exists(dir_path):
    if not os.path.exists(dir_path):
        os.makedirs(dir_path)
    else:
        if os.path.isfile(dir_path):
            raise Exception("Error: the build directory '{}' is a file".format(dir_path))

def ensure_build_dir_exists():
    ensure_dir_exists(build_dir)

def ensure_dirs_for_files(fnames):
    dirs = { os.path.dirname(f) for f in fnames }
    for d in dirs:
        ensure_dir_exists(d)

class Rule(object):
    def __init__(self, name):
        self.name = name

    def needRun(self, depends, targets):
        if targets is None or depends is None:
            return True

        if len(targets) == 0 or len(depends) == 0:
            return True

        depend_time = max(modify_time(d) for d in depends)
        target_time = min(modify_time(t) for t in targets)
        return depend_time >= target_time

    def run(self, fnames):
        all_files = []
        for fname in fnames:
            if self.is_valid(fname):
                all_files.append(fname)
            else:
                print("Warning: rule '{}' could not be applied to file '{}', ignoring it".format(self.name, fname))

        depends = self.generate_depends(all_files)
        targets = self.generate_targets(all_files)

        if not self.needRun(depends, targets):
            return targets

        toExec = self.generate(depends, targets)

        for ex in toExec:
            print(' '.join(ex))
            ret = subprocess.call(ex)
            if ret != 0:
                raise Exception("An error occured while executing rule '{}'".format(self.name))

        return targets

    # Default dependencies, can be overwritten in subclass
    def generate_depends(self, fnames):
        return fnames

    def clean(self, fnames, clean_list):
        all_files = []
        for fname in fnames:
            if self.is_valid(fname):
                all_files.append(fname)
            else:
                print("Warning: rule '{}' could not be applied to file '{}', ignoring it".format(self.name, fname))

        targets = self.generate_targets(all_files)
        clean_list.extend(targets)
        return targets

class ExtChangeRule(Rule):
    def __init__(self, name, ext1, ext2):
        self._ext1 = ext1
        self._ext2 = ext2
        super().__init__(name)

    def is_valid(self, fname):
        (_, ext) = os.path.splitext(fname)
        return ext == self._ext1

    def generate_targets(self, fnames):
        result = []

        fnames = [replace_build_dir(f) for f in fnames]
        ensure_dirs_for_files(fnames)

        for fname in fnames:
            (root, _) = os.path.splitext(fname)
            result.append(root+self._ext2)

        return result

class cpp2oRule(ExtChangeRule):
    def __init__(self):
        super().__init__('.cpp->.o', '.cpp', '.o')
        self.include_dirs = []
        self.flags = []

    def generate(self, depends, targets):
        dep = depends[0]
        targ = targets[0]

        to_exec = [ 'g++' ]
        to_exec = to_exec + self.flags
        for idir in self.include_dirs:
            to_exec = to_exec + [ '-I', idir ]
        to_exec = to_exec + [ '-c', dep, '-o', targ ]
        return [ to_exec ]

class o2exeRule(Rule):
    def __init__(self):
        super().__init__('.o->exe')
        self.executable_name = 'a.out'
        self.lib_dirs = []
        self.libraries = []
        self.flags = []
        self.post_flags = []

    def is_valid(self, fname):
        (_, ext) = os.path.splitext(fname)
        return ext == '.o'

    def generate_targets(self, fnames):
        return [ self.executable_name ]

    def generate(self, depends, targets):
        targ = targets[0]
        to_exec = [ 'g++' ] + self.flags

        if len(self.lib_dirs) > 0:
            lib_dirs = [ '-L'+d for d in self.lib_dirs ]
            to_exec = to_exec + lib_dirs

        to_exec = to_exec + depends + self.post_flags

        if len(self.libraries) > 0:
            libs = [ '-l'+l for l in self.libraries ]
            to_exec = to_exec + libs

        to_exec = to_exec + [ '-o', targ ]
        return [ to_exec ]

class o2staticlibRule(Rule):
    def __init__(self):
        super().__init__('.o->.a')
        self.library_name = 'mylib.a'
        self.lib_dir = None
        self.flags = []

    def is_valid(self, fname):
        (_, ext) = os.path.splitext(fname)
        return ext == '.o'

    def generate_targets(self, fnames):
        if self.lib_dir is None:
            libname = self.library_name
        else:
            libname = os.path.join(self.lib_dir, self.library_name)
            ensure_dir_exists(self.lib_dir)
        return [ libname ]

    def generate(self, depends, targets):
        targ = targets[0]
        to_exec = [ 'ar', 'rvs' ] + self.flags + [ targ ] + depends
        return [ to_exec ]

cpp2o_rule = cpp2oRule()
o2exe_rule = o2exeRule()
o2static_lib_rule = o2staticlibRule()


class FileType(object):
    _types = {}
    def __init__(self, name, *rules):
        if name in FileType._types:
            raise Exception("Error: a target type with the name '{}' already exists".format(name))

        self.name = name
        self.rules = rules

        FileType._types[name] = self

    def run(self, fnames):
        results = []
        for fname in fnames:
            depends = [ fname ]
            for rule in self.rules:
                depends = rule.run(depends)
            results.extend(depends)
        return results

    def clean(self, fnames, clean_list):
        results = []
        for fname in fnames:
            depends = [ fname ]
            for rule in self.rules:
                depends = rule.clean(depends, clean_list)
            results.extend(depends)
        return results

cpp_file_type = FileType('cpp', cpp2o_rule)


ignore_extensions = [ '.temp', '.tmp', '.swp', '.ignore' ]
ignore_filenames = [ 'Makefile' ]


class Target(object):
    _targets = {}

    def __init__(self, name, fileType, rule):
        if name in Target._targets:
            raise Exception("Error: a target with that name already exists!")

        self.name = name
        self.fileType = fileType
        self.rule = rule
        self.depends = []

        Target._targets[name] = self

    def Add(self, path, force=False):
        if not os.path.exists(path):
            raise Exception("Error: could not find file '{}'".format(path))

        path = os.path.abspath(path)

        if os.path.isdir(path):
            for f in os.listdir(path):
                ff = os.path.join(path, f)
                self.Add(ff, force)
            return

        if os.path.isfile(path):
            if not force and self._should_ignore_file(path):
                print("Warning: ignoring file '{}'".format(path))
                return
            self.depends.append(path)

    def _should_ignore_file(self, fpath):
        if len(fpath) <= 0:
            return False

        fname = os.path.basename(fpath)

        if fname[0] == '.': return True
        if fname in ignore_filenames: return True

        (_, ext) = os.path.splitext(fname)
        if ext in ignore_extensions:
            return True

        return False

    def run(self):
        results = self.fileType.run(self.depends)
        finished = self.rule.run(results)
        print("Success!")
        return finished

    def clean(self):
        clean_list = []
        results = self.fileType.clean(self.depends, clean_list)
        finished = self.rule.clean(results, clean_list)

        ex = [ 'rm', '-f' ] + clean_list
        print(' '.join(ex))
        ret = subprocess.call(ex)
        if ret != 0:
            raise Exception("An error occured while cleaning".format(self.name))

default_cpp_target = Target("default_cpp", cpp_file_type, o2exe_rule)
default_static_lib_target = Target("static_lib", cpp_file_type, o2static_lib_rule)

default_target = None

def set_default_target(target):
    global default_target
    default_target = target
    ensure_build_dir_exists()

def make():
    target = default_target
    clean = False
    if len(sys.argv) > 1:
        if sys.argv[1] == "clean":
            clean = True
            if len(sys.argv) > 2:
                targ_name = sys.argv[2]
                target = Target._targets[targ_name]
        else:
            targ_name = sys.argv[1]
            target = Target._targets[targ_name]

    if clean:
        target.clean()
    else:
        target.run()



### My stuff (not part of the library being built above)

import shlex

def call_llvm_config(*flags):
    flags = list(flags)
    pobj = subprocess.Popen(['llvm-config'] + flags, stdout=PIPE, universal_newlines=True)
    (result, _) = pobj.communicate()
    if pobj.returncode != 0:
        print("Error calling llvm-config...")
        return None

    return shlex.split(result)




def main():
    llvm_compile_flags = call_llvm_config('--cxxflags')
    llvm_link_flags = call_llvm_config('--ldflags', '--libs', 'core')

    if llvm_compile_flags is None or llvm_link_flags is None:
        return

    default_cpp_target.Add('src/')
    default_cpp_target.Add('main.cpp')
    set_default_target(default_cpp_target)

    cpp2o_rule.include_dirs.append('include')
    cpp2o_rule.include_dirs.append('public')
    cpp2o_rule.flags.append('-std=c++11')
    cpp2o_rule.flags.append('-Wall')
    cpp2o_rule.flags.append('-g')
    cpp2o_rule.flags.extend(llvm_compile_flags)
    cpp2o_rule.flags.append('-fexceptions') # because llvm is dumb

    o2exe_rule.executable_name = "parser"
    o2exe_rule.post_flags.extend(llvm_link_flags)
    o2exe_rule.libraries.append('pthread')
    o2exe_rule.libraries.append('dl')


    #o2static_lib_rule.library_name = "libspark.a"
    #o2static_lib_rule.lib_dir = "lib"

    #libtarg = Target("lib", cpp_file_type, o2static_lib_rule)
    #libtarg.Add('src/')

    #make_exe = o2exeRule()
    #with_lib_targ = Target("with_lib", cpp_file_type, make_exe)
    #with_lib_targ.Add('main.cpp')
    #make_exe.executable_name = "parser"
    #make_exe.lib_dirs.append("lib")
    #make_exe.libraries.append("spark")

    make()



if __name__ == "__main__":
    main()
